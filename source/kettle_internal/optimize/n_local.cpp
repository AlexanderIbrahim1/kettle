#include <variant>
#include <vector>

#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/compound_gate.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/optimize/n_local.hpp>
#include <kettle/parameter/parameter.hpp>

#include <kettle_internal/gates/compound_gate/gate_id.hpp>
#include <kettle_internal/gates/compound_gate_map.hpp>
#include <kettle_internal/gates/primitive_gate/gate_id.hpp>
#include <kettle_internal/gates/primitive_gate_map.hpp>

// TODO: add ability to set the initial parameters of the circuit
// - or maybe do it post-initialization?
namespace gid = ket::internal::gate_id;
namespace kp = ket::param;

namespace 
{

void verify_valid_rotation_gates_(const std::vector<ket::GeneralGate>& gates)
{
    for (auto gen_gate : gates) {
        if (std::holds_alternative<ket::Gate>(gen_gate)) {
            const auto gate = std::get<ket::Gate>(gen_gate);
            if (gate == ket::Gate::U || gate == ket::Gate::CU || gate == ket::Gate::M) {
                throw std::runtime_error {"ERROR: cannot create n-local circuit with U, CU, or M gates.\n"};
            }
        }
// basically not needed right now; all CompoundGates are valid
//         else if (std::holds_alternative<ket::CompoundGate>(gen_gate)) {
//             // NOLINT
//         }
//         else {}
    }
}

void verify_valid_entanglement_gates_(const std::vector<ket::GeneralGate>& gates)
{
    for (auto gen_gate : gates) {
        if (std::holds_alternative<ket::Gate>(gen_gate)) {
            const auto gate = std::get<ket::Gate>(gen_gate);
            if (!gid::is_1c1t_gate(gate) && !gid::is_1c1t1a_gate(gate)) {
                throw std::runtime_error {"ERROR: entanglement gate must be controlled\n"};
            }
        }
// basically not needed right now; all CompoundGates are valid
//         else if (std::holds_alternative<ket::CompoundGate>(gen_gate)) {
//             // NOLINT
//         }
//         else {}
    }
}

void apply_rotation_gates_1t_(ket::Gate gate, ket::QuantumCircuit& circuit)
{
    const auto func = ket::internal::GATE_TO_FUNCTION_1T.at(gate);
    for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
        (circuit.*func)(i_target);
    }
}

void apply_rotation_gates_1t1a_(ket::Gate gate, ket::QuantumCircuit& circuit, std::vector<kp::ParameterID>& parameter_ids)
{
    constexpr auto key = kp::parameterized {};
    const auto func = ket::internal::GATE_TO_FUNCTION_1T1A_INIT_PARAM.at(gate);
    for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
        parameter_ids.emplace_back((circuit.*func)(i_target, ket::DEFAULT_NLOCAL_GATE_PARAMETER, key));
    }
}

void apply_rotation_gates_1c1t_(ket::Gate gate, ket::QuantumCircuit& circuit)
{
    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T.at(gate);
    for (std::size_t i {0}; i < (circuit.n_qubits() / 2); ++i) {
        const auto i_control = 2 * i;
        const auto i_target = i_control + 1;
        (circuit.*func)(i_control, i_target);
    }
}

void apply_rotation_gates_1c1t1a_(ket::Gate gate, ket::QuantumCircuit& circuit, std::vector<kp::ParameterID>& parameter_ids)
{
    constexpr auto key = kp::parameterized {};
    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T1A_INIT_PARAM.at(gate);
    for (std::size_t i {0}; i < (circuit.n_qubits() / 2); ++i) {
        const auto i_control = 2 * i;
        const auto i_target = i_control + 1;
        parameter_ids.emplace_back((circuit.*func)(i_control, i_target, ket::DEFAULT_NLOCAL_GATE_PARAMETER, key));
    }
}

void apply_rotation_gates_2c1t_(ket::CompoundGate gate, ket::QuantumCircuit& circuit)
{
    const auto func = ket::internal::GATE_TO_FUNCTION_2C1T.at(gate);
    for (std::size_t i {0}; i < (circuit.n_qubits() / 3); ++i) {
        const auto control0 = 3 * i;
        const auto control1 = (3 * i) + 1;
        const auto target = (3 * i) + 2;
        (circuit.*func)(control0, control1, target);
    }
}

auto apply_rotation_gates_(
    ket::QuantumCircuit& circuit,
    const std::vector<ket::GeneralGate>& rotation_blocks
) -> std::vector<kp::ParameterID>
{
    auto parameter_ids = std::vector<kp::ParameterID> {};
    
    for (auto gen_gate : rotation_blocks) {
        if (std::holds_alternative<ket::Gate>(gen_gate)) {
            const auto gate = std::get<ket::Gate>(gen_gate);
            if (gid::is_1t_gate(gate)) {
                apply_rotation_gates_1t_(gate, circuit);
            }
            else if (gid::is_1t1a_gate(gate)) {
                apply_rotation_gates_1t1a_(gate, circuit, parameter_ids);
            }
            else if (gid::is_1c1t_gate(gate)) {
                apply_rotation_gates_1c1t_(gate, circuit);
            }
            else if (gid::is_1c1t1a_gate(gate)) {
                apply_rotation_gates_1c1t1a_(gate, circuit, parameter_ids);
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid `Gate` found when applying rotation gates.\n"};
            }
        }
        else if (std::holds_alternative<ket::CompoundGate>(gen_gate)) {
            const auto gate = std::get<ket::CompoundGate>(gen_gate);
            if (gid::is_doubly_controled_gate(gate)) {
                apply_rotation_gates_2c1t_(gate, circuit);
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid `CompoundGate` found when applying rotation gates.\n"};
            }
        }
        else {
            throw std::runtime_error {"DEV ERROR: invalid `GeneralGate` found when applying rotation gates.\n"};
        }
    }

    return parameter_ids;
}

void apply_entanglement_gates_1c1t_(
    ket::Gate gate,
    ket::QuantumCircuit& circuit,
    ket::NLocalEntangelement entanglement_kind
)
{
    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T.at(gate);

    if (entanglement_kind == ket::NLocalEntangelement::LINEAR) {
        for (std::size_t i_qubit {0}; i_qubit < circuit.n_qubits() - 1; ++i_qubit) {
            (circuit.*func)(i_qubit, i_qubit + 1);
        }
    }
    else if (entanglement_kind == ket::NLocalEntangelement::FULL) {
        for (std::size_t i_control {0}; i_control < circuit.n_qubits() - 1; ++i_control) {
            for (std::size_t i_target {i_control + 1}; i_target < circuit.n_qubits(); ++i_target) {
                (circuit.*func)(i_control, i_target);
            }
        }
    }
    else {
        throw std::runtime_error {"DEV ERROR: Invalid entanglement chosen for applying 1c1t entanglement gates.\n"};
    }
}

void apply_entanglement_gates_1c1t1a_(
    ket::Gate gate,
    ket::QuantumCircuit& circuit,
    std::vector<kp::ParameterID>& parameter_ids,
    ket::NLocalEntangelement entanglement_kind
)
{
    constexpr auto key = kp::parameterized {};
    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T1A_INIT_PARAM.at(gate);

    if (entanglement_kind == ket::NLocalEntangelement::LINEAR) {
        for (std::size_t i_qubit {0}; i_qubit < circuit.n_qubits() - 1; ++i_qubit) {
            parameter_ids.emplace_back((circuit.*func)(i_qubit, i_qubit + 1, ket::DEFAULT_NLOCAL_GATE_PARAMETER, key));
        }
    }
    else if (entanglement_kind == ket::NLocalEntangelement::FULL) {
        for (std::size_t i_control {0}; i_control < circuit.n_qubits() - 1; ++i_control) {
            for (std::size_t i_target {i_control + 1}; i_control < circuit.n_qubits(); ++i_target) {
                parameter_ids.emplace_back((circuit.*func)(i_control, i_target, ket::DEFAULT_NLOCAL_GATE_PARAMETER, key));
            }
        }
    }
    else {
        throw std::runtime_error {"DEV ERROR: Invalid entanglement chosen for applying 1c1t1a entanglement gates.\n"};
    }
}

// TODO: find a way to get rid of all of this repetition, after I get it working!
// - probably by making it a circuit member function
void apply_entanglement_gates_2c1t_(
    ket::CompoundGate gate,
    ket::QuantumCircuit& circuit,
    ket::NLocalEntangelement entanglement_kind
)
{
    const auto func = ket::internal::GATE_TO_FUNCTION_2C1T.at(gate);

    if (entanglement_kind == ket::NLocalEntangelement::LINEAR) {
        for (std::size_t i_qubit {0}; i_qubit < circuit.n_qubits() - 2; ++i_qubit) {
            (circuit.*func)(i_qubit, i_qubit + 1, i_qubit + 2);
        }
    }
    else if (entanglement_kind == ket::NLocalEntangelement::FULL) {
        for (std::size_t i_control0 {0}; i_control0 < circuit.n_qubits() - 2; ++i_control0) {
            for (std::size_t i_control1 {i_control0 + 1}; i_control1 < circuit.n_qubits() - 1; ++i_control1) {
                for (std::size_t i_target {i_control1 + 1}; i_target < circuit.n_qubits(); ++i_target) {
                    (circuit.*func)(i_control0, i_control1, i_target);
                }
            }
        }
    }
    else {
        throw std::runtime_error {"DEV ERROR: Invalid entanglement chosen for applying 2c1t entanglement gates.\n"};
    }

}

auto apply_entanglement_gates_(
    ket::QuantumCircuit& circuit,
    const std::vector<ket::GeneralGate>& entanglement_blocks,
    ket::NLocalEntangelement entanglement_kind
) -> std::vector<kp::ParameterID>
{
    auto parameter_ids = std::vector<kp::ParameterID> {};
    
    for (auto gen_gate : entanglement_blocks) {
        if (std::holds_alternative<ket::Gate>(gen_gate)) {
            const auto gate = std::get<ket::Gate>(gen_gate);
            if (gid::is_1c1t_gate(gate)) {
                apply_entanglement_gates_1c1t_(gate, circuit, entanglement_kind);
            }
            else if (gid::is_1c1t1a_gate(gate)) {
                apply_entanglement_gates_1c1t1a_(gate, circuit, parameter_ids, entanglement_kind);
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid `Gate` found when applying entanglement gates.\n"};
            }
        }
        else if (std::holds_alternative<ket::CompoundGate>(gen_gate)) {
            const auto gate = std::get<ket::CompoundGate>(gen_gate);
            if (gid::is_doubly_controled_gate(gate)) {
                apply_entanglement_gates_2c1t_(gate, circuit, entanglement_kind);
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid `CompoundGate` found when applying entanglement gates.\n"};
            }
        }
        else {
            throw std::runtime_error {"DEV ERROR: invalid `GeneralGate` found when applying entanglement gates.\n"};
        }
    }

    return parameter_ids;
}

}  // namespace


namespace ket
{

auto n_local(
    std::size_t n_qubits,
    const std::vector<GeneralGate>& rotation_blocks,
    const std::vector<GeneralGate>& entanglement_blocks,
    NLocalEntangelement entanglement_kind,
    std::size_t n_repetitions,
    SkipLastRotationLayerFlag flag
) -> std::tuple<QuantumCircuit, std::vector<kp::ParameterID>>
{
    verify_valid_rotation_gates_(rotation_blocks);
    verify_valid_entanglement_gates_(entanglement_blocks);

    auto circuit = QuantumCircuit {n_qubits};

    auto parameter_ids = std::vector<kp::ParameterID> {};

    for (std::size_t i {0}; i < n_repetitions; ++i) {
        const auto rotation_param_ids= apply_rotation_gates_(circuit, rotation_blocks);
        parameter_ids.insert(parameter_ids.end(), rotation_param_ids.begin(), rotation_param_ids.end());

        const auto entanglement_param_ids= apply_entanglement_gates_(circuit, entanglement_blocks, entanglement_kind);
        parameter_ids.insert(parameter_ids.end(), entanglement_param_ids.begin(), entanglement_param_ids.end());
    }

    if (flag == SkipLastRotationLayerFlag::FALSE) {
        const auto rotation_param_ids= apply_rotation_gates_(circuit, rotation_blocks);
        parameter_ids.insert(parameter_ids.end(), rotation_param_ids.begin(), rotation_param_ids.end());
    }

    return {circuit, parameter_ids};
}

}  // namespace ket

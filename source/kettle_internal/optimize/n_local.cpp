#include <vector>

#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/optimize/n_local.hpp>
#include <kettle/parameter/parameter.hpp>

#include <kettle_internal/gates/primitive_gate/gate_id.hpp>
#include <kettle_internal/gates/primitive_gate_map.hpp>

// TODO: add ability to set the initial parameters of the circuit
// - or maybe do it post-initialization?

namespace 
{

inline constexpr auto DEFAULT_ROTATION_GATE_PARAMETER = double {0.0};

void verify_valid_gates(const std::vector<ket::Gate>& gates)
{
    for (auto gate : gates) {
        if (gate == ket::Gate::U || gate == ket::Gate::CU || gate == ket::Gate::M) {
            throw std::runtime_error {"ERROR: cannot create n-local circuit with U, CU, or M gates.\n"};
        }
    }
}

auto apply_rotation_gates(
    ket::QuantumCircuit& circuit,
    const std::vector<ket::Gate>& rotation_blocks
) -> std::vector<ket::param::ParameterID>
{
    namespace gid = ket::internal::gate_id;

    auto parameter_ids = std::vector<ket::param::ParameterID> {};

    constexpr auto key = ket::param::parameterized {};
    constexpr auto param = DEFAULT_ROTATION_GATE_PARAMETER;
    
    for (auto gate : rotation_blocks) {
        if (gid::is_1t_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1T.at(gate);

            for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
                (circuit.*func)(i_target);
            }
        }
        else if (gid::is_1t1a_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1T1A_INIT_PARAM.at(gate);

            for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
                parameter_ids.emplace_back((circuit.*func)(i_target, param, key));
            }
        }
        else if (gid::is_1c1t_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1C1T.at(gate);

            for (std::size_t i {0}; i < (circuit.n_qubits() / 2); ++i) {
                const auto i_control = 2 * i;
                const auto i_target = i_control + 1;
                (circuit.*func)(i_control, i_target);
            }
        }
        else if (gid::is_1c1t1a_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1C1T1A_INIT_PARAM.at(gate);

            for (std::size_t i {0}; i < (circuit.n_qubits() / 2); ++i) {
                const auto i_control = 2 * i;
                const auto i_target = i_control + 1;
                parameter_ids.emplace_back((circuit.*func)(i_control, i_target, param, key));
            }
        }
        else {
            throw std::runtime_error {"DEV ERROR: invalid gate found when applying rotation gates.\n"};
        }
    }

    return parameter_ids;
}

}  // namespace


namespace ket
{


}  // namespace ket

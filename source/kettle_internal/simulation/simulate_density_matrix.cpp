#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <Eigen/Dense>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle/simulation/simulate_density_matrix.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/measure_density_matrix.hpp"
#include "kettle_internal/simulation/operations_density_matrix.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"


namespace ki = ket::internal;
namespace kpi = ket::param::internal;

namespace
{

/*
    Helper struct for the static_assert(), to see what ket::Gate instance is passed that causes
    the template instantiation to fail.
*/
template <ket::Gate GateType>
struct gate_always_false : std::false_type
{};

constexpr inline auto MEASURING_THREAD_ID = int {0};

template <ket::Gate GateType>
void simulate_one_target_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    namespace cre = ki::create;

    const auto target_index = static_cast<Eigen::Index>(cre::unpack_single_qubit_gate_index(info));
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = ki::SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    ki::apply_1t_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    ki::apply_1t_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);
}


template <ket::Gate GateType>
void simulate_one_target_one_angle_gate_(
    const kpi::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto [target_index_st, theta] = kpi::unpack_target_and_angle(parameter_values_map, info);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = ki::SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    ki::apply_1t1a_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    ki::apply_1t1a_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);
}


void simulate_u_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto target_index = static_cast<Eigen::Index>(ki::create::unpack_single_qubit_gate_index(info));
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = ki::SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    ki::apply_u_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);

    const auto mat_adj = ket::conjugate_transpose(mat);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    ki::apply_u_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);
}


template <ket::Gate GateType>
void simulate_one_control_one_target_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    namespace cre = ki::create;

    const auto [control_index_st, target_index_st] = cre::unpack_double_qubit_gate_indices(info);
    const auto control_index = static_cast<Eigen::Index>(control_index_st);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    auto pair_iterator_inner = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    ki::apply_1c1t_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    ki::apply_1c1t_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);
}


template <ket::Gate GateType>
void simulate_one_control_one_target_one_angle_gate_(
    const kpi::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto [control_index_st, target_index_st, theta] = kpi::unpack_control_target_and_angle(parameter_values_map, info);
    const auto control_index = static_cast<Eigen::Index>(control_index_st);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    auto pair_iterator_inner = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    ki::apply_1c1t1a_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    ki::apply_1c1t1a_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);
}


void simulate_cu_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
    const ki::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto [control_index_st, target_index_st] = ki::create::unpack_double_qubit_gate_indices(info);
    const auto control_index = static_cast<Eigen::Index>(control_index_st);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);

    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    auto pair_iterator_inner = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};

    // perform the multiplication of U * rho; fill the buffer
    ki::apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);

    const auto mat_adj = ket::conjugate_transpose(mat);

    // perform the multiplication of (U * rho) * U^t; write the result to the density matrix itself
    ki::apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);
}


void simulate_gate_info_(
    [[maybe_unused]] const kpi::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const ki::FlatIndexPair<Eigen::Index>& single_pair,
    [[maybe_unused]] const ki::FlatIndexPair<Eigen::Index>& double_pair,
    const ket::GateInfo& gate_info,
    [[maybe_unused]] int thread_id,
    [[maybe_unused]] std::optional<int> prng_seed,
    [[maybe_unused]] ket::ClassicalRegister& c_register,
    Eigen::MatrixXcd& buffer
)
{
    namespace cre = ki::create;
    using G = ket::Gate;

    switch (gate_info.gate) {
        case G::H : {
            simulate_one_target_gate_<G::H>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::X : {
            simulate_one_target_gate_<G::X>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::Y : {
            simulate_one_target_gate_<G::Y>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::Z : {
            simulate_one_target_gate_<G::Z>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::S : {
            simulate_one_target_gate_<G::S>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SDAG : {
            simulate_one_target_gate_<G::SDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::T : {
            simulate_one_target_gate_<G::T>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::TDAG : {
            simulate_one_target_gate_<G::TDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SX : {
            simulate_one_target_gate_<G::SX>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SXDAG : {
            simulate_one_target_gate_<G::SXDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::RX : {
            simulate_one_target_one_angle_gate_<G::RX>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::RY : {
            simulate_one_target_one_angle_gate_<G::RY>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::RZ : {
            simulate_one_target_one_angle_gate_<G::RZ>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::P : {
            simulate_one_target_one_angle_gate_<G::P>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::CH : {
            simulate_one_control_one_target_gate_<G::CH>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CX : {
            simulate_one_control_one_target_gate_<G::CX>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CY : {
            simulate_one_control_one_target_gate_<G::CY>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CZ : {
            simulate_one_control_one_target_gate_<G::CZ>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CS : {
            simulate_one_control_one_target_gate_<G::CS>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSDAG : {
            simulate_one_control_one_target_gate_<G::CSDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CT : {
            simulate_one_control_one_target_gate_<G::CT>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CTDAG : {
            simulate_one_control_one_target_gate_<G::CTDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSX : {
            simulate_one_control_one_target_gate_<G::CSX>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSXDAG : {
            simulate_one_control_one_target_gate_<G::CSXDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRX : {
            simulate_one_control_one_target_one_angle_gate_<G::CRX>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRY : {
            simulate_one_control_one_target_one_angle_gate_<G::CRY>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRZ : {
            simulate_one_control_one_target_one_angle_gate_<G::CRZ>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CP : {
            simulate_one_control_one_target_one_angle_gate_<G::CP>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::U : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            simulate_u_gate_(state, gate_info, *unitary_ptr, single_pair, buffer);
            break;
        }
        case G::CU : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            simulate_cu_gate_(state, gate_info, *unitary_ptr, double_pair, buffer);
            break;
        }
        case G::M : {
            // this operation is more complicated to make multithreaded because the threads have already been
            // spawned before entering the simulation loop; thus, it is easier to just make the measurement
            // a single-threaded operation
            if (thread_id == MEASURING_THREAD_ID) {
                [[maybe_unused]]
                const auto [ignore, bit_index] = cre::unpack_m_gate(gate_info);
                const auto measured = ki::simulate_measurement_(state, gate_info, prng_seed);
                c_register.set(bit_index, measured);
            }
            break;
        }
        default : {
            break;
        }
    }
}

auto simulate_loop_body_iterative_(  // NOLINT(readability-function-cognitive-complexity)
    const ket::QuantumCircuit& circuit,
    ket::DensityMatrix& state,
    const ki::FlatIndexPair<Eigen::Index>& single_pair,
    const ki::FlatIndexPair<Eigen::Index>& double_pair,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& cregister,
    Eigen::MatrixXcd& buffer
) -> std::vector<ket::CircuitLogger>
{
    using Elements = std::reference_wrapper<const std::vector<ket::CircuitElement>>;

    auto elements_stack = std::vector<Elements> {};
    elements_stack.push_back(std::ref(circuit.circuit_elements()));

    auto instruction_pointers = std::vector<std::size_t> {};
    instruction_pointers.push_back(0);

    auto circuit_loggers = std::vector<ket::CircuitLogger> {};

    const auto& parameter_values_map = kpi::create_parameter_values_map(circuit.parameter_data_map());

    while (elements_stack.size() != 0) {
        const auto& elements = elements_stack.back();
        const auto i_ptr = instruction_pointers.back();

        ++instruction_pointers.back();

        if (i_ptr >= elements.get().size()) {
            elements_stack.pop_back();
            instruction_pointers.pop_back();
            continue;
        }

        const auto& element = elements.get()[i_ptr];

        if (element.is_circuit_logger()) {
            throw std::runtime_error {"DEV ERROR: loggers haven't been implemented yet for the density matrix simulator.\n"};
//             const auto& logger = element.get_circuit_logger();
// 
//             if (logger.is_classical_register_circuit_logger()) {
//                 auto cregister_logger = logger.get_classical_register_circuit_logger();
//                 cregister_logger.add_classical_register(cregister);
//                 circuit_loggers.emplace_back(std::move(cregister_logger));
//             }
//             else if (logger.is_statevector_circuit_logger()) {
//                 auto statevector_logger = logger.get_statevector_circuit_logger();
//                 statevector_logger.add_statevector(state);
//                 circuit_loggers.emplace_back(std::move(statevector_logger));
//             }
//             else {
//                 throw std::runtime_error {"DEV ERROR: unimplemented circuit logger in `simulate_loop_body_iterative_()`\n"};
//             }
        }
        else if (element.is_control_flow()) {
            const auto& control_flow = element.get_control_flow();

            if (control_flow.is_if_statement()) {
                const auto& if_stmt = control_flow.get_if_statement();

                if (if_stmt(cregister)) {
                    const auto& subcircuit = *if_stmt.circuit();
                    elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                    instruction_pointers.push_back(0);
                }
            }
            else if (control_flow.is_if_else_statement()) {
                const auto& if_else_stmt = control_flow.get_if_else_statement();

                // NOTE: omitting the return type here causes a dangling reference
                const auto& subcircuit = [&]() -> const ket::QuantumCircuit& {
                    if (if_else_stmt(cregister)) {
                        return *if_else_stmt.if_circuit();
                    } else {
                        return *if_else_stmt.else_circuit();
                    }
                }();

                elements_stack.push_back(std::ref(subcircuit.circuit_elements()));
                instruction_pointers.push_back(0);
            }
            else {
                throw std::runtime_error {"DEV ERROR: unimplemented control flow in `simulate_loop_body_iterative_()`\n"};
            }
        }
        else if (element.is_gate()) {
            const auto gate_info = element.get_gate();

            simulate_gate_info_(
                parameter_values_map,
                state,
                single_pair,
                double_pair,
                gate_info,
                thread_id,
                prng_seed,
                cregister,
                buffer
            );
        }
        else {
            throw std::runtime_error {"DEV ERROR: unimplemented circuit element in `simulate_loop_body_iterative_()`\n"};
        }
    }

    return circuit_loggers;
}

void check_valid_number_of_qubits_(const ket::QuantumCircuit& circuit, const ket::DensityMatrix& state)
{
    if (circuit.n_qubits() != state.n_qubits()) {
        throw std::runtime_error {"Invalid simulation; circuit and state have different number of qubits."};
    }

    if (circuit.n_qubits() == 0) {
        throw std::runtime_error {"Cannot simulate a circuit or state with zero qubits."};
    }
}

}  // namespace

namespace ket
{

DensityMatrixSimulator::DensityMatrixSimulator(std::size_t n_qubits)
{
    if (n_qubits == 0) {
        throw std::runtime_error {"ERROR: cannot perform a DensityMatrix simulation with 0 qubits.\n"};
    }

    const auto n_states = static_cast<Eigen::Index>(1UL << n_qubits);

    buffer_ = Eigen::MatrixXcd(n_states, n_states);
}

void DensityMatrixSimulator::run(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed)
{
    namespace ki = ket::internal;

    check_valid_number_of_qubits_(circuit, state);

    const auto n_single_gate_pairs = static_cast<Eigen::Index>(ki::number_of_single_qubit_gate_pairs_(circuit.n_qubits()));
    const auto single_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_single_gate_pairs};

    const auto n_double_gate_pairs = static_cast<Eigen::Index>(ki::number_of_double_qubit_gate_pairs_(circuit.n_qubits()));
    const auto double_pair = ki::FlatIndexPair<Eigen::Index> {.i_lower=0, .i_upper=n_double_gate_pairs};

    cregister_ = ket::ClonePtr<ClassicalRegister> {ClassicalRegister {circuit.n_bits()}};

    // the `simulate_loop_body_()` function is used by both the single-threaded and multi-threaded
    // code, and certain operations are only done on the thread with thread id 0
    const auto thread_id = MEASURING_THREAD_ID;

    circuit_loggers_ = simulate_loop_body_iterative_(circuit, state, single_pair, double_pair, thread_id, prng_seed, *cregister_, buffer_);

    has_been_run_ = true;
}

[[nodiscard]]
auto DensityMatrixSimulator::has_been_run() const -> bool
{
    return has_been_run_;
}

[[nodiscard]]
auto DensityMatrixSimulator::classical_register() const -> const ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

auto DensityMatrixSimulator::classical_register() -> ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

[[nodiscard]]
auto DensityMatrixSimulator::circuit_loggers() const -> const std::vector<CircuitLogger>&
{
    return circuit_loggers_;
}

void simulate(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed)
{
    auto simulator = DensityMatrixSimulator {state.n_qubits()};
    simulator.run(circuit, state, prng_seed);
}


}  // namespace ket

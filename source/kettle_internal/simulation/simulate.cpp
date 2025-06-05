#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/state.hpp"

#include "kettle/simulation/simulate.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/measure.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/operations.hpp"


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
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair& pair
)
{
    namespace cre = ki::create;
    using Gate = ket::Gate;

    const auto target_index = cre::unpack_single_qubit_gate_index(info);
    const auto n_qubits = state.n_qubits();

    auto pair_iterator = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::H) {
            ki::apply_h_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::X) {
            ki::apply_x_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::Y) {
            ki::apply_y_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::Z) {
            ki::apply_z_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::S) {
            ki::apply_s_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::SDAG) {
            ki::apply_sdag_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::T) {
            ki::apply_t_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::TDAG) {
            ki::apply_tdag_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::SX) {
            ki::apply_sx_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::SXDAG) {
            ki::apply_sxdag_gate(state, state0_index, state1_index);
        }
        else {
            static_assert(gate_always_false<GateType>::value, "Invalid one target gate.");
        }
    }
}


template <ket::Gate GateType>
void simulate_one_target_one_angle_gate_(
    const kpi::MapVariant& parameter_values_map,
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair& pair
)
{
    using Gate = ket::Gate;

    const auto [target_index, theta] = kpi::unpack_target_and_angle(parameter_values_map, info);
    const auto n_qubits = state.n_qubits();

    auto pair_iterator = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::RX) {
            ki::apply_rx_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::RY) {
            ki::apply_ry_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::RZ) {
            ki::apply_rz_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::P) {
            ki::apply_p_gate(state, state1_index, theta);
        }
        else {
            static_assert(gate_always_false<GateType>::value, "Invalid one target one angle gate.");
        }
    }
}


void simulate_u_gate_(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
    const ki::FlatIndexPair& pair
)
{
    const auto target_index = ki::create::unpack_single_qubit_gate_index(info);
    const auto n_qubits = state.n_qubits();
    auto pair_iterator = ki::SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();
        ki::apply_u_gate(state, state0_index, state1_index, mat);
    }
}


template <ket::Gate GateType>
void simulate_one_control_one_target_gate_(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair& pair
)
{
    namespace cre = ki::create;
    using Gate = ket::Gate;

    const auto [control_index, target_index] = cre::unpack_double_qubit_gate_indices(info);
    const auto n_qubits = state.n_qubits();

    auto pair_iterator = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        [[maybe_unused]] const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::CH) {
            ki::apply_h_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CX) {
            ki::apply_x_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CY) {
            ki::apply_y_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CZ) {
            ki::apply_z_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CS) {
            ki::apply_s_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CSDAG) {
            ki::apply_sdag_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CT) {
            ki::apply_t_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CTDAG) {
            ki::apply_tdag_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CSX) {
            ki::apply_sx_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CSXDAG) {
            ki::apply_sxdag_gate(state, state0_index, state1_index);
        }
        else {
            static_assert(gate_always_false<GateType>::value, "Invalid one control one target gate.");
        }
    }
}


template <ket::Gate GateType>
void simulate_one_control_one_target_one_angle_gate_(
    const kpi::MapVariant& parameter_values_map,
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ki::FlatIndexPair& pair
)
{
    using Gate = ket::Gate;

    const auto [control_index, target_index, theta] = kpi::unpack_control_target_and_angle(parameter_values_map, info);
    const auto n_qubits = state.n_qubits();

    auto pair_iterator = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        [[maybe_unused]] const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::CRX) {
            ki::apply_rx_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CRY) {
            ki::apply_ry_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CRZ) {
            ki::apply_rz_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CP) {
            ki::apply_p_gate(state, state1_index, theta);
        }
        else {
            static_assert(gate_always_false<GateType>::value, "Invalid one control one target one angle gate.");
        }
    }
}


void simulate_cu_gate_(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
    const ki::FlatIndexPair& pair
)
{
    const auto [control_index, target_index] = ki::create::unpack_double_qubit_gate_indices(info);
    const auto n_qubits = state.n_qubits();
    auto pair_iterator = ki::DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();
        ki::apply_u_gate(state, state0_index, state1_index, mat);
    }
}


void simulate_gate_info_(
    const kpi::MapVariant& parameter_values_map,
    ket::QuantumState& state,
    const ki::FlatIndexPair& single_pair,
    const ki::FlatIndexPair& double_pair,
    const ket::GateInfo& gate_info,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& c_register
)
{
    namespace cre = ki::create;
    using G = ket::Gate;

    switch (gate_info.gate) {
        case G::H : {
            simulate_one_target_gate_<G::H>(state, gate_info, single_pair);
            break;
        }
        case G::X : {
            simulate_one_target_gate_<G::X>(state, gate_info, single_pair);
            break;
        }
        case G::Y : {
            simulate_one_target_gate_<G::Y>(state, gate_info, single_pair);
            break;
        }
        case G::Z : {
            simulate_one_target_gate_<G::Z>(state, gate_info, single_pair);
            break;
        }
        case G::S : {
            simulate_one_target_gate_<G::S>(state, gate_info, single_pair);
            break;
        }
        case G::SDAG : {
            simulate_one_target_gate_<G::SDAG>(state, gate_info, single_pair);
            break;
        }
        case G::T : {
            simulate_one_target_gate_<G::T>(state, gate_info, single_pair);
            break;
        }
        case G::TDAG : {
            simulate_one_target_gate_<G::TDAG>(state, gate_info, single_pair);
            break;
        }
        case G::SX : {
            simulate_one_target_gate_<G::SX>(state, gate_info, single_pair);
            break;
        }
        case G::SXDAG : {
            simulate_one_target_gate_<G::SXDAG>(state, gate_info, single_pair);
            break;
        }
        case G::RX : {
            simulate_one_target_one_angle_gate_<G::RX>(parameter_values_map, state, gate_info, single_pair);
            break;
        }
        case G::RY : {
            simulate_one_target_one_angle_gate_<G::RY>(parameter_values_map, state, gate_info, single_pair);
            break;
        }
        case G::RZ : {
            simulate_one_target_one_angle_gate_<G::RZ>(parameter_values_map, state, gate_info, single_pair);
            break;
        }
        case G::P : {
            simulate_one_target_one_angle_gate_<G::P>(parameter_values_map, state, gate_info, single_pair);
            break;
        }
        case G::CH : {
            simulate_one_control_one_target_gate_<G::CH>(state, gate_info, double_pair);
            break;
        }
        case G::CX : {
            simulate_one_control_one_target_gate_<G::CX>(state, gate_info, double_pair);
            break;
        }
        case G::CY : {
            simulate_one_control_one_target_gate_<G::CY>(state, gate_info, double_pair);
            break;
        }
        case G::CZ : {
            simulate_one_control_one_target_gate_<G::CZ>(state, gate_info, double_pair);
            break;
        }
        case G::CS : {
            simulate_one_control_one_target_gate_<G::CS>(state, gate_info, double_pair);
            break;
        }
        case G::CSDAG : {
            simulate_one_control_one_target_gate_<G::CSDAG>(state, gate_info, double_pair);
            break;
        }
        case G::CT : {
            simulate_one_control_one_target_gate_<G::CT>(state, gate_info, double_pair);
            break;
        }
        case G::CTDAG : {
            simulate_one_control_one_target_gate_<G::CTDAG>(state, gate_info, double_pair);
            break;
        }
        case G::CSX : {
            simulate_one_control_one_target_gate_<G::CSX>(state, gate_info, double_pair);
            break;
        }
        case G::CSXDAG : {
            simulate_one_control_one_target_gate_<G::CSXDAG>(state, gate_info, double_pair);
            break;
        }
        case G::CRX : {
            simulate_one_control_one_target_one_angle_gate_<G::CRX>(parameter_values_map, state, gate_info, double_pair);
            break;
        }
        case G::CRY : {
            simulate_one_control_one_target_one_angle_gate_<G::CRY>(parameter_values_map, state, gate_info, double_pair);
            break;
        }
        case G::CRZ : {
            simulate_one_control_one_target_one_angle_gate_<G::CRZ>(parameter_values_map, state, gate_info, double_pair);
            break;
        }
        case G::CP : {
            simulate_one_control_one_target_one_angle_gate_<G::CP>(parameter_values_map, state, gate_info, double_pair);
            break;
        }
        case G::U : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            simulate_u_gate_(state, gate_info, *unitary_ptr, single_pair);
            break;
        }
        case G::CU : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            simulate_cu_gate_(state, gate_info, *unitary_ptr, double_pair);
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
    }
}

auto simulate_loop_body_iterative_(  // NOLINT(readability-function-cognitive-complexity)
    const ket::QuantumCircuit& circuit,
    ket::QuantumState& state,
    const ki::FlatIndexPair& single_pair,
    const ki::FlatIndexPair& double_pair,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& cregister
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
            const auto& logger = element.get_circuit_logger();

            if (logger.is_classical_register_circuit_logger()) {
                auto cregister_logger = logger.get_classical_register_circuit_logger();
                cregister_logger.add_classical_register(cregister);
                circuit_loggers.emplace_back(std::move(cregister_logger));
            }
            else if (logger.is_statevector_circuit_logger()) {
                auto statevector_logger = logger.get_statevector_circuit_logger();
                statevector_logger.add_statevector(state);
                circuit_loggers.emplace_back(std::move(statevector_logger));
            }
            else {
                throw std::runtime_error {"DEV ERROR: unimplemented circuit logger in `simulate_loop_body_iterative_()`\n"};
            }
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
                cregister
            );
        }
        else {
            throw std::runtime_error {"DEV ERROR: unimplemented circuit element in `simulate_loop_body_iterative_()`\n"};
        }
    }

    return circuit_loggers;
}

void check_valid_number_of_qubits_(const ket::QuantumCircuit& circuit, const ket::QuantumState& state)
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

void StatevectorSimulator::run(const QuantumCircuit& circuit, QuantumState& state, std::optional<int> prng_seed)
{
    namespace ki = ket::internal;

    check_valid_number_of_qubits_(circuit, state);

    const auto n_single_gate_pairs = ki::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
    const auto single_pair = ki::FlatIndexPair {.i_lower=0, .i_upper=n_single_gate_pairs};

    const auto n_double_gate_pairs = ki::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
    const auto double_pair = ki::FlatIndexPair {.i_lower=0, .i_upper=n_double_gate_pairs};

    cregister_ = ket::ClonePtr<ClassicalRegister> {ClassicalRegister {circuit.n_bits()}};

    // the `simulate_loop_body_()` function is used by both the single-threaded and multi-threaded
    // code, and certain operations are only done on the thread with thread id 0
    const auto thread_id = MEASURING_THREAD_ID;

    circuit_loggers_ = simulate_loop_body_iterative_(circuit, state, single_pair, double_pair, thread_id, prng_seed, *cregister_);

    has_been_run_ = true;
}

[[nodiscard]]
auto StatevectorSimulator::has_been_run() const -> bool
{
    return has_been_run_;
}

[[nodiscard]]
auto StatevectorSimulator::classical_register() const -> const ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

auto StatevectorSimulator::classical_register() -> ClassicalRegister&
{
    if (!cregister_) {
        throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
    }

    return *cregister_;
}

[[nodiscard]]
auto StatevectorSimulator::circuit_loggers() const -> const std::vector<CircuitLogger>&
{
    return circuit_loggers_;
}

void simulate(const QuantumCircuit& circuit, QuantumState& state, std::optional<int> prng_seed)
{
    auto simulator = StatevectorSimulator {};
    simulator.run(circuit, state, prng_seed);
}


}  // namespace ket


// void simulate_multithreaded_loop_(
//     std::barrier<>& sync_point,
//     const ket::QuantumCircuit& circuit,
//     ket::QuantumState& state,
//     const FlatIndexPair& single_pair,
//     const FlatIndexPair& double_pair,
//     int thread_id,
//     std::optional<int> prng_seed,
//     ket::ClassicalRegister& c_register
// )
// {
//     for (const auto& element : circuit) {
//         simulate_loop_body_(state, single_pair, double_pair, element, thread_id, prng_seed, c_register);
//         sync_point.arrive_and_wait();
//     }
// }
// /*
//     WARNING: the current multithreaded implementation is slower than the singlethreaded implementation;
//     I'm not sure of the reasons yet (too much waiting at the barrier, multiple states per cache line, etc.)
// 
//     A quick benchmark shows that the threads spend a large amount of time waiting.
// */
// void simulate_multithreaded(
//     const QuantumCircuit& circuit,
//     QuantumState& state,
//     std::size_t n_threads,
//     std::optional<int> prng_seed = std::nullopt
// )
// {
//     if (n_threads == 0) {
//         throw std::runtime_error {"Cannot perform simulation with 0 threads.\n"};
//     }
// 
//     im::check_valid_number_of_qubits_(circuit, state);
// 
//     const auto n_single_gate_pairs = im::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
//     const auto single_flat_index_pairs = im::partial_sum_pairs_(n_single_gate_pairs, n_threads);
// 
//     const auto n_double_gate_pairs = im::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
//     const auto double_flat_index_pairs = im::partial_sum_pairs_(n_double_gate_pairs, n_threads);
// 
//     auto c_register = ClassicalRegister {circuit.n_bits()};
// 
//     auto threads = std::vector<std::jthread> {};
//     threads.reserve(n_threads);
// 
//     auto barrier = std::barrier {static_cast<std::ptrdiff_t>(n_threads)};
// 
//     for (std::size_t i {0}; i < n_threads; ++i) {
//         threads.emplace_back(
//             im::simulate_multithreaded_loop_,
//             std::ref(barrier),
//             std::ref(circuit),
//             std::ref(state),
//             single_flat_index_pairs[i],
//             double_flat_index_pairs[i],
//             i,
//             prng_seed,
//             std::ref(c_register)
//         );
//     }
// 
//     for (auto& thread : threads) {
//         thread.join();
//     }
// }

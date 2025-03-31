#pragma once

// TODO: remove
#include <iostream>

#include <algorithm>
#include <barrier>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <vector>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/simulation/gate_pair_generator.hpp"
#include "mini-qiskit/simulation/simulate_utils.hpp"
#include "mini-qiskit/simulation/multithread_simulate_utils.hpp"
#include "mini-qiskit/simulation/operations.hpp"
#include "mini-qiskit/state.hpp"

namespace impl_mqis
{

template <mqis::Gate GateType>
void simulate_single_qubit_gate_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    const FlatIndexPair& pair
)
{
    using Gate = mqis::Gate;

    const auto target_index = unpack_single_qubit_gate_index(info);

    auto pair_iterator = SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::H) {
            apply_h_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::X) {
            apply_x_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::Y) {
            apply_y_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::Z) {
            apply_z_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::RX) {
            [[maybe_unused]] const auto [ignore, theta] = unpack_one_target_one_angle_gate(info);
            apply_rx_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::RY) {
            [[maybe_unused]] const auto [ignore, theta] = unpack_one_target_one_angle_gate(info);
            apply_ry_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::RZ) {
            [[maybe_unused]] const auto [ignore, theta] = unpack_one_target_one_angle_gate(info);
            apply_rz_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::P) {
            [[maybe_unused]] const auto [ignore, theta] = unpack_one_target_one_angle_gate(info);
            apply_p_gate(state, state1_index, theta);
        }
        else {
            static_assert(impl_mqis::always_false<void>::value, "Invalid single qubit gate");
        }
    }
}

inline void simulate_single_qubit_gate_general_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    const mqis::Matrix2X2& mat,
    const FlatIndexPair& pair
)
{
    const auto target_index = unpack_single_qubit_gate_index(info);
    auto pair_iterator = SingleQubitGatePairGenerator {target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();
        apply_u_gate(state, state0_index, state1_index, mat);
    }
}

template <mqis::Gate GateType>
void simulate_double_qubit_gate_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    const FlatIndexPair& pair
)
{
    using Gate = mqis::Gate;

    const auto [control_index, target_index] = unpack_double_qubit_gate_indices(info);

    auto pair_iterator = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        [[maybe_unused]] const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::CX) {
            apply_x_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CY) {
            apply_y_gate(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CZ) {
            apply_z_gate(state, state1_index);
        }
        else if constexpr (GateType == Gate::CRX) {
            [[maybe_unused]] const auto [ignore0, ignore1, theta] = unpack_one_control_one_target_one_angle_gate(info);
            apply_rx_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CRY) {
            [[maybe_unused]] const auto [ignore0, ignore1, theta] = unpack_one_control_one_target_one_angle_gate(info);
            apply_ry_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CRZ) {
            [[maybe_unused]] const auto [ignore0, ignore1, theta] = unpack_one_control_one_target_one_angle_gate(info);
            apply_rz_gate(state, state0_index, state1_index, theta);
        }
        else if constexpr (GateType == Gate::CP) {
            // NOTE: the DoubleQubitGatePairGenerator needs to calculate the `state0_index` before
            // it calculates the `state1_index`, so we're not losing too much in terms of performance
            [[maybe_unused]] const auto [ignore0, ignore1, theta] = unpack_one_control_one_target_one_angle_gate(info);
            apply_p_gate(state, state1_index, theta);
        }
        else {
            static_assert(impl_mqis::always_false<void>::value, "Invalid double qubit gate: must be one of {CX, CRX}");
        }
    }
}

inline void simulate_double_qubit_gate_general_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    const mqis::Matrix2X2& mat,
    const FlatIndexPair& pair
)
{
    const auto [control_index, target_index] = unpack_double_qubit_gate_indices(info);
    auto pair_iterator = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    pair_iterator.set_state(pair.i_lower);

    for (std::size_t i {pair.i_lower}; i < pair.i_upper; ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();
        apply_u_gate(state, state0_index, state1_index, mat);
    }
}

inline void simulate_loop_body_(
    const mqis::QuantumCircuit& circuit,
    mqis::QuantumState& state,
    const FlatIndexPair& single_gate_pair,
    const FlatIndexPair& double_gate_pair,
    const mqis::GateInfo& gate
)
{
    using G = mqis::Gate;

    switch (gate.gate) {
        case G::H : {
            simulate_single_qubit_gate_<G::H>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::X : {
            simulate_single_qubit_gate_<G::X>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::Y : {
            simulate_single_qubit_gate_<G::Y>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::Z : {
            simulate_single_qubit_gate_<G::Z>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RX : {
            simulate_single_qubit_gate_<G::RX>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RY : {
            simulate_single_qubit_gate_<G::RY>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RZ : {
            simulate_single_qubit_gate_<G::RZ>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::P : {
            simulate_single_qubit_gate_<G::P>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::CX : {
            simulate_double_qubit_gate_<G::CX>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CY : {
            simulate_double_qubit_gate_<G::CY>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CZ : {
            simulate_double_qubit_gate_<G::CZ>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRX : {
            simulate_double_qubit_gate_<G::CRX>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRY : {
            simulate_double_qubit_gate_<G::CRY>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRZ : {
            simulate_double_qubit_gate_<G::CRZ>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CP : {
            simulate_double_qubit_gate_<G::CP>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::U : {
            const auto matrix_index = unpack_gate_matrix_index(gate);
            const auto& matrix = circuit.unitary_gate(matrix_index);
            simulate_single_qubit_gate_general_(state, gate, circuit.n_qubits(), matrix, single_gate_pair);
            break;
        }
        case G::CU : {
            const auto matrix_index = unpack_gate_matrix_index(gate);
            const auto& matrix = circuit.unitary_gate(matrix_index);
            simulate_double_qubit_gate_general_(state, gate, circuit.n_qubits(), matrix, double_gate_pair);
            break;
        }
        case G::M : {
            break;
        }
    }
}

inline void check_valid_number_of_qubits_(const mqis::QuantumCircuit& circuit, const mqis::QuantumState& state)
{
    if (circuit.n_qubits() != state.n_qubits()) {
        throw std::runtime_error {"Invalid simulation; circuit and state have different number of qubits."};
    }

    if (circuit.n_qubits() == 0) {
        throw std::runtime_error {"Cannot simulate a circuit or state with zero qubits."};
    }
}

inline void simulate_multithreaded_loop_(
    std::barrier<>& sync_point,
    const mqis::QuantumCircuit& circuit,
    mqis::QuantumState& state,
    const FlatIndexPair& single_gate_pair,
    const FlatIndexPair& double_gate_pair
)
{
    for (const auto& gate : circuit) {
        simulate_loop_body_(circuit, state, single_gate_pair, double_gate_pair, gate);
        sync_point.arrive_and_wait();
    }
}

}  // namespace impl_mqis

namespace mqis
{

inline void simulate(const QuantumCircuit& circuit, QuantumState& state)
{
    namespace im = impl_mqis;

    im::check_valid_number_of_qubits_(circuit, state);

    const auto n_single_gate_pairs = im::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
    const auto single_flat_index_pair = im::FlatIndexPair {0, n_single_gate_pairs};

    const auto n_double_gate_pairs = im::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
    const auto double_flat_index_pair = im::FlatIndexPair {0, n_double_gate_pairs};

    for (const auto& gate : circuit) {
        simulate_loop_body_(circuit, state, single_flat_index_pair, double_flat_index_pair, gate);
    }
}

/*
    WARNING: the current multithreaded implementation is slower than the singlethreaded implementation;
    I'm not sure of the reasons yet (too much waiting at the barrier, multiple states per cache line, etc.)
*/
inline void simulate_multithreaded(const QuantumCircuit& circuit, QuantumState& state, std::size_t n_threads)
{
    namespace im = impl_mqis;

    if (n_threads == 0) {
        throw std::runtime_error {"Cannot perform simulation with 0 threads.\n"};
    }

    im::check_valid_number_of_qubits_(circuit, state);

    const auto n_single_gate_pairs = im::number_of_single_qubit_gate_pairs_(circuit.n_qubits());
    const auto single_flat_index_pairs = im::partial_sum_pairs_(n_single_gate_pairs, n_threads);

    const auto n_double_gate_pairs = im::number_of_double_qubit_gate_pairs_(circuit.n_qubits());
    const auto double_flat_index_pairs = im::partial_sum_pairs_(n_double_gate_pairs, n_threads);

    auto threads = std::vector<std::jthread> {};
    threads.reserve(n_threads);

    auto barrier = std::barrier {static_cast<std::ptrdiff_t>(n_threads)};

    for (std::size_t i {0}; i < n_threads; ++i) {
        threads.emplace_back(
            im::simulate_multithreaded_loop_,
            std::ref(barrier),
            std::ref(circuit),
            std::ref(state),
            single_flat_index_pairs[i],
            double_flat_index_pairs[i]
        );
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

}  // namespace mqis

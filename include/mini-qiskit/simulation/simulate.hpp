#pragma once

#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/simulation/gate_pair_generator.hpp"
#include "mini-qiskit/simulation/operations.hpp"
#include "mini-qiskit/state.hpp"

namespace impl_mqis
{

struct FlatIndexPair
{
    std::size_t i_lower;
    std::size_t i_upper;
};

constexpr auto number_of_single_qubit_gate_pairs(std::size_t n_qubits) -> std::size_t
{
    if (n_qubits == 0) {
        throw std::runtime_error {
            "UNREACHABLE: dev error, can't get number of single qubit gate pairs for 0 qubits\n"
        };
    }

    return static_cast<std::size_t>(1 << (n_qubits - 1));
}

constexpr auto number_of_double_qubit_gate_pairs(std::size_t n_qubits) -> std::size_t
{
    // the case where `n_qubits == 1` does not make sense for double qubit gate simulations;
    //   - the calculation for the number of double gates is invalid
    // Luckily, this does not matter;
    //   - if there is only one qubit, then no double qubit gates should be simulated anyways
    //   - so we can set the upper qubit value to 0, so the loops never run

    if (n_qubits == 0) {
        throw std::runtime_error {
            "UNREACHABLE: dev error, can't get number of double qubit gate pairs for 0 qubits\n"
        };
    }

    if (n_qubits == 1) {
        return 0;
    } else {
        return 1ul << (n_qubits - 2);
    }
}


template <mqis::Gate GateType>
void simulate_single_qubit_gate(
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

void simulate_single_qubit_gate_general(
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
void simulate_double_qubit_gate(
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

void simulate_double_qubit_gate_general(
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
            simulate_single_qubit_gate<G::H>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::X : {
            simulate_single_qubit_gate<G::X>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::Y : {
            simulate_single_qubit_gate<G::Y>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::Z : {
            simulate_single_qubit_gate<G::Z>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RX : {
            simulate_single_qubit_gate<G::RX>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RY : {
            simulate_single_qubit_gate<G::RY>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::RZ : {
            simulate_single_qubit_gate<G::RZ>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::P : {
            simulate_single_qubit_gate<G::P>(state, gate, circuit.n_qubits(), single_gate_pair);
            break;
        }
        case G::CX : {
            simulate_double_qubit_gate<G::CX>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CY : {
            simulate_double_qubit_gate<G::CY>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CZ : {
            simulate_double_qubit_gate<G::CZ>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRX : {
            simulate_double_qubit_gate<G::CRX>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRY : {
            simulate_double_qubit_gate<G::CRY>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CRZ : {
            simulate_double_qubit_gate<G::CRZ>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::CP : {
            simulate_double_qubit_gate<G::CP>(state, gate, circuit.n_qubits(), double_gate_pair);
            break;
        }
        case G::U : {
            const auto matrix_index = unpack_gate_matrix_index(gate);
            const auto& matrix = circuit.unitary_gate(matrix_index);
            simulate_single_qubit_gate_general(state, gate, circuit.n_qubits(), matrix, single_gate_pair);
            break;
        }
        case G::CU : {
            const auto matrix_index = unpack_gate_matrix_index(gate);
            const auto& matrix = circuit.unitary_gate(matrix_index);
            simulate_double_qubit_gate_general(state, gate, circuit.n_qubits(), matrix, double_gate_pair);
            break;
        }
        case G::M : {
            break;
        }
    }
}

inline void simulate_between_(
    const mqis::QuantumCircuit& circuit,
    mqis::QuantumState& state,
    const FlatIndexPair& single_gate_pair,
    const FlatIndexPair& double_gate_pair
)
{
    for (const auto& gate : circuit) {
        simulate_loop_body_(circuit, state, single_gate_pair, double_gate_pair, gate);
    }
}

}  // namespace impl_mqis

namespace mqis
{

inline void simulate(const QuantumCircuit& circuit, QuantumState& state)
{
    if (circuit.n_qubits() != state.n_qubits()) {
        throw std::runtime_error {"Invalid simulation; circuit and state have different number of qubits."};
    }

    if (circuit.n_qubits() == 0) {
        throw std::runtime_error {"Cannot simulate a circuit or state with zero qubits."};
    }

    const auto n_single_gates = impl_mqis::number_of_single_qubit_gate_pairs(circuit.n_qubits());
    const auto n_double_gates = impl_mqis::number_of_double_qubit_gate_pairs(circuit.n_qubits());

    const auto single_gate_pair = impl_mqis::FlatIndexPair {0, n_single_gates};
    const auto double_gate_pair = impl_mqis::FlatIndexPair {0, n_double_gates};

    impl_mqis::simulate_between_(circuit, state, single_gate_pair, double_gate_pair);
}

}  // namespace mqis

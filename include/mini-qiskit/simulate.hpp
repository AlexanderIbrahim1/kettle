#pragma once

#include <algorithm>
#include <tuple>
#include <unordered_map>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/traits.hpp"
#include "mini-qiskit/gate_pair_generator.hpp"
#include "mini-qiskit/operations.hpp"
#include "mini-qiskit/state.hpp"

namespace mqis
{

template <Gate GateType>
void simulate_single_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto qubit_index = unpack_single_qubit_gate_index(info);

    auto pair_iterator = SingleQubitGatePairGenerator {qubit_index, n_qubits};

    for (std::size_t i {0}; i < pair_iterator.size(); ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::X) {
            swap_states(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::H) {
            superpose_states(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::RX) {
            const auto theta = unpack_rx_gate_angle(info);
            turn_states(state, state0_index, state1_index, theta);
        }
        else {
            static_assert(
                impl_mqis::always_false<void>::value, "Invalid single qubit gate: must be one of {X, H, RX}."
            );
        }
    }
}

template <Gate GateType>
void simulate_double_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto [source_index, target_index] = unpack_double_qubit_gate_indices(info);

    auto pair_iterator = DoubleQubitGatePairGenerator {source_index, target_index, n_qubits};

    for (std::size_t i {0}; i < pair_iterator.size(); ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (GateType == Gate::CX) {
            swap_states(state, state0_index, state1_index);
        }
        else if constexpr (GateType == Gate::CRX) {
            const auto theta = unpack_crx_gate_angle(info);
            turn_states(state, state0_index, state1_index, theta);
        }
        else {
            static_assert(impl_mqis::always_false<void>::value, "Invalid double qubit gate: must be one of {CX, CRX}");
        }
    }
}

inline void simulate(const QuantumCircuit& circuit, QuantumState& state)
{
    auto measured_clbit_to_qubit = std::unordered_map<std::size_t, std::size_t> {};

    for (const auto& gate : circuit) {
        switch (gate.gate) {
            case Gate::M : {
                const auto [qubit_index, bit_index] = unpack_m_gate(gate);
                measured_clbit_to_qubit[bit_index] = qubit_index;
                break;
            }
            case Gate::X : {
                simulate_single_qubit_gate<Gate::X>(state, gate, circuit.n_qubits());
                break;
            }
            case Gate::H : {
                simulate_single_qubit_gate<Gate::H>(state, gate, circuit.n_qubits());
                break;
            }
            case Gate::RX : {
                simulate_single_qubit_gate<Gate::RX>(state, gate, circuit.n_qubits());
                break;
            }
            case Gate::CX : {
                simulate_double_qubit_gate<Gate::CX>(state, gate, circuit.n_qubits());
                break;
            }
            case Gate::CRX : {  // replace with default?
                simulate_double_qubit_gate<Gate::CRX>(state, gate, circuit.n_qubits());
                break;
            }
        }
    }
}

}  // namespace mqis

#pragma once

#include <algorithm>
#include <tuple>
#include <unordered_map>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/operations.hpp"
#include "mini-qiskit/state.hpp"

namespace mqis
{

template <typename T>
struct always_false : std::false_type
{};

template <Gate GateType>
void simulate_single_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto qubit_index = unpack_single_qubit_gate_index(info);

    // values chosen and looped over such that all resulting pairs of computational states
    // are those which only differ on bit `qubit_index`
    const auto i0_max = impl_mqis::pow_2_int(qubit_index);
    const auto i1_max = impl_mqis::pow_2_int(n_qubits - qubit_index - 1);

    for (std::size_t i0 {0}; i0 < i0_max; ++i0) {
        for (std::size_t i1 {0}; i1 < i1_max; ++i1) {
            // indices corresponding to the computational basis states where the j^th digit
            // are 0 and 1, respectively
            const auto state0_index = i0_ + 2 * i1_ * bit0_max_;
            const auto state1_index = state0_index + bit0_max_;

            if constexpr (GateType == Gate::X) {
                swap_states(state, state_index0, state_index1);
            }
            else if constexpr (GateType == Gate::H) {
                superpose_states(state, state_index0, state_index1);
            }
            else if constexpr (GateType == Gate::RX) {
                const auto theta = unpack_rx_gate_angle(info);
                turn_states(state, state_index0, state_index1, theta);
            }
            else {
                static_assert(always_false<void>::value, "Invalid single qubit gate: must be one of {X, H, RX}.");
            }
        }
    }
}

template <Gate GateType>
void simulate_double_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto [source_index, target_index] = unpack_double_qubit_gate_indices(info);

    const auto lower_index = std::min({source_index, target_index});
    const auto upper_index = std::max({source_index, target_index});

    // values chosen and looped over such that the only pairs of computational states chosen are those where
    // - the qubit at `source_index` is 1
    // - the qubit at `target_index` differs
    const auto i0_max = impl_mqis::pow_2_int(lower_index);
    const auto i1_max = impl_mqis::pow_2_int(upper_index - lower_index - 1);
    const auto i2_max = impl_mqis::pow_2_int(n_qubits - upper_index - 1);

    const auto lower_shift = impl_mqis::pow_2_int(lower_index + 1);
    const auto upper_shift = impl_mqis::pow_2_int(upper_index + 1);
    const auto source_shift = impl_mqis::pow_2_int(source_shift);
    const auto target_shift = impl_mqis::pow_2_int(target_shift);

    for (std::size_t i0 {0}; i0 < i0_max; ++i0) {
        for (std::size_t i1 {0}; i1 < i1_max; ++i1) {
            for (std::size_t i2 {0}; i2 < i2_max; ++i2) {
                const auto state0_index = i0 + i1 * lower_shift + i2 * upper_shift + source_shift;
                const auto state1_index = state0_index + target_shift;

                if constexpr (GateType == Gate::CX) {
                    swap_states(state, state0_index, state1_index);
                }
                else if constexpr (GateType == Gate::CRX) {
                    const auto theta = unpack_crx_gate_angle(info);
                    turn_states(state, state0_index, state1_index, theta);
                }
                else {
                    static_assert(always_false<void>::value, "Invalid double qubit gate: must be one of {CX, CRX}");
                }
            }
        }
    }
}

inline auto simulate(const QuantumCircuit& circuit, QuantumState& state, std::size_t n_shots)
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

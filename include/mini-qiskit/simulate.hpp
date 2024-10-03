#pragma once

#include <tuple>
#include <unordered_map>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/operations.hpp"
#include "mini-qiskit/state.hpp"

namespace mqis
{

/*
    Generator object that yields all pairs of state indices whose bit strings differ only on
    bit `qubit_index`.

    std::generator doesn't work in C++20 :(
*/
class SingleQubitGatePairGenerator
{
public:
    explicit SingleQubitGatePairGenerator(std::size_t qubit_index, std::size_t n_qubits)
        : qubit_index_ {qubit_index}
        , bit0_max_ {impl_mqis::pow_2_int(qubit_index)}
        , bit1_max_ {impl_mqis::pow_2_int(n_qubits - qubit_index - 1)}
    {}

    constexpr auto yield() -> std::tuple<std::size_t, std::size_t>
    {
        ++i1_;

        if (i1_ == bit1_max_) {
            ++i0_;
            i1_ = 0;
        }

        // indices corresponding to the computational basis states where the j^th digit
        // are 0 and 1, respectively
        const auto state0_index = i0_ + 2 * i1_ * bit0_max_;
        const auto state1_index = state0_index + bit0_max_;

        return {state0_index, state1_index};
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return bit0_max_ * bit1_max_;
    }

private:
    std::size_t qubit_index_;
    std::size_t bit0_max_;
    std::size_t bit1_max_;
    std::size_t i0_ {0};
    std::size_t i1_ {0};
};

inline void simulate_single_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto qubit_index = unpack_single_qubit_gate_index(info);
    auto generator = SingleQubitGatePairGenerator {qubit_index, n_qubits};

    for (std::size_t i {0}; i < generator.size(); ++i) {
        const auto [state_index0, state_index1] = generator.yield();

        switch (info.gate) {
            case Gate::X : {
                swap_states(state, state_index0, state_index1);
                break;
            }
            case Gate::H : {
                superpose_states(state, state_index0, state_index1);
                break;
            }
            default : {  // case Gate::RX
                const auto theta = unpack_rx_gate_angle(info);
                turn_states(state, state_index0, state_index1, theta);
                break;
            }
        }
    }
}

inline void simulate_double_qubit_gate(QuantumState& state, const GateInfo& info, std::size_t n_qubits)
{
    const auto [source_index, target_index] = unpack_double_qubit_gate_indices(info);

    //     const auto qubit_index = unpack_single_qubit_gate_index(info);
    //     auto generator = SingleQubitGatePairGenerator {qubit_index, n_qubits};
    //
    //     for (std::size_t i {0}; i < generator.size(); ++i) {
    //         const auto [state_index0, state_index1] = generator.yield();
    //
    //         switch (info.gate) {
    //             case Gate::X: {
    //                 swap_states(state, state_index0, state_index1);
    //                 break;
    //             }
    //             case Gate::H: {
    //                 superpose_states(state, state_index0, state_index1);
    //                 break;
    //             }
    //             default: { // case Gate::RX
    //                 const auto theta = unpack_rx_gate_angle(info);
    //                 turn_states(state, state_index0, state_index1, theta);
    //                 break;
    //             }
    //         }
    //     }
}

inline auto simulate(const QuantumCircuit& circuit, QuantumState& state, std::size_t n_shots)
{
    auto measured_clbit_to_qubit = std::unordered_map<std::size_t, std::size_t> {};

    for (const auto& gate : circuit) {
        if (gate.gate == Gate::M) {
            const auto [qubit_index, bit_index] = unpack_m_gate(gate);
            measured_clbit_to_qubit[bit_index] = qubit_index;
            break;
        }
        else if (gate.gate == Gate::X || gate.gate == Gate::H || gate.gate == Gate::RX) {
            simulate_single_qubit_gate(state, gate, circuit.n_qubits());
        }
        else {
        }
    }
}

}  // namespace mqis

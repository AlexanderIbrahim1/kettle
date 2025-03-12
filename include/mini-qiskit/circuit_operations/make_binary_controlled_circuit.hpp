#pragma once

#include <stdexcept>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/multiplicity_controlled_u_gate.hpp"
#include "mini-qiskit/circuit_operations/append_circuits.hpp"
#include "mini-qiskit/circuit_operations/make_controlled_circuit.hpp"

namespace mqis
{

/*
    This function creates a binary-controlled circuit by repeating the subcircuit
    the required number of times. This is slower than finding a way to combine the
    circuits and creating a new gate with each iteration.
*/
template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
inline auto make_binary_controlled_circuit_naive(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> mqis::QuantumCircuit
{
    auto new_circuit = QuantumCircuit {n_new_qubits};

    const auto size = impl_mqis::get_container_size(control_qubits);

    for (std::size_t i {0}; i < size; ++i) {
        const auto control = impl_mqis::get_container_index(control_qubits, i);
        const auto n_iterations = static_cast<std::size_t>(1 << i);

        for (std::size_t i_iter {0}; i_iter < n_iterations; ++i_iter) {
            const auto controlled_subcircuit = make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
            extend_circuit(new_circuit, controlled_subcircuit);
        }
    }

    return new_circuit;
}

}  // namespace mqis

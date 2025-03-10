#pragma once

#include <stdexcept>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/primitive_gate.hpp"

namespace mqis
{

/*
PLAN:

Take an existing QuantumCircuit instance, and make the execution of the entire circuit
dependent on a control qubit outside of that circuit.

The existing QuantumCircuit gets turned into a new QuantumCircuit instance.

The function should take as arguments:
  - the QuantumCircuit instance to be made controlled
  - the control qubit
  - the new qubit indices that the qubits of the current QuantumCircuit instance get mapped to

Later I should make a multiplicity-controlled version of this function
  - I would have to turn all the general gates into U-gates

TODO: add a check to make sure all the new qubit indices are unique
TODO: add a check to make sure the control qubit isn't one of the new qubit indices
TODO: add a check to make sure the new qubit indices, and control qubit, will all fit onto the new circuit
*/
template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
inline auto make_controlled_circuit(
    const mqis::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control_qubit,
    const Container& container
) -> mqis::QuantumCircuit
{
    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));

    if (size != subcircuit.n_qubits()) {
        throw std::runtime_error {
            "The number of proposed new qubit indices, and the number of qubits in the subcircuit, do not match"
        };
    }

    auto new_circuit = mqis::QuantumCircuit {n_new_qubits};

    for (const auto& gate_info : subcircuit) {
        switch (gate_info.gate)
        {
            case Gate::X : {
                const auto original_target_qubit = impl_mqis::unpack_x_gate(gate_info);
                const auto new_target_qubit = impl_mqis::get_container_index(container, original_target_qubit);
                new_circuit.add_cu_gate(X_GATE, control_qubit, new_target_qubit);
                break;
            }
        }
    }
}

}  // namespace mqis

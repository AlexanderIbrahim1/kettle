#include <cstddef>
#include <stdexcept>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/swap.hpp"

namespace ket
{

void apply_swap(QuantumCircuit& circuit, std::size_t target_qubit0, std::size_t target_qubit1)
{
    if (target_qubit0 == target_qubit1) {
        throw std::runtime_error {"Cannot swap a qubit with itself"};
    }

    circuit.add_cx_gate(target_qubit0, target_qubit1);
    circuit.add_cx_gate(target_qubit1, target_qubit0);
    circuit.add_cx_gate(target_qubit0, target_qubit1);
}

void apply_control_swap(QuantumCircuit& circuit, std::size_t control_qubit, std::size_t swap_qubit0, std::size_t swap_qubit1)
{
    // solution taken from: https://quantumcomputing.stackexchange.com/a/9343

    if (swap_qubit0 == swap_qubit1) {
        throw std::runtime_error {"Cannot swap a qubit with itself"};
    }

    if (control_qubit == swap_qubit0 || control_qubit == swap_qubit1) {
        throw std::runtime_error {"Cannot use the control qubit as one of the qubits to be swapped"};
    }

    circuit.add_cx_gate(swap_qubit1, swap_qubit0);
    circuit.add_ccx_gate(control_qubit, swap_qubit0, swap_qubit1);
    circuit.add_cx_gate(swap_qubit1, swap_qubit0);
}

}  // namespace ket

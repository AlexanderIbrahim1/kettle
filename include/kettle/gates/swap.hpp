#pragma once

#include <cstddef>

#include "kettle/circuit/circuit.hpp"

namespace ket
{

/*
    Apply a SWAP gate to the qubits whose indices are given by `target_qubit0` and `target_qubit1`.
*/
void apply_swap(QuantumCircuit& circuit, std::size_t target_qubit0, std::size_t target_qubit1);

/*
    Apply a SWAP gate to the qubits whose indices are given by `target_qubit0` and `target_qubit1`,
    dependent on the qubit at index `control_qubit` being in the 1 state.
*/
void apply_control_swap(QuantumCircuit& circuit, std::size_t control_qubit, std::size_t swap_qubit0, std::size_t swap_qubit1);

}  // namespace ket

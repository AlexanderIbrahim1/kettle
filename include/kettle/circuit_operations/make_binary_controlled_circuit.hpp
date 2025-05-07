#pragma once

#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/utils.hpp"

namespace ket
{

/*
    This function creates a binary-controlled circuit by repeating the subcircuit
    the required number of times. This is slower than finding a way to combine the
    circuits and creating a new gate with each iteration.
*/
template <QubitIndices Container = QubitIndicesIList>
auto make_binary_controlled_circuit_naive(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit;

/*
    This function creates a binary-controlled circuit by accepting a container of
    increasing binary powers of the subcircuit in question.
    
    It assumes that the circuits are the correct binary powers. This results in
    fewer gates than the naive approach.
*/
template <QubitIndices Container = QubitIndicesIList>
auto make_binary_controlled_circuit_from_binary_powers(
    const std::vector<QuantumCircuit>& subcircuit_powers,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit;

}  // namespace ket

#pragma once

#include "kettle/circuit/circuit.hpp"


namespace ket
{

template <QubitIndices Container = QubitIndicesIList>
auto make_controlled_circuit(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const Container& mapped_qubits
) -> ket::QuantumCircuit;

template <QubitIndices Container = QubitIndicesIList>
auto make_multiplicity_controlled_circuit(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit;

}  // namespace ket

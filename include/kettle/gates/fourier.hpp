#pragma once

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/utils.hpp"


namespace ket
{

template <QubitIndices Container = QubitIndicesIList>
void apply_forward_fourier_transform(QuantumCircuit& circuit, const Container& container);

template <QubitIndices Container = QubitIndicesIList>
void apply_inverse_fourier_transform(QuantumCircuit& circuit, const Container& container);

}  // namespace ket

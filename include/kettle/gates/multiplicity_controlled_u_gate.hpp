#pragma once

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"

namespace ket
{

/*
    Apply the single qubit unitary gate `unitary` to the target qubit whose index is given
    by `target_index`. This unitary gate is controlled by the control qubits whose indices
    are given by `control_indices`.

    The target qubit undergoes the transformation only if all the control qubits are set;
    in other words, this is an AND-style multiplicity controlled gate.
*/
template <QubitIndices Container = QubitIndicesIList>
void apply_multiplicity_controlled_u_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    std::size_t target_index,
    const Container& control_indices,
    double matrix_sqrt_tolerance = ket::internal::MATRIX_2X2_SQRT_TOLERANCE
);

}  // namespace ket

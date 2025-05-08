#pragma once

#include <cstddef>
#include <utility>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"

namespace ket
{

void apply_toffoli_gate(
    QuantumCircuit& circuit,
    const std::pair<std::size_t, std::size_t>& control_qubits,
    std::size_t target_qubit
);

void apply_doubly_controlled_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    const std::pair<std::size_t, std::size_t>& control_qubits,
    std::size_t target_qubit
);

}  // namespace ket

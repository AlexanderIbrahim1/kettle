#pragma once

#include <cstddef>
#include <utility>

#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"

namespace mqis
{

void apply_toffoli_gate(
    QuantumCircuit& circuit,
    const std::pair<std::size_t, std::size_t>& control_qubits,
    std::size_t target_qubit
)
{
    const auto [control_qubit0, control_qubit1] = control_qubits;

    circuit.add_cu_gate(sqrt_x_gate(), control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(sqrt_x_gate_conj(), control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(sqrt_x_gate(), control_qubit0, target_qubit);
}

}  // namespace mqis

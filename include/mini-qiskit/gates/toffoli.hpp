#pragma once

#include <cstddef>
#include <utility>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"

namespace impl_mqis
{

static constexpr auto SQRT_X_GATE = mqis::Matrix2X2 {
    {0.5, 0.5 },
    {0.5, -0.5},
    {0.5, -0.5},
    {0.5, 0.5 },
};

static constexpr auto SQRT_X_GATE_CONJ = mqis::conjugate_transpose(SQRT_X_GATE);

}  // namespace impl_mqis

namespace mqis
{

void apply_toffoli_gate(
    QuantumCircuit& circuit,
    const std::pair<std::size_t, std::size_t>& control_qubits,
    std::size_t target_qubit
)
{
    const auto [control_qubit0, control_qubit1] = control_qubits;

    circuit.add_cu_gate(impl_mqis::SQRT_X_GATE, control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(impl_mqis::SQRT_X_GATE_CONJ, control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(impl_mqis::SQRT_X_GATE, control_qubit0, target_qubit);
}

}  // namespace mqis

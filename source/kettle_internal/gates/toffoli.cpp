#include <cstddef>
#include <utility>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/toffoli.hpp"

namespace ket
{

void apply_doubly_controlled_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    const std::pair<std::size_t, std::size_t>& control_qubits,
    std::size_t target_qubit
)
{
    const auto [control_qubit0, control_qubit1] = control_qubits;

    const auto mat_sqrt = matrix_square_root(unitary);
    const auto mat_sqrt_adj = conjugate_transpose(mat_sqrt);

    circuit.add_cu_gate(mat_sqrt, control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(mat_sqrt_adj, control_qubit1, target_qubit);
    circuit.add_cx_gate(control_qubit0, control_qubit1);
    circuit.add_cu_gate(mat_sqrt, control_qubit0, target_qubit);
}

}  // namespace ket

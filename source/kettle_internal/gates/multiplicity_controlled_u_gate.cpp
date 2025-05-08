#include <initializer_list>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/multiplicity_controlled_u_gate.hpp"

namespace
{

struct MCUGateStackFrame_
{
    ket::Matrix2X2 gate;
    std::vector<std::size_t> control_indices;
    std::size_t target_index;
};

auto split_control_indices_(
    const std::vector<std::size_t>& control_indices
) -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>
{
    const auto bottom_control_indices = std::vector<std::size_t> {control_indices[0]};
    const auto top_control_indices = std::vector<std::size_t> {control_indices.begin() + 1, control_indices.end()};

    return {bottom_control_indices, top_control_indices};
}

}  // namespace

namespace ket
{

template <QubitIndices Container>
void apply_multiplicity_controlled_u_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    std::size_t target_index,
    const Container& control_indices,
    double matrix_sqrt_tolerance
)
{
    auto stack = std::vector<MCUGateStackFrame_> {};
    stack.emplace_back(unitary, std::vector<std::size_t> {control_indices.begin(), control_indices.end()}, target_index);

    while (stack.size() != 0) {
        const auto frame = stack.back();
        stack.pop_back();

        if (frame.control_indices.size() == 1) {
            circuit.add_cu_gate(frame.gate, frame.control_indices[0], frame.target_index);
            continue;
        }

        const auto [bottom_controls, top_controls] = split_control_indices_(frame.control_indices);
        const auto sqrt_gate = matrix_square_root(frame.gate, matrix_sqrt_tolerance);
        const auto sqrt_gate_conj = conjugate_transpose(sqrt_gate);
        const auto mcx_target_qubit = frame.control_indices[0];
        const auto gate_target_qubit = frame.target_index;

        stack.emplace_back(sqrt_gate, top_controls, gate_target_qubit);
        stack.emplace_back(sqrt_gate, bottom_controls, gate_target_qubit);
        stack.emplace_back(x_gate(), top_controls, mcx_target_qubit);
        stack.emplace_back(sqrt_gate_conj, bottom_controls, gate_target_qubit);
        stack.emplace_back(x_gate(), top_controls, mcx_target_qubit);
    }
}
template
void apply_multiplicity_controlled_u_gate<QubitIndicesVector>(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    std::size_t target_index,
    const QubitIndicesVector& control_indices,
    double matrix_sqrt_tolerance
);
template
void apply_multiplicity_controlled_u_gate<QubitIndicesIList>(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    std::size_t target_index,
    const QubitIndicesIList& control_indices,
    double matrix_sqrt_tolerance
);

}  // namespace ket

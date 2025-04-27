#pragma once

#include <initializer_list>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/common_u_gates.hpp"

namespace impl_ket
{

struct MCUGateStackFrame_
{
    ket::Matrix2X2 gate;
    std::vector<std::size_t> control_indices;
    std::size_t target_index;
};

inline auto split_control_indices_(
    const std::vector<std::size_t>& control_indices
) -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>
{
    const auto bottom_control_indices = std::vector<std::size_t> {control_indices[0]};
    const auto top_control_indices = std::vector<std::size_t> {control_indices.begin() + 1, control_indices.end()};

    return {bottom_control_indices, top_control_indices};
}

}  // namespace impl_ket

namespace ket
{

/*
    Apply the single qubit unitary gate `unitary` to the target qubit whose index is given
    by `target_index`. This unitary gate is controlled by the control qubits whose indices
    are given by `control_indices`.

    The target qubit undergoes the transformation only if all the control qubits are set;
    in other words, this is an AND-style multiplicity controlled gate.
*/
template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
void apply_multiplicity_controlled_u_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& unitary,
    std::size_t target_index,
    const Container& control_indices,
    double matrix_sqrt_tolerance = impl_ket::MATRIX_2X2_SQRT_TOLERANCE
)
{
    auto stack = std::vector<impl_ket::MCUGateStackFrame_> {};
    stack.emplace_back(unitary, std::vector<std::size_t> {control_indices.begin(), control_indices.end()}, target_index);

    while (stack.size() != 0) {
        const auto frame = stack.back();
        stack.pop_back();

        if (frame.control_indices.size() == 1) {
            circuit.add_cu_gate(frame.gate, frame.control_indices[0], frame.target_index);
            continue;
        }

        const auto [bottom_controls, top_controls] = impl_ket::split_control_indices_(frame.control_indices);
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

}  // namespace ket

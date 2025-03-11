#pragma once

#include <initializer_list>
#include <iterator>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"

/*
PLAN
- accept a 2x2 matrix, a target qubit, and a non-zero number of control qubits
- can solve via recursion or iteration
  - iteration is probably easier to reason through

- cover base cases:
  - if one control qubit is given, forward to add_cu_gate()

- create stack frames
  - holds the gate, the target index, and the control indices

- push the input into a stack frame, and then put it on top of the stack

- while the stack is not empty:
  - pop a stack frame off the top of the stack
  - check the number of control qubits
    - if it is one:
      - call `add_cu_gate()`
    - else:
      - create the five decomposed stack frames
      - push them into the stack (in reverse order)
  - go back to the top
*/

namespace impl_mqis
{

struct MCUGateStackFrame_
{
    mqis::Matrix2X2 gate;
    std::vector<std::size_t> control_indices;
    std::size_t target_index;
};

auto split_control_indices_(const std::vector<std::size_t>& control_indices)
    -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>
{
    const auto bottom_control_indices = std::vector<std::size_t> {control_indices[0]};
    const auto top_control_indices = std::vector<std::size_t> {control_indices.begin() + 1, control_indices.end()};

    return {bottom_control_indices, top_control_indices};
}

}  // namespace impl_mqis

namespace mqis
{

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void apply_multiplicity_controlled_u_gate(
    QuantumCircuit& circuit,
    const Matrix2X2& gate,
    std::size_t target_index,
    const Container& container
)
{
    auto stack = std::vector<impl_mqis::MCUGateStackFrame_> {};
    stack.emplace_back(gate, std::vector<std::size_t> {container.begin(), container.end()}, target_index);

    while (stack.size() != 0) {
        const auto frame = stack.back();
        stack.pop_back();

        if (frame.control_indices.size() == 1) {
            circuit.add_cu_gate(frame.gate, frame.control_indices[0], frame.target_index);
            continue;
        }

        const auto [bottom_control_indices, top_control_indices] =
            impl_mqis::split_control_indices_(frame.control_indices);
        const auto sqrt_gate = matrix_square_root(frame.gate);
        const auto sqrt_gate_conj = conjugate_transpose(sqrt_gate);
        const auto mcx_target_qubit = frame.control_indices[0];
        const auto gate_target_qubit = frame.target_index;

        stack.emplace_back(sqrt_gate, top_control_indices, gate_target_qubit);
        stack.emplace_back(sqrt_gate, bottom_control_indices, gate_target_qubit);
        stack.emplace_back(x_gate(), top_control_indices, mcx_target_qubit);
        stack.emplace_back(sqrt_gate_conj, bottom_control_indices, gate_target_qubit);
        stack.emplace_back(x_gate(), top_control_indices, mcx_target_qubit);
    }
}

}  // namespace mqis

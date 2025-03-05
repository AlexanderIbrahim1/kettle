#pragma once

#include <initializer_list>
#include <iterator>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"

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

struct MCUGateStackFrame
{
    mqis::Matrix2X2 gate;
    std::size_t target_index;
    std::vector<std::size_t> control_indices;
};

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
    auto stack = std::vector<impl_mqis::MCUGateStackFrame> {};
    stack.emplace_back(gate, target_index, std::vector<std::size_t>{container});

    while (stack.size() != 0) {
        const auto& frame = stack.back();

        if (frame.control_indices.size() == 1) {
            circuit.add_cu_gate(frame.gate, frame.control_indices[0], frame.target_index);
        }
    }
}

}  // namespace mqis

//    void add_cu_gate(const Matrix2X2& gate, std::size_t source_index, std::size_t target_index)
//    {
//        check_qubit_range_(source_index, "source qubit", "CU");
//        check_qubit_range_(target_index, "target qubit", "CU");
//        check_previous_gate_is_not_measure_(source_index, "CU");
//        check_previous_gate_is_not_measure_(target_index, "CU");
//
//        unitary_gates_.push_back(gate);
//        const auto gate_index = unitary_gates_.size() - 1;
//
//        gates_.emplace_back(impl_mqis::create_cu_gate(source_index, target_index, gate_index));
//    }

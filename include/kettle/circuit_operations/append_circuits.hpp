#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"

namespace impl_ket
{

inline void check_matching_number_of_qubits_(const ket::QuantumCircuit& left, const ket::QuantumCircuit& right)
{
    if (left.n_qubits() != right.n_qubits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of qubits."};
    }
}

inline void check_matching_number_of_bits_(const ket::QuantumCircuit& left, const ket::QuantumCircuit& right)
{
    if (left.n_bits() != right.n_bits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of bits."};
    }
}

inline auto bitwise_or(const std::vector<std::uint8_t>& left, const std::vector<std::uint8_t>& right) -> std::vector<std::uint8_t>
{
    if (left.size() != right.size()) {
        throw std::runtime_error {"Cannot calculate OR of two bitsets of different sizes."};
    }

    const auto size = left.size();

    auto output = std::vector<std::uint8_t> {};
    output.reserve(size);
    for (std::size_t i {0}; i < size; ++i) {
        output.push_back(static_cast<std::uint8_t>(left[i] || right[i]));
    }

    return output;
}

}  // namespace impl_ket

namespace ket
{
    
inline void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right)
{
    namespace ctrl = impl_ket::control;

    impl_ket::check_matching_number_of_qubits_(left, right);
    impl_ket::check_matching_number_of_bits_(left, right);

    const auto n_left_matrices = left.unitary_gates_.size();
    const auto n_left_control_flow_instructions = left.control_flow_instructions_.size();

    const auto n_new_gates = left.gates_.size() + right.gates_.size();
    left.gates_.reserve(n_new_gates);

    // the U and CU gates refer to the indices of the held matrix in a vector in the QuantumCircuit;
    // the CONTROL gates refer to the indices of the held ControlFlowInstruction in a vector in the QuantumCircuit;
    // all three need to have their indices updated to reflect the new indices
    for (const auto& right_gate : right.gates_) {
        if (right_gate.gate == Gate::U) {
            auto [target_index, matrix_index] = impl_ket::unpack_u_gate(right_gate);
            matrix_index += n_left_matrices;
            left.gates_.emplace_back(impl_ket::create_u_gate(target_index, matrix_index));
        }
        else if (right_gate.gate == Gate::CU) {
            auto [control_index, target_index, matrix_index] = impl_ket::unpack_cu_gate(right_gate);
            matrix_index += n_left_matrices;
            left.gates_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, matrix_index));
        }
        else if (right_gate.gate == Gate::CONTROL) {
            const auto cfi_kind = ctrl::unpack_control_flow_kind(right_gate);
            auto cfi_index = ctrl::unpack_control_flow_index(right_gate);
            cfi_index += n_left_control_flow_instructions;
            left.gates_.emplace_back(ctrl::create_control_flow_gate(cfi_index, cfi_kind));
        }
        else {
            left.gates_.push_back(right_gate);
        }
    }

    // GateInfo vector can be extended trivially; none of the GateInfo gates themselves reference the indices
    const auto n_new_matrices = left.unitary_gates_.size() + right.unitary_gates_.size();
    left.unitary_gates_.reserve(n_new_matrices);
    left.unitary_gates_.insert(
        left.unitary_gates_.end(),
        right.unitary_gates_.begin(),
        right.unitary_gates_.end()
    );

    const auto n_new_instructions = left.control_flow_instructions_.size() + right.control_flow_instructions_.size();
    left.control_flow_instructions_.reserve(n_new_instructions);
    left.control_flow_instructions_.insert(
        left.control_flow_instructions_.end(),
        right.control_flow_instructions_.begin(),
        right.control_flow_instructions_.end()
    );
}

inline auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit
{
    extend_circuit(left, right);
    return left;
}


}  // namespace ket

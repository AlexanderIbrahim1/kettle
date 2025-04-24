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
    impl_ket::check_matching_number_of_qubits_(left, right);
    impl_ket::check_matching_number_of_bits_(left, right);

    const auto n_left_matrices = left.unitaries_.size();

    const auto n_new_gates = left.elements_.size() + right.elements_.size();
    left.elements_.reserve(n_new_gates);

    // TODO: write unit tests for extending the circuit with control flow

    // the U and CU gates refer to the indices of the held matrix in a vector in the QuantumCircuit;
    // they need to have their indices updated to reflect the new indices
    for (const auto& right_element : right.elements_) {
        if (right_element.is_control_flow()) {
            left.elements_.push_back(right_element);
        }
        else {
            const auto& right_gate = right_element.get_gate();

            if (right_gate.gate == Gate::U) {
                auto [target_index, matrix_index] = impl_ket::unpack_u_gate(right_gate);
                matrix_index += n_left_matrices;
                left.elements_.emplace_back(impl_ket::create_u_gate(target_index, matrix_index));
            }
            else if (right_gate.gate == Gate::CU) {
                auto [control_index, target_index, matrix_index] = impl_ket::unpack_cu_gate(right_gate);
                matrix_index += n_left_matrices;
                left.elements_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, matrix_index));
            }
            else {
                left.elements_.push_back(right_gate);
            }
        }
    }

    // vector of unitaries can be extended trivially; none of the unitaries themselves reference the indices
    const auto n_new_matrices = left.unitaries_.size() + right.unitaries_.size();
    left.unitaries_.reserve(n_new_matrices);
    left.unitaries_.insert(
        left.unitaries_.end(),
        right.unitaries_.begin(),
        right.unitaries_.end()
    );
}

inline auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit
{
    extend_circuit(left, right);
    return left;
}


}  // namespace ket

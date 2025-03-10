#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/circuit.hpp"

namespace impl_mqis
{

inline void check_matching_number_of_qubits_(const mqis::QuantumCircuit& left, const mqis::QuantumCircuit& right)
{
    if (left.n_qubits() != right.n_qubits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of qubits."};
    }
}

inline void check_matching_number_of_bits_(const mqis::QuantumCircuit& left, const mqis::QuantumCircuit& right)
{
    if (left.n_bits() != right.n_bits()) {
        throw std::runtime_error {"Cannot append circuits with different numbers of bits."};
    }
}

inline auto is_operating_on_measured_qubit_(const mqis::GateInfo& info, const std::vector<std::uint8_t>& measure_bitmask) -> bool
{
    if (is_single_qubit_gate(info)) {
        const auto target_index = unpack_single_qubit_gate_index(info);
        return measure_bitmask[target_index];
    }
    else if (is_double_qubit_gate(info)) {
        const auto [control_index, target_index] = unpack_double_qubit_gate_indices(info);
        return measure_bitmask[control_index] || measure_bitmask[target_index];
    }
    else {
        throw std::runtime_error {"UNREACHABLE: development error; invalid qubit gate found"};
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

}  // namespace impl_mqis

namespace mqis
{
    
inline auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit
{
    impl_mqis::check_matching_number_of_qubits_(left, right);
    impl_mqis::check_matching_number_of_bits_(left, right);

    const auto n_left_matrices = left.unitary_gates_.size();

    auto new_gates = left.gates_;
    new_gates.reserve(left.gates_.size() + right.gates_.size());

    for (const auto& right_gate : right.gates_) {
        if (impl_mqis::is_operating_on_measured_qubit_(right_gate, left.measure_bitmask_)) {
            throw std::runtime_error {
                "No gate on the right QuantumCircuit instance can act on a qubit that has already \n"
                "been measured on the left QuantumCircuit instance.\n"
            };
        }

        // the matrix-using gates refer to the index of the held matrix; so all the matrix-using gates
        // need to have their indices updated to reflect the new indices
        if (right_gate.gate == Gate::U) {
            auto [target_index, matrix_index] = impl_mqis::unpack_u_gate(right_gate);
            matrix_index += n_left_matrices;
            new_gates.emplace_back(impl_mqis::create_u_gate(target_index, matrix_index));
        }
        else if (right_gate.gate == Gate::CU) {
            auto [control_index, target_index, matrix_index] = impl_mqis::unpack_cu_gate(right_gate);
            matrix_index += n_left_matrices;
            new_gates.emplace_back(impl_mqis::create_cu_gate(control_index, target_index, matrix_index));
        }
        else {
            new_gates.push_back(right_gate);
        }
    }

    // the previous loop has already checked that there are no repeated measurements
    auto new_measure_bitmask = impl_mqis::bitwise_or(left.measure_bitmask_, right.measure_bitmask_);

    // GateInfo vector can be extended trivially; none of the GateInfo gates themselves reference the indices
    auto new_unitary_gates = left.unitary_gates_;
    new_unitary_gates.reserve(left.unitary_gates_.size() + right.unitary_gates_.size());
    new_unitary_gates.insert(new_unitary_gates.end(), right.unitary_gates_.begin(), right.unitary_gates_.end());

    left.measure_bitmask_ = std::move(new_measure_bitmask);
    left.gates_ = std::move(new_gates);
    left.unitary_gates_ = std::move(new_unitary_gates);

    return left;
}

}  // namespace mqis

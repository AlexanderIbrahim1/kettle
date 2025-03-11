#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"

namespace impl_mqis
{

inline auto non_u_gate_to_u_gate(const mqis::GateInfo& info) -> mqis::Matrix2X2
{
    using G = mqis::Gate;

    if (info.gate == G::X || info.gate == G::CX) {
        return mqis::x_gate();
    }
    else if (info.gate == G::RX || info.gate == G::CRX) {
        const auto angle = unpack_gate_angle(info);
        return mqis::rx_gate(angle);
    }
    else if (info.gate == G::H) {
        return mqis::h_gate();
    }
    else if (info.gate == G::CP) {
        const auto angle = unpack_gate_angle(info);
        return mqis::p_gate(angle);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, gate provided cannot be turned to a U-gate."};
    }
}

inline auto as_u_gate(const mqis::QuantumCircuit& circuit, const mqis::GateInfo& info) -> std::tuple<mqis::GateInfo, mqis::Matrix2X2>
{
    if (info.gate == mqis::Gate::U || info.gate == mqis::Gate::CU) {
        const auto i_matrix = impl_mqis::unpack_matrix_index(info);
        return {info, circuit.unitary_gate(i_matrix)};
    }
    else {
        const auto matrix = impl_mqis::non_u_gate_to_u_gate(info);

        if (impl_mqis::is_single_qubit_gate_and_not_u(info)) {
            const auto target = impl_mqis::unpack_single_qubit_gate_index(info);
            const auto u_gate_info = impl_mqis::create_u_gate(target, 0);  // TODO: replace 0 with dummy

            return {u_gate_info, matrix};
        }
        else if (impl_mqis::is_double_qubit_gate_and_not_cu(info)) {
            const auto [control, target] = impl_mqis::unpack_double_qubit_gate_indices(info);
            const auto u_gate_info = impl_mqis::create_cu_gate(control, target, 0);  // TODO: 0 replace with dummy

            return {u_gate_info, matrix};
        }
        else {
            throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'as_u_gate()'"};
        }
    }
}

inline auto is_matching_u_gate_info(const mqis::GateInfo& left_info, const mqis::GateInfo& right_info) -> bool
{
    if (left_info.gate != right_info.gate) {
        return false;
    }

    if (left_info.gate == mqis::Gate::U) {
        const auto unpack = impl_mqis::unpack_single_qubit_gate_index;
        return unpack(left_info) == unpack(right_info);
    }
    else if (left_info.gate == mqis::Gate::CU) {
        const auto unpack = impl_mqis::unpack_double_qubit_gate_indices;
        return unpack(left_info) == unpack(right_info);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'is_matching_u_gate_info()'"};
    }
}

}  // namespace impl_mqis


namespace mqis
{

auto almost_eq(
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double matrix_complex_tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> bool
{
    // begin with the fastest checks first (qubits, bits, and bitmask values)
    if (left.n_qubits() != right.n_qubits()) {
        return false;
    }

    if (left.n_bits() != right.n_bits()) {
        return false;
    }

    if (left.measure_bitmask() != right.measure_bitmask()) {
        return false;
    }

    // don't bother checking the gates if there aren't the same number on both sides
    const auto n_left_gates  = static_cast<std::size_t>(std::distance(left.begin(), left.end()));
    const auto n_right_gates = static_cast<std::size_t>(std::distance(right.begin(), right.end()));

    if (n_left_gates != n_right_gates) {
        return false;
    }

    for (std::size_t i_gate {0}; i_gate < n_left_gates; ++i_gate) {
        const auto& left_info = left[i_gate];
        const auto& right_info = right[i_gate];

        if (left_info.gate == Gate::M && right_info.gate == Gate::M) {
            return impl_mqis::unpack_m_gate(left_info) == impl_mqis::unpack_m_gate(right_info);
        }
        else if (left_info.gate == Gate::M && right_info.gate != Gate::M) {
            return false;
        }
        else if (left_info.gate != Gate::M && right_info.gate == Gate::M) {
            return false;
        }
        else {
            const auto [new_left_info, new_left_matrix] = impl_mqis::as_u_gate(left, left_info);
            const auto [new_right_info, new_right_matrix] = impl_mqis::as_u_gate(right, right_info);

            if (!impl_mqis::is_matching_u_gate_info(new_left_info, new_right_info)) {
                return false;
            }

            if (!almost_eq(new_left_matrix, new_right_matrix, matrix_complex_tolerance_sq)) {
                return false;
            }
        }
    }

    return true;
}

}  // namespace mqis

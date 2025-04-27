#pragma once

#include <cmath>
#include <stdexcept>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"

namespace impl_ket
{

inline auto non_u_gate_to_u_gate(const ket::GateInfo& info) -> ket::Matrix2X2
{
    if (impl_ket::gate_id::is_non_angle_transform_gate(info.gate)) {
        return ket::non_angle_gate(info.gate);
    }

    if (impl_ket::gate_id::is_angle_transform_gate(info.gate)) {
        const auto angle = impl_ket::unpack_gate_angle(info);
        return ket::angle_gate(info.gate, angle);
    }

    throw std::runtime_error {"UNREACHABLE: dev error, gate provided cannot be turned to a U-gate."};
}

inline auto as_u_gate(const ket::QuantumCircuit& circuit, const ket::GateInfo& info) -> std::tuple<ket::GateInfo, ket::Matrix2X2>
{
    using G = ket::Gate;

    if (info.gate == G::U || info.gate == G::CU) {
        const auto i_matrix = impl_ket::unpack_gate_matrix_index(info);
        return {info, circuit.unitary_gate(i_matrix)};
    }

    const auto matrix = non_u_gate_to_u_gate(info);
    const auto dummy_gate_index = 0;

    if (gate_id::is_single_qubit_transform_gate(info.gate) && info.gate != G::U) {
        const auto target = impl_ket::unpack_single_qubit_gate_index(info);
        const auto u_gate_info = impl_ket::create_u_gate(target, dummy_gate_index);

        return {u_gate_info, matrix};
    }

    if (gate_id::is_double_qubit_transform_gate(info.gate) && info.gate != G::CU) {
        const auto [control, target] = impl_ket::unpack_double_qubit_gate_indices(info);
        const auto u_gate_info = impl_ket::create_cu_gate(control, target, dummy_gate_index);

        return {u_gate_info, matrix};
    }

    throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'as_u_gate()'"};
}

inline auto is_matching_u_gate_info(const ket::GateInfo& left_info, const ket::GateInfo& right_info) -> bool
{
    if (left_info.gate != right_info.gate) {
        return false;
    }

    if (left_info.gate == ket::Gate::U) {
        const auto unpack = impl_ket::unpack_single_qubit_gate_index;
        return unpack(left_info) == unpack(right_info);
    }

    if (left_info.gate == ket::Gate::CU) {
        const auto unpack = impl_ket::unpack_double_qubit_gate_indices;
        return unpack(left_info) == unpack(right_info);
    }

    throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'is_matching_u_gate_info()'"};
}

}  // namespace impl_ket


namespace ket
{

// NOLINTNEXTLINE(misc-no-recursion, readability-function-cognitive-complexity)
inline auto almost_eq(
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double tol_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> bool
{
    namespace comp = impl_ket::compare;

    // begin with the fastest checks first (qubits, bits, and bitmask values)
    if (left.n_qubits() != right.n_qubits()) {
        return false;
    }

    if (left.n_bits() != right.n_bits()) {
        return false;
    }

    // don't bother checking the gates if there aren't the same number on both sides
    const auto n_left_gates  = static_cast<std::size_t>(std::distance(left.begin(), left.end()));
    const auto n_right_gates = static_cast<std::size_t>(std::distance(right.begin(), right.end()));

    if (n_left_gates != n_right_gates) {
        return false;
    }

    for (std::size_t i_gate {0}; i_gate < n_left_gates; ++i_gate) {
        const auto& left_element = left[i_gate];
        const auto& right_element = right[i_gate];

        if (left_element.is_control_flow() && right_element.is_control_flow()) {
            const auto& left_ctrl = left_element.get_control_flow();
            const auto& right_ctrl = right_element.get_control_flow();

            if (left_ctrl.is_if_statement() && right_ctrl.is_if_statement()) {
                const auto& left_if_stmt = left_ctrl.get_if_statement();
                const auto& right_if_stmt = right_ctrl.get_if_statement();

                if (left_if_stmt.predicate() != right_if_stmt.predicate()) {
                    return false;
                }

                if (!almost_eq(*left_if_stmt.circuit(), *right_if_stmt.circuit(), tol_sq)) {
                    return false;
                }
            }
            else if (left_ctrl.is_if_else_statement() && right_ctrl.is_if_else_statement()) {
                const auto& left_if_else_stmt = left_ctrl.get_if_else_statement();
                const auto& right_if_else_stmt = right_ctrl.get_if_else_statement();

                if (left_if_else_stmt.predicate() != right_if_else_stmt.predicate()) {
                    return false;
                }

                if (!almost_eq(*left_if_else_stmt.if_circuit(), *right_if_else_stmt.if_circuit(), tol_sq)) {
                    return false;
                }

                if (!almost_eq(*left_if_else_stmt.else_circuit(), *right_if_else_stmt.else_circuit(), tol_sq)) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (left_element.is_gate() && right_element.is_gate()) {
            const auto& left_gate = left_element.get_gate();
            const auto& right_gate = right_element.get_gate();

            if (left_gate.gate == Gate::M && right_gate.gate == Gate::M) {
                if (!comp::is_m_gate_equal(left_gate, right_gate)) {
                    return false;
                }
            }
            else if (left_gate.gate != Gate::M && right_gate.gate != Gate::M) {
                const auto [new_left_gate, new_left_matrix] = impl_ket::as_u_gate(left, left_gate);
                const auto [new_right_gate, new_right_matrix] = impl_ket::as_u_gate(right, right_gate);

                if (!impl_ket::is_matching_u_gate_info(new_left_gate, new_right_gate)) {
                    return false;
                }

                if (!almost_eq(new_left_matrix, new_right_matrix, tol_sq)) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    return true;
}

}  // namespace ket

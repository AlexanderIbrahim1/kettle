#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <vector>

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
    else if (impl_ket::gate_id::is_angle_transform_gate(info.gate)) {
        const auto angle = impl_ket::unpack_gate_angle(info);
        return ket::angle_gate(info.gate, angle);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, gate provided cannot be turned to a U-gate."};
    }
}

inline auto as_u_gate(const ket::QuantumCircuit& circuit, const ket::GateInfo& info) -> std::tuple<ket::GateInfo, ket::Matrix2X2>
{
    using G = ket::Gate;

    if (info.gate == G::U || info.gate == G::CU) {
        const auto i_matrix = impl_ket::unpack_gate_matrix_index(info);
        return {info, circuit.unitary_gate(i_matrix)};
    }
    else {
        const auto matrix = non_u_gate_to_u_gate(info);
        const auto dummy_gate_index = 0;

        if (gate_id::is_single_qubit_transform_gate(info.gate) && info.gate != G::U) {
            const auto target = impl_ket::unpack_single_qubit_gate_index(info);
            const auto u_gate_info = impl_ket::create_u_gate(target, dummy_gate_index);

            return {u_gate_info, matrix};
        }
        else if (gate_id::is_double_qubit_transform_gate(info.gate) && info.gate != G::CU) {
            const auto [control, target] = impl_ket::unpack_double_qubit_gate_indices(info);
            const auto u_gate_info = impl_ket::create_cu_gate(control, target, dummy_gate_index);

            return {u_gate_info, matrix};
        }
        else {
            throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'as_u_gate()'"};
        }
    }
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
    else if (left_info.gate == ket::Gate::CU) {
        const auto unpack = impl_ket::unpack_double_qubit_gate_indices;
        return unpack(left_info) == unpack(right_info);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'is_matching_u_gate_info()'"};
    }
}

}  // namespace impl_ket


namespace ket
{

auto almost_eq(
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double matrix_complex_tolerance_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> bool
{
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
        const auto& left_info = left[i_gate];
        const auto& right_info = right[i_gate];

        if (left_info.gate == Gate::M && right_info.gate == Gate::M) {
            return impl_ket::unpack_m_gate(left_info) == impl_ket::unpack_m_gate(right_info);
        }
        else if (left_info.gate == Gate::M && right_info.gate != Gate::M) {
            return false;
        }
        else if (left_info.gate != Gate::M && right_info.gate == Gate::M) {
            return false;
        }
        else {
            const auto [new_left_info, new_left_matrix] = impl_ket::as_u_gate(left, left_info);
            const auto [new_right_info, new_right_matrix] = impl_ket::as_u_gate(right, right_info);

            if (!impl_ket::is_matching_u_gate_info(new_left_info, new_right_info)) {
                return false;
            }

            if (!almost_eq(new_left_matrix, new_right_matrix, matrix_complex_tolerance_sq)) {
                return false;
            }
        }
    }

    return true;
}

}  // namespace ket

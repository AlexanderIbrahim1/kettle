#include <cmath>
#include <stdexcept>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/parameter/parameter.hpp"

#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/gates/primitive_gate/gate_id.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/gates/primitive_gate/gate_compare.hpp"

namespace kp = ket::param;
namespace kpi = ket::param::internal;

namespace
{

auto non_u_gate_to_u_gate_(const kp::EvaluatedParameterDataMap& param_map, const ket::GateInfo& info) -> ket::Matrix2X2
{
    if (ket::internal::gate_id::is_non_angle_transform_gate(info.gate)) {
        return ket::non_angle_gate(info.gate);
    }

    if (ket::internal::gate_id::is_angle_transform_gate(info.gate)) {
        const auto angle = [&]() {
            if (info.param_expression_ptr) {
                return kpi::Evaluator{}.evaluate(*info.param_expression_ptr, param_map);
            } else {
                return ket::internal::create::unpack_gate_angle(info);
            }
        }();

        return ket::angle_gate(info.gate, angle);
    }

    throw std::runtime_error {"UNREACHABLE: dev error, gate provided cannot be turned to a U-gate."};
}

auto as_u_gate_(const kp::EvaluatedParameterDataMap& param_map, const ket::GateInfo& info) -> ket::GateInfo
{
    using G = ket::Gate;

    if (info.gate == G::U || info.gate == G::CU) {
        return info;
    }

    const auto u_gate = non_u_gate_to_u_gate_(param_map, info);
    auto unitary = ket::ClonePtr<ket::Matrix2X2> {u_gate};

    if (ket::internal::gate_id::is_single_qubit_transform_gate(info.gate) && info.gate != G::U) {
        const auto target = ket::internal::create::unpack_single_qubit_gate_index(info);
        const auto u_gate_info = ket::internal::create::create_u_gate(target, std::move(unitary));

        return u_gate_info;
    }

    if (ket::internal::gate_id::is_double_qubit_transform_gate(info.gate) && info.gate != G::CU) {
        const auto [control, target] = ket::internal::create::unpack_double_qubit_gate_indices(info);
        const auto u_gate_info = ket::internal::create::create_cu_gate(control, target, std::move(unitary));

        return u_gate_info;
    }

    throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'as_u_gate()'"};
}

auto have_matching_indices_(const ket::GateInfo& left_info, const ket::GateInfo& right_info) -> bool
{
    if (left_info.gate != right_info.gate) {
        return false;
    }

    if (left_info.gate == ket::Gate::U) {
        const auto unpack = ket::internal::create::unpack_single_qubit_gate_index;
        return unpack(left_info) == unpack(right_info);
    }

    if (left_info.gate == ket::Gate::CU) {
        const auto unpack = ket::internal::create::unpack_double_qubit_gate_indices;
        return unpack(left_info) == unpack(right_info);
    }

    throw std::runtime_error {"UNREACHABLE: dev error, invalid Gate found in 'have_matching_indices_()'"};
}

auto all_remaining_elements_are_circuit_loggers_(const ket::QuantumCircuit& circuit, std::size_t i_start) -> bool
{
    if (i_start >= circuit.n_circuit_elements()) {
        throw std::runtime_error {"DEV ERROR: cannot check if remaining elements are circuit loggers\n"};
    }

    for (std::size_t i {i_start}; i < circuit.n_circuit_elements(); ++i) {
        if (!circuit[i].is_circuit_logger()) {
            return false;
        }
    }

    return true;
}

}  // namespace


namespace ket
{

// TODO: split the branching inside the "is_circuit_logger" and "is_gate" sections into separate functions,
// to make the function easier to read
auto almost_eq(  // NOLINT(misc-no-recursion, readability-function-cognitive-complexity)
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double tol_sq
) -> bool
{
    namespace comp = ket::internal::compare;
    namespace gid = ket::internal::gate_id;

    // begin with the fastest checks first (qubits, bits, and bitmask values)
    if (left.n_qubits() != right.n_qubits()) {
        return false;
    }

    if (left.n_bits() != right.n_bits()) {
        return false;
    }

    // don't bother checking the gates if there aren't the same number on both sides
    auto i_left = std::size_t {0};
    auto i_right = std::size_t {0};

    const auto& left_param_map = kpi::create_parameter_values_map(left.parameter_data_map());
    const auto& right_param_map = kpi::create_parameter_values_map(right.parameter_data_map());

    while (i_left < left.n_circuit_elements() && i_right < right.n_circuit_elements()) {
        const auto& left_element = left[i_left];
        if (left_element.is_circuit_logger()) {
            ++i_left;
            continue;
        }

        const auto& right_element = right[i_right];
        if (right_element.is_circuit_logger()) {
            ++i_right;
            continue;
        }

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
            else if (left_gate.gate == Gate::RESET && right_gate.gate == Gate::RESET) {
                if (!comp::is_reset_gate_equal(left_gate, right_gate)) {
                    return false;
                }
            }
            else if (gid::is_unitary_gate(left_gate.gate) && gid::is_unitary_gate(right_gate.gate)) {
                const auto new_left_gate = as_u_gate_(left_param_map, left_gate);
                const auto new_right_gate = as_u_gate_(right_param_map, right_gate);

                if (!have_matching_indices_(new_left_gate, new_right_gate)) {
                    return false;
                }

                if (!almost_eq(*new_left_gate.unitary_ptr, *new_right_gate.unitary_ptr, tol_sq)) {
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

        ++i_left;
        ++i_right;
    }

    if (i_left == left.n_circuit_elements() && i_right < right.n_circuit_elements()) {
        if (!all_remaining_elements_are_circuit_loggers_(right, i_right)) {
            return false;
        }
    }

    if (i_left < left.n_circuit_elements() && i_right == right.n_circuit_elements()) {
        if (!all_remaining_elements_are_circuit_loggers_(left, i_left)) {
            return false;
        }
    }

    return true;
}

}  // namespace ket

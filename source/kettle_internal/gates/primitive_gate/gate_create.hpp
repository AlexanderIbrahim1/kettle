#pragma once

#include <cmath>
#include <cstddef>
#include <tuple>

#include "kettle/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/parameter/parameter_expression.hpp"

#include "kettle/gates/primitive_gate.hpp"


namespace ket::internal::create
{

/*
    Create a single-qubit gate with no parameters.
*/
auto create_one_target_gate(ket::PrimitiveGate gate, std::size_t target_index) -> ket::PrimitiveGateInfo;

/*
    Returns the `target_qubit` of a single-qubit gate with no parameters.
*/
auto unpack_one_target_gate(const ket::PrimitiveGateInfo& info) -> std::size_t;

/*
    Create a single-qubit gate with an angle parameter.
*/
auto create_one_target_one_angle_gate(ket::PrimitiveGate gate, std::size_t target_index, double theta) -> ket::PrimitiveGateInfo;

/*
    Returns the `{target_qubit, angle}` of a single-qubit gate with an angle parameter.
*/
auto unpack_one_target_one_angle_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, double>;

/*
    Create a single-qubit gate with a parameterized angle.
*/
auto create_one_target_one_parameter_gate(ket::PrimitiveGate gate, std::size_t target_index, ket::param::ParameterExpression param_expression) -> ket::PrimitiveGateInfo;

/*
    Returns the `{target_qubit, param_expression_ptr}` of a single-qubit gate with an angle parameter.
*/
auto unpack_one_target_one_parameter_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, const ket::ClonePtr<ket::param::ParameterExpression>&>;

/*
    Create a controlled gate with no parameters.
*/
auto create_one_control_one_target_gate(ket::PrimitiveGate gate, std::size_t control_index, std::size_t target_index) -> ket::PrimitiveGateInfo;

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate with no parameters.
*/
auto unpack_one_control_one_target_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>;

/*
    Create a controlled gate with an angle parameter.
*/
auto create_one_control_one_target_one_angle_gate(ket::PrimitiveGate gate, std::size_t control_index, std::size_t target_index, double theta) -> ket::PrimitiveGateInfo;

/*
    Returns the `{control_qubit, target_qubit, angle}` of a double-qubit gate with an angle parameter.
*/
auto unpack_one_control_one_target_one_angle_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t, double>;

auto create_one_control_one_target_one_parameter_gate(
    ket::PrimitiveGate gate,
    std::size_t control_index,
    std::size_t target_index,
    ket::param::ParameterExpression param_expression
) -> ket::PrimitiveGateInfo;

auto unpack_one_control_one_target_one_parameter_gate(
    const ket::PrimitiveGateInfo& info
) -> std::tuple<std::size_t, std::size_t, const ket::ClonePtr<ket::param::ParameterExpression>&>;

/*
    Create a U-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`.
*/
auto create_u_gate(std::size_t target_index, ket::ClonePtr<ket::Matrix2X2> unitary) -> ket::PrimitiveGateInfo;

/*
    Returns the `{target_qubit, unitary_ptr}` of a U-gate.
*/
auto unpack_u_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, const ket::ClonePtr<ket::Matrix2X2>&>;

/*
    Create a CU-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`,
    controlled by the qubit at index `control_index`.
*/
auto create_cu_gate(std::size_t control_index, std::size_t target_index, ket::ClonePtr<ket::Matrix2X2> unitary) -> ket::PrimitiveGateInfo;

/*
    Returns the `{control_qubit, target_qubit, unitary_ptr}` of a CU-gate.
*/
auto unpack_cu_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t, const ket::ClonePtr<ket::Matrix2X2>&>;

/*
    Create an M-gate, which measures the qubit at `qubit_index`, and stores the result at `bit_index`.
*/
auto create_m_gate(std::size_t qubit_index, std::size_t bit_index) -> ket::PrimitiveGateInfo;

/*
    Returns the `{qubit_index, bit_index}` of an M-gate.
*/
auto unpack_m_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>;

/*
    Returns the `target_qubit` of a single-qubit gate, with or without parameters.
*/
auto unpack_single_qubit_gate_index(const ket::PrimitiveGateInfo& info) -> std::size_t;

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate, with or without parameters.
*/
auto unpack_double_qubit_gate_indices(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>;

/*
    Returns the `angle` of a single-qubit gate or double-qubit gate, as long as it is parameterized.
*/
auto unpack_gate_angle(const ket::PrimitiveGateInfo& info) -> double;

/*
    Returns the `unitary_ptr` of a U-gate or CU-gate.
*/
auto unpack_unitary_matrix(const ket::PrimitiveGateInfo& info) -> const ket::ClonePtr<ket::Matrix2X2>&;

}  // namespace ket::internal::create

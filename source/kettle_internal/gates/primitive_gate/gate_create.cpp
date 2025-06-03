#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <tuple>

#include "kettle/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/parameter/parameter_expression.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/gates/primitive_gate/gate_id.hpp"

// TODO: get rid of the comments here, and update the comments in the corresponding header file

namespace ket::internal::create
{

/*
    Parameters indicating to the developer that a given gate does not use a certain data member in
    a ket::PrimitiveGateInfo instance.

    Note that DUMMY_ARG3 cannot be constexpr, because ClonePtr holds `std::unique_ptr<T>`, which is
    not constexpr in C++20 (only in C++23).
*/
constexpr inline auto DUMMY_ARG1 = std::size_t {0};
constexpr inline auto DUMMY_ARG2 = double {0.0};
const inline auto DUMMY_ARG3 = ket::ClonePtr<ket::Matrix2X2> {nullptr};
const inline auto DUMMY_ARG4 = ket::ClonePtr<ket::param::ParameterExpression> {nullptr};

/*
    Create a single-qubit gate with no parameters.
*/
auto create_one_target_gate(ket::PrimitiveGate gate, std::size_t target_index) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3, .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `target_qubit` of a single-qubit gate with no parameters.
*/
auto unpack_one_target_gate(const ket::PrimitiveGateInfo& info) -> std::size_t
{
    return info.arg0;  // target index
}

/*
    Create a single-qubit gate with a fixed angle.
*/
auto create_one_target_one_angle_gate(ket::PrimitiveGate gate, std::size_t target_index, double theta) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=theta, .unitary_ptr=DUMMY_ARG3, .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{target_qubit, angle}` of a single-qubit gate with a fixed angle.
*/
auto unpack_one_target_one_angle_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, double>
{
    return {info.arg0, info.arg2};  // target index, angle
}

/*
    Create a single-qubit gate with a parameterized angle.
*/
auto create_one_target_one_parameter_gate(ket::PrimitiveGate gate, std::size_t target_index, ket::param::ParameterExpression param_expression) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target-one-angle gate provided.\n"};
    }

    return {
        .gate=gate,
        .arg0=target_index,
        .arg1=DUMMY_ARG1,
        .arg2=DUMMY_ARG2,
        .unitary_ptr=DUMMY_ARG3,
        .param_expression_ptr=ket::ClonePtr {std::move(param_expression)}
    };
}

/*
    Returns the `{target_qubit, param_expression_ptr}` of a single-qubit gate with an angle parameter.
*/
auto unpack_one_target_one_parameter_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, const ket::ClonePtr<ket::param::ParameterExpression>&>
{
    return {info.arg0, info.param_expression_ptr};  // target index, param_expression_ptr
}

/*
    Create a controlled gate with no parameters.
*/
auto create_one_control_one_target_gate(ket::PrimitiveGate gate, std::size_t control_index, std::size_t target_index) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_control_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3, .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate with no parameters.
*/
auto unpack_one_control_one_target_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control index, target_index
}

/*
    Create a controlled gate with a fixed angle.
*/
auto create_one_control_one_target_one_angle_gate(ket::PrimitiveGate gate, std::size_t control_index, std::size_t target_index, double theta) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_control_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=theta, .unitary_ptr=DUMMY_ARG3, .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{control_qubit, target_qubit, angle}` of a double-qubit gate with a fixed angle.
*/
auto unpack_one_control_one_target_one_angle_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t, double>
{
    return {info.arg0, info.arg1, info.arg2};  // control index, target index, angle
}

auto create_one_control_one_target_one_parameter_gate(
    ket::PrimitiveGate gate,
    std::size_t control_index,
    std::size_t target_index,
    ket::param::ParameterExpression param_expression
) -> ket::PrimitiveGateInfo
{
    if (!gate_id::is_one_control_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target-one-angle gate provided.\n"};
    }

    return {
        .gate=gate,
        .arg0=control_index,
        .arg1=target_index,
        .arg2=DUMMY_ARG2,
        .unitary_ptr=DUMMY_ARG3,
        .param_expression_ptr=ket::ClonePtr {std::move(param_expression)}
    };
}

auto unpack_one_control_one_target_one_parameter_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t, const ket::ClonePtr<ket::param::ParameterExpression>&>
{
    return {info.arg0, info.arg1, info.param_expression_ptr};  // control index, target index, param_expression_ptr
}


/*
    Create a U-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`.
*/
auto create_u_gate(std::size_t target_index, ket::ClonePtr<ket::Matrix2X2> unitary) -> ket::PrimitiveGateInfo
{
    return {.gate=ket::PrimitiveGate::U, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .unitary_ptr=std::move(unitary), .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{target_qubit, unitary_ptr}` of a U-gate.
*/
auto unpack_u_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, const ket::ClonePtr<ket::Matrix2X2>&>
{
    return {info.arg0, info.unitary_ptr};  // target index, unitary_ptr
}

/*
    Create a CU-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`,
    controlled by the qubit at index `control_index`.
*/
auto create_cu_gate(std::size_t control_index, std::size_t target_index, ket::ClonePtr<ket::Matrix2X2> unitary) -> ket::PrimitiveGateInfo
{
    return {.gate=ket::PrimitiveGate::CU, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .unitary_ptr=std::move(unitary), .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{control_qubit, target_qubit, unitary_ptr}` of a CU-gate.
*/
auto unpack_cu_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t, const ket::ClonePtr<ket::Matrix2X2>&>
{
    return {info.arg0, info.arg1, info.unitary_ptr};  // control index, target index, unitary_ptr
}

/*
    Create an M-gate, which measures the qubit at `qubit_index`, and stores the result at `bit_index`.
*/
auto create_m_gate(std::size_t qubit_index, std::size_t bit_index) -> ket::PrimitiveGateInfo
{
    return {.gate=ket::PrimitiveGate::M, .arg0=qubit_index, .arg1=bit_index, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3, .param_expression_ptr=DUMMY_ARG4};
}

/*
    Returns the `{qubit_index, bit_index}` of an M-gate.
*/
auto unpack_m_gate(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // qubit index, bit index
}

/*
    Returns the `target_qubit` of a single-qubit gate, with or without parameters.
*/
auto unpack_single_qubit_gate_index(const ket::PrimitiveGateInfo& info) -> std::size_t
{
    return info.arg0;  // target_index
}

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate, with or without parameters.
*/
auto unpack_double_qubit_gate_indices(const ket::PrimitiveGateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control_index, target_index
}

/*
    Returns the `angle` of a single-qubit gate or double-qubit gate, as long as it is parameterized.
*/
auto unpack_gate_angle(const ket::PrimitiveGateInfo& info) -> double
{
    return info.arg2;  // angle
}

/*
    Returns the `unitary_ptr` of a U-gate or CU-gate.
*/
auto unpack_unitary_matrix(const ket::PrimitiveGateInfo& info) -> const ket::ClonePtr<ket::Matrix2X2>&
{
    return info.unitary_ptr;  // unitary_ptr
}

}  // namespace ket::internal::create

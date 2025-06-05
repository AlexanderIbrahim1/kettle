#pragma once

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"

namespace ket::param::internal
{

// the visitor pattern doesn't work unless the object is wrapped in a std::variant
using MapVariant = std::variant<std::reference_wrapper<const EvaluatedParameterDataMap>>;

struct Evaluator
{
    auto operator()(const Parameter& expr, const MapVariant& param_map) const -> double;

    auto operator()(const LiteralExpression& expr, [[maybe_unused]] const MapVariant& param_map) const -> double;

    auto operator()(const BinaryExpression& expr, const MapVariant& param_map) const -> double;

    [[nodiscard]]
    auto evaluate(const ParameterExpression& expr, const MapVariant& param_map) const -> double;
};

/*
    Unpack the target qubit index and angle of a one-target-one-angle unitary gate.
    
    If the gate is parameterized, then the associated value from `parameter_values_map` is
    used; otherwise, the fixed angle assigned to the gate is used.
*/
auto unpack_target_and_angle(
    const MapVariant& parameter_values_map,
    const ket::GateInfo& info
) -> std::tuple<std::size_t, double>;

/*
    Unpack the control qubit index, target qubit index, and angle of a one-target-one-angle
    unitary gate.
    
    If the gate is parameterized, then the associated value from `parameter_values_map` is
    used; otherwise, the fixed angle assigned to the gate is used.
*/
auto unpack_control_target_and_angle(
    const MapVariant& parameter_values_map,
    const ket::GateInfo& info
) -> std::tuple<std::size_t, std::size_t, double>;

/*
    Evaluate all the `ParameterExpression` instances in `param_data_map`, to get the
    actual floating-point values for all the parameters.
*/
auto create_parameter_values_map(const ParameterDataMap& param_data_map) -> EvaluatedParameterDataMap;

}  // namespace ket::param::internal

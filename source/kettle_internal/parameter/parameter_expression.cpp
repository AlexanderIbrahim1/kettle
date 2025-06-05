#include <stdexcept>
#include <variant>

#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"

#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"

namespace kp = ket::param;
namespace ki = ket::internal;

namespace ket::param::internal
{

auto Evaluator::operator()(const Parameter& expr, const MapVariant& param_map) const -> double
{
    const auto& params = std::get<std::reference_wrapper<const kp::EvaluatedParameterDataMap>>(param_map).get();

    if (!params.contains(expr.id())) {
        throw std::runtime_error {"DEV ERROR: unable to find parameter during expression evaluation.\n"};
    }

    return params.at(expr.id());
}

auto Evaluator::operator()(const LiteralExpression& expr, [[maybe_unused]] const MapVariant& param_map) const -> double
{
    return expr.value;
}

auto Evaluator::operator()(const BinaryExpression& expr, const MapVariant& param_map) const -> double
{
    switch (expr.operation)
    {
        case BinaryOperation::ADD : {
            return evaluate(*expr.left, param_map) + evaluate(*expr.right, param_map);
        }
        case BinaryOperation::MUL : {
            return evaluate(*expr.left, param_map) * evaluate(*expr.right, param_map);
        }
        default : {
            throw std::runtime_error {"DEV ERROR: found invalid binary operation between parameter expressions\n"};
        }
    }
}

[[nodiscard]]
auto Evaluator::evaluate(const ParameterExpression& expr, const MapVariant& param_map) const -> double
{
    return std::visit(*this, expr, param_map);
}

auto unpack_target_and_angle(
    const MapVariant& parameter_values_map,
    const ket::GateInfo& info
) -> std::tuple<std::size_t, double>
{
    if (info.param_expression_ptr) {
        auto [target_qubit, param_expression_ptr] = ki::create::unpack_one_target_one_parameter_gate(info);
        auto angle = Evaluator{}.evaluate(*param_expression_ptr, parameter_values_map);

        return {target_qubit, angle};
    } else {
        return ki::create::unpack_one_target_one_angle_gate(info);
    }
}

auto unpack_control_target_and_angle(
    const MapVariant& parameter_values_map,
    const ket::GateInfo& info
) -> std::tuple<std::size_t, std::size_t, double>
{
    if (info.param_expression_ptr) {
        auto [control_qubit, target_qubit, param_expression_ptr] = ki::create::unpack_one_control_one_target_one_parameter_gate(info);
        auto angle = Evaluator{}.evaluate(*param_expression_ptr, parameter_values_map);

        return {control_qubit, target_qubit, angle};
    } else {
        return ki::create::unpack_one_control_one_target_one_angle_gate(info);
    }
}

auto create_parameter_values_map(const ParameterDataMap& param_data_map) -> EvaluatedParameterDataMap
{
    auto output = EvaluatedParameterDataMap {};

    for (const auto& [id, data]: param_data_map) {
        if (data.value == std::nullopt) {
            throw std::runtime_error {"ERROR: cannot perform simulation with an uninitialized parameter value.\n"};
        }

        output[id] = data.value.value();
    }

    return output;
}

}  // namespace ket::param::internal

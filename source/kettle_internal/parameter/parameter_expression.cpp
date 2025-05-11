#include <stdexcept>
#include <variant>

#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"
#include "kettle_internal/parameter/parameter_expression_internal.hpp"

namespace ket::param::internal
{

auto Evaluator::operator()(const Parameter& expr, const MapVariant& param_map) const -> double
{
    const auto& params = std::get<std::reference_wrapper<const Map>>(param_map);
    return params.get().at(expr.id());
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

}  // namespace ket::param::internal

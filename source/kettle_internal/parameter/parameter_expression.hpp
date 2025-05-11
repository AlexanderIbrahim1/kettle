#pragma once

#include <map>
#include <stdexcept>
#include <variant>

#include "kettle/common/clone_ptr.hpp"
#include "kettle/parameter/parameter.hpp"

namespace ket::param
{

struct LiteralExpression;
struct BinaryExpression;

using Expression = std::variant<Parameter, LiteralExpression, BinaryExpression>;
using ExpressionHandle = ClonePtr<Expression>;
using Map = std::map<ParameterID, double>;
using MapVariant = std::variant<std::reference_wrapper<const Map>>;

enum class ParameterOperation : std::uint8_t
{
    ADD,
    MUL
};

struct LiteralExpression
{
    double value;
};

struct BinaryExpression
{
    ParameterOperation operation;
    ExpressionHandle left;
    ExpressionHandle right;
};

struct Evaluator
{
    double operator()(const Parameter& expr, const MapVariant& param_map) const
    {
        const auto& params = std::get<std::reference_wrapper<const Map>>(param_map);
        return params.get().at(expr.id());
    }

    double operator()(const LiteralExpression& expr, [[maybe_unused]] const MapVariant& param_map) const
    {
        return expr.value;
    }

    double operator()(const BinaryExpression& expr, const MapVariant& param_map) const
    {
        switch (expr.operation)
        {
            case ParameterOperation::ADD : {
                return evaluate(*expr.left, param_map) + evaluate(*expr.right, param_map);
            }
            case ParameterOperation::MUL : {
                return evaluate(*expr.left, param_map) * evaluate(*expr.right, param_map);
            }
            default : {
                throw std::runtime_error {"DEV ERROR: found invalid binary operation between parameter expressions\n"};
            }
        }
    }

    [[nodiscard]]
    double evaluate(const Expression& expr, const MapVariant& param_map) const
    {
        return std::visit(*this, expr, param_map);
    }
};


}  // namespace ket::param

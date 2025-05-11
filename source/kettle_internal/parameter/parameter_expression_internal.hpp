#pragma once

#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"

namespace ket::param::internal
{

struct Evaluator
{
    auto operator()(const Parameter& expr, const MapVariant& param_map) const -> double;

    auto operator()(const LiteralExpression& expr, [[maybe_unused]] const MapVariant& param_map) const -> double;

    auto operator()(const BinaryExpression& expr, const MapVariant& param_map) const -> double;

    [[nodiscard]]
    auto evaluate(const Expression& expr, const MapVariant& param_map) const -> double;
};


}  // namespace ket::param::internal

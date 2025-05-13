#pragma once

#include <unordered_map>

#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"

namespace ket::param::internal
{

using Map = std::unordered_map<ParameterID, double, ParameterIdHash>;
using MapVariant = std::variant<std::reference_wrapper<const Map>>;

struct Evaluator
{
    auto operator()(const Parameter& expr, const MapVariant& param_map) const -> double;

    auto operator()(const LiteralExpression& expr, [[maybe_unused]] const MapVariant& param_map) const -> double;

    auto operator()(const BinaryExpression& expr, const MapVariant& param_map) const -> double;

    [[nodiscard]]
    auto evaluate(const ParameterExpression& expr, const MapVariant& param_map) const -> double;
};


}  // namespace ket::param::internal

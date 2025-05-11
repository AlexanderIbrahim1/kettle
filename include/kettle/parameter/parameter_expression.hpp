#pragma once

#include <map>
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

}  // namespace ket::param

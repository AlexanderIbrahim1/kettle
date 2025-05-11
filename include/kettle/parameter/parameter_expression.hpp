#pragma once

#include <variant>

#include "kettle/common/clone_ptr.hpp"
#include "kettle/parameter/parameter.hpp"

namespace ket::param
{

enum class BinaryOperation : std::uint8_t
{
    ADD,
    MUL
};

struct LiteralExpression;
struct BinaryExpression;

using ParameterExpression = std::variant<Parameter, LiteralExpression, BinaryExpression>;

struct LiteralExpression
{
    double value;
};

struct BinaryExpression
{
    BinaryOperation operation;
    ClonePtr<ParameterExpression> left;
    ClonePtr<ParameterExpression> right;
};

}  // namespace ket::param

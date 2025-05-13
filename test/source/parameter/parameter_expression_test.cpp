#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"
#include "kettle_internal/parameter/parameter_expression_internal.hpp"


namespace kp = ket::param;
namespace kpi = ket::param::internal;

TEST_CASE("EvaluateExpression")
{
    const auto dummy_map = kpi::Map {};
    const auto dummy_map_variant = kpi::MapVariant {std::reference_wrapper {dummy_map}};

    SECTION("a single literal")
    {
        const auto expr = kp::LiteralExpression {1.5};

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("a single parameter")
    {
        const auto parameter = kp::Parameter {"theta"};
        const auto map = kpi::Map { {parameter.id(), 1.5} };
        const auto map_variant = kpi::MapVariant {std::reference_wrapper {map}};

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(parameter, map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("addition between two literals")
    {
        const auto expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::ADD,
            .left=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {0.4}}}
        };

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("addition between literal and parameter")
    {
        const auto parameter = kp::Parameter {"theta"};
        const auto map = kpi::Map { {parameter.id(), 1.5} };
        const auto map_variant = kpi::MapVariant {std::reference_wrapper {map}};

        const auto expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::ADD,
            .left=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {kp::ParameterExpression {parameter}}
        };

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel(2.6));
    }

    SECTION("addition between two parameters")
    {
        const auto theta = kp::Parameter {"theta"};
        const auto phi = kp::Parameter {"phi"};
        const auto map = kpi::Map { {theta.id(), 1.5}, {phi.id(), -0.4} };
        const auto map_variant = kpi::MapVariant {std::reference_wrapper {map}};

        const auto expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::ADD,
            .left=ket::ClonePtr {kp::ParameterExpression {theta}},
            .right=ket::ClonePtr {kp::ParameterExpression {phi}}
        };

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel(1.1));
    }

    SECTION("multiplication between two literals")
    {
        const auto expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::MUL,
            .left=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {0.5}}}
        };

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(0.55));
    }

    SECTION("multiply parameter and literal, then add literal")
    {
        const auto theta = kp::Parameter {"theta"};
        const auto map = kpi::Map { {theta.id(), 1.5} };
        const auto map_variant = kpi::MapVariant {std::reference_wrapper {map}};

        const auto left_expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::MUL,
            .left=ket::ClonePtr {kp::ParameterExpression {theta}},
            .right=ket::ClonePtr {kp::ParameterExpression {kp::LiteralExpression {0.5}}}
        };

        const auto right_expr = kp::LiteralExpression {2.2};

        const auto expr = kp::BinaryExpression {
            .operation=kp::BinaryOperation::ADD,
            .left=ket::ClonePtr {kp::ParameterExpression {left_expr}},
            .right=ket::ClonePtr {kp::ParameterExpression {right_expr}}
        };

        const auto evaluator = kpi::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel((1.5 * 0.5) + 2.2));
    }
}

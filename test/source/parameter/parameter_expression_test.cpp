#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/parameter/parameter.hpp"
#include "kettle_internal/parameter/parameter_expression.hpp"


TEST_CASE("EvaluateExpression")
{
    const auto dummy_map = ket::Map {};
    const auto dummy_map_variant = ket::MapVariant {std::reference_wrapper {dummy_map}};

    SECTION("a single literal")
    {
        const auto expr = ket::LiteralExpression {1.5};

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("a single parameter")
    {
        const auto parameter = ket::Parameter {"theta"};
        const auto map = ket::Map { {parameter.id(), 1.5} };
        const auto map_variant = ket::MapVariant {std::reference_wrapper {map}};

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(parameter, map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("addition between two literals")
    {
        const auto expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::ADD,
            .left=ket::ClonePtr {ket::Expression {ket::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {ket::Expression {ket::LiteralExpression {0.4}}}
        };

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(1.5));
    }

    SECTION("addition between literal and parameter")
    {
        const auto parameter = ket::Parameter {"theta"};
        const auto map = ket::Map { {parameter.id(), 1.5} };
        const auto map_variant = ket::MapVariant {std::reference_wrapper {map}};

        const auto expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::ADD,
            .left=ket::ClonePtr {ket::Expression {ket::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {ket::Expression {parameter}}
        };

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel(2.6));
    }

    SECTION("addition between two parameters")
    {
        const auto theta = ket::Parameter {"theta"};
        const auto phi = ket::Parameter {"phi"};
        const auto map = ket::Map { {theta.id(), 1.5}, {phi.id(), -0.4} };
        const auto map_variant = ket::MapVariant {std::reference_wrapper {map}};

        const auto expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::ADD,
            .left=ket::ClonePtr {ket::Expression {theta}},
            .right=ket::ClonePtr {ket::Expression {phi}}
        };

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel(1.1));
    }

    SECTION("multiplication between two literals")
    {
        const auto expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::MUL,
            .left=ket::ClonePtr {ket::Expression {ket::LiteralExpression {1.1}}},
            .right=ket::ClonePtr {ket::Expression {ket::LiteralExpression {0.5}}}
        };

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, dummy_map_variant), Catch::Matchers::WithinRel(0.55));
    }

    SECTION("multiply parameter and literal, then add literal")
    {
        const auto theta = ket::Parameter {"theta"};
        const auto map = ket::Map { {theta.id(), 1.5} };
        const auto map_variant = ket::MapVariant {std::reference_wrapper {map}};

        const auto left_expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::MUL,
            .left=ket::ClonePtr {ket::Expression {theta}},
            .right=ket::ClonePtr {ket::Expression {ket::LiteralExpression {0.5}}}
        };

        const auto right_expr = ket::LiteralExpression {2.2};

        const auto expr = ket::BinaryExpression {
            .operation=ket::ParameterOperation::ADD,
            .left=ket::ClonePtr {ket::Expression{left_expr}},
            .right=ket::ClonePtr {ket::Expression{right_expr}}
        };

        const auto evaluator = ket::Evaluator {};
        REQUIRE_THAT(evaluator.evaluate(expr, map_variant), Catch::Matchers::WithinRel((1.5 * 0.5) + 2.2));
    }
}

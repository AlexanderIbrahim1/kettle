#include <cstddef>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/circuit/circuit.hpp>
#include <kettle/circuit/control_flow.hpp>
#include <kettle/circuit/control_flow_predicate.hpp>
#include <kettle/io/io_control_flow.hpp>


TEST_CASE("format_csv_integers_()")
{
    struct TestCase
    {
        std::vector<int> input;
        std::string expected;
    };

    const auto testcase = GENERATE(
        TestCase {{}, "[]"},
        TestCase {{0}, "[0]"},
        TestCase {{0, 3}, "[0, 3]"},
        TestCase {{0, 3, 1}, "[0, 3, 1]"}
    );

    REQUIRE(impl_ket::format_csv_integers_(testcase.input) == testcase.expected);
}

TEST_CASE("format_control_flow_predicate_()")
{
    using Predicate = ket::ControlFlowPredicate;
    using Kind = ket::ControlFlowBooleanKind;

    struct TestCase
    {
        Predicate input;
        std::string expected;
    };

    const auto testcase = GENERATE(
        TestCase {Predicate { {0}, {1}, Kind::IF}, "BITS[0] == [1]"},
        TestCase {Predicate { {0}, {1}, Kind::IF_NOT}, "BITS[0] != [1]"},
        TestCase {Predicate { {0, 3}, {1, 0}, Kind::IF}, "BITS[0, 3] == [1, 0]"},
        TestCase {Predicate { {0, 3}, {1, 0}, Kind::IF_NOT}, "BITS[0, 3] != [1, 0]"},
        TestCase {Predicate { {0, 3, 1}, {0, 0, 1}, Kind::IF}, "BITS[0, 3, 1] == [0, 0, 1]"}
    );

    REQUIRE(impl_ket::format_control_flow_predicate_(testcase.input) == testcase.expected);
}

TEST_CASE("format classical control flow statements")
{
    using Predicate = ket::ControlFlowPredicate;
    using Kind = ket::ControlFlowBooleanKind;

    const auto predicate = Predicate { {0, 3}, {1, 0}, Kind::IF};

    auto if_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_x_gate(0); return circ; }();
    auto else_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_h_gate(0); return circ; }();

    SECTION("if statement")
    {
        const auto if_stmt = impl_ket::ClassicalIfStatement {
            predicate,
            std::make_unique<ket::QuantumCircuit>(if_subcirc)
        };

        const auto expected = std::string {"IF BITS[0, 3] == [1, 0]"};
        const auto actual = impl_ket::format_classical_if_statement_header_(if_stmt);

        REQUIRE(expected == actual);
    }

    SECTION("if statement")
    {
        const auto if_else_stmt = impl_ket::ClassicalIfElseStatement {
            predicate,
            std::make_unique<ket::QuantumCircuit>(if_subcirc),
            std::make_unique<ket::QuantumCircuit>(else_subcirc)
        };
        const auto expected0 = std::string {"IF BITS[0, 3] == [1, 0]"};
        const auto expected1 = std::string {"ELSE"};
        const auto [actual0, actual1] = impl_ket::format_classical_if_else_statement_header_(if_else_stmt);

        REQUIRE(expected0 == actual0);
        REQUIRE(expected1 == actual1);
    }
}

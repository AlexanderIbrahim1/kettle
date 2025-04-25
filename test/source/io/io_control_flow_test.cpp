#include <cstddef>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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

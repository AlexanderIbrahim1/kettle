#include <cstddef>
#include <stdexcept>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <mini-qiskit/simulation/multithread_simulate_utils.hpp>

TEST_CASE("load_balanced_division()")
{
    SECTION("successful load balancing")
    {
        struct TestCase
        {
            std::size_t numerator;
            std::size_t denominator;
            std::vector<std::size_t> expected;
        };

        const auto testcase = GENERATE(
            TestCase {50, 7, {8, 7, 7, 7, 7, 7, 7}},
            TestCase {49, 7, {7, 7, 7, 7, 7, 7, 7}},
            TestCase {20, 3, {7, 7, 6}},
            TestCase {32, 8, {4, 4, 4, 4, 4, 4, 4, 4}}
        );

        const auto actual = impl_mqis::load_balanced_division(testcase.numerator, testcase.denominator);

        REQUIRE_THAT(actual, Catch::Matchers::UnorderedEquals(actual));
    }

    SECTION("throws when denominator is 0")
    {
        REQUIRE_THROWS_AS(impl_mqis::load_balanced_division(10, 0), std::runtime_error);
    }
}

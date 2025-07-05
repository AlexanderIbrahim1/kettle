#include <cstddef>
#include <stdexcept>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle_internal/simulation/multithread_simulate_utils.hpp"

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

        const auto actual = ket::internal::load_balanced_division_(testcase.numerator, testcase.denominator);

        REQUIRE_THAT(actual, Catch::Matchers::UnorderedEquals(actual));
    }

    SECTION("throws when denominator is 0")
    {
        REQUIRE_THROWS_AS(ket::internal::load_balanced_division_(10, 0), std::runtime_error);
    }
}

TEST_CASE("partial_sums_from_zero()")
{
    using Indices = std::vector<std::size_t>;

    struct TestCase
    {
        Indices values;
        Indices expected;
    };

    const auto testcase = GENERATE(
        TestCase { {}, {0} },
        TestCase { {1, 2, 3}, {0, 1, 3, 6} },
        TestCase { {2, 2, 2, 2}, {0, 2, 4, 6, 8} }
    );

    const auto actual = ket::internal::partial_sums_from_zero_(testcase.values);

    REQUIRE_THAT(actual, Catch::Matchers::Equals(testcase.expected));
}

TEST_CASE("partial_sum_pairs_()")
{
    struct TestCase
    {
        std::size_t n_gate_pairs;
        std::size_t n_threads;
        std::vector<ket::internal::FlatIndexPair<std::size_t>> expected;
    };

    const auto testcase = GENERATE(
        TestCase { 8, 1, {{0, 8}} },
        TestCase { 8, 2, {{0, 4}, {4, 8}} },
        TestCase { 16, 1, {{0, 16}} },
        TestCase { 16, 2, {{0, 8}, {8, 16}} },
        TestCase { 16, 4, {{0, 4}, {4, 8}, {8, 12}, {12, 16}} }
    );

    const auto actual = ket::internal::partial_sum_pairs_(testcase.n_gate_pairs, testcase.n_threads);

    REQUIRE_THAT(actual, Catch::Matchers::Equals(testcase.expected));
}

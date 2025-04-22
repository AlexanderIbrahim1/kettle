#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/common/mathtools.hpp"

TEST_CASE("flat_index_to_grid_indices_2d")
{
    struct TestCase
    {
        std::size_t i0;
        std::size_t i1;
        std::size_t size1;
    };

    const auto grid_indices_to_flat_index = [](const TestCase& tc) {
        return tc.i1 + tc.i0 * tc.size1;
    };

    const auto testcase = GENERATE(
        TestCase {0, 0, 10},
        TestCase {3, 5, 10},
        TestCase {3, 7, 12}
    );

    const auto flat_index = grid_indices_to_flat_index(testcase);
    const auto [i0_actual, i1_actual] = impl_ket::flat_index_to_grid_indices_2d(flat_index, testcase.size1);

    REQUIRE(i0_actual == testcase.i0);
    REQUIRE(i1_actual == testcase.i1);
}

TEST_CASE("flat_index_to_grid_indices_3d")
{
    struct TestCase
    {
        std::size_t i0;
        std::size_t i1;
        std::size_t i2;
        std::size_t size1;
        std::size_t size2;
    };

    const auto grid_indices_to_flat_index = [](const TestCase& tc) {
        return tc.i2 + tc.i1 * tc.size2 + tc.i0 * tc.size1 * tc.size2;
    };

    const auto testcase = GENERATE(
        TestCase {0, 0, 0, 10, 20},
        TestCase {3, 5, 7, 10, 20},
        TestCase {1, 5, 2, 7, 9}
    );

    const auto flat_index = grid_indices_to_flat_index(testcase);
    const auto [i0_actual, i1_actual, i2_actual] = impl_ket::flat_index_to_grid_indices_3d(flat_index, testcase.size1, testcase.size2);

    REQUIRE(i0_actual == testcase.i0);
    REQUIRE(i1_actual == testcase.i1);
    REQUIRE(i2_actual == testcase.i2);
}

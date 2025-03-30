#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/common/mathtools.hpp"

TEST_CASE("big endian and little endian flips")
{
    struct InputAndOutput
    {
        std::size_t input;
        std::size_t output;
    };

    SECTION("fip via two bits")
    {
        const auto n_bits = std::size_t {2};

        const auto pair =
            GENERATE(InputAndOutput {0, 0}, InputAndOutput {1, 2}, InputAndOutput {2, 1}, InputAndOutput {3, 3});

        const auto output = impl_mqis::endian_flip(pair.input, n_bits);
        REQUIRE(output == pair.output);
    }

    SECTION("fip via three bits")
    {
        const auto n_bits = std::size_t {3};

        const auto pair = GENERATE(
            InputAndOutput {0, 0},
            InputAndOutput {1, 4},
            InputAndOutput {2, 2},
            InputAndOutput {3, 6},
            InputAndOutput {4, 1},
            InputAndOutput {5, 5},
            InputAndOutput {6, 3},
            InputAndOutput {7, 7}
        );

        const auto output = impl_mqis::endian_flip(pair.input, n_bits);
        REQUIRE(output == pair.output);
    }

    SECTION("fip via four bits")
    {
        const auto n_bits = std::size_t {4};

        // NOTICE case {11, 13}
        // - this means we can't generate all non-identical pairs by looping over the first
        //   2^{n_bits - 1} inputs
        // - to generate all non-identical pairs, we have to loop over all values except
        //   for the first and last
        const auto pair = GENERATE(
            InputAndOutput {0, 0},
            InputAndOutput {1, 8},
            InputAndOutput {2, 4},
            InputAndOutput {3, 12},
            InputAndOutput {4, 2},
            InputAndOutput {5, 10},
            InputAndOutput {6, 6},
            InputAndOutput {7, 14},
            InputAndOutput {8, 1},
            InputAndOutput {9, 9},
            InputAndOutput {10, 5},
            InputAndOutput {11, 13},
            InputAndOutput {12, 3},
            InputAndOutput {13, 11},
            InputAndOutput {14, 7},
            InputAndOutput {15, 15}
        );

        const auto output = impl_mqis::endian_flip(pair.input, n_bits);
        REQUIRE(output == pair.output);
    }
}

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
    const auto [i0_actual, i1_actual] = impl_mqis::flat_index_to_grid_indices_2d(flat_index, testcase.size1);

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
    const auto [i0_actual, i1_actual, i2_actual] = impl_mqis::flat_index_to_grid_indices_3d(flat_index, testcase.size1, testcase.size2);

    REQUIRE(i0_actual == testcase.i0);
    REQUIRE(i1_actual == testcase.i1);
    REQUIRE(i2_actual == testcase.i2);
}

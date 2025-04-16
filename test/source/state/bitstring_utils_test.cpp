#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/state/bitstring_utils.hpp"

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

        const auto output = impl_mqis::endian_flip_(pair.input, n_bits);
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

        const auto output = impl_mqis::endian_flip_(pair.input, n_bits);
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

        const auto output = impl_mqis::endian_flip_(pair.input, n_bits);
        REQUIRE(output == pair.output);
    }
}

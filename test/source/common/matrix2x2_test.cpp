#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/matrix2x2.hpp"

TEST_CASE("conjugate transpose")
{
    SECTION("real matrix")
    {
        const auto mat = mqis::Matrix2X2 {
            {1.0, 0.0},
            {2.0, 0.0},
            {3.0, 0.0},
            {4.0, 0.0},
        };

        const auto expected = mqis::Matrix2X2 {
            {1.0, 0.0},
            {3.0, 0.0},
            {2.0, 0.0},
            {4.0, 0.0},
        };

        const auto actual = mqis::conjugate_transpose(mat);
        REQUIRE(mqis::almost_eq(actual, expected));
    }

    SECTION("complex matrix")
    {
        const auto mat = mqis::Matrix2X2 {
            {1.0, 2.0},
            {3.0, 4.0},
            {5.0, 6.0},
            {7.0, 8.0},
        };

        const auto expected = mqis::Matrix2X2 {
            {1.0, -2.0},
            {5.0, -6.0},
            {3.0, -4.0},
            {7.0, -8.0},
        };

        const auto actual = mqis::conjugate_transpose(mat);
        REQUIRE(mqis::almost_eq(actual, expected));
    }
}

TEST_CASE("square root of 2x2 matrix")
{
    SECTION("X gate")
    {
        const auto x_gate = mqis::Matrix2X2 {
            {0.0, 0.0},
            {1.0, 0.0},
            {1.0, 0.0},
            {0.0, 0.0}
        };

        const auto sqrt_x_gate = mqis::matrix_square_root(x_gate);
        const auto squared = sqrt_x_gate * sqrt_x_gate;

        REQUIRE(mqis::almost_eq(squared, x_gate));
    }
}

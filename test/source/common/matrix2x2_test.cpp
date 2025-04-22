#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/common/matrix2x2.hpp"

TEST_CASE("conjugate transpose")
{
    SECTION("real matrix")
    {
        const auto mat = ket::Matrix2X2 {
            {1.0, 0.0},
            {2.0, 0.0},
            {3.0, 0.0},
            {4.0, 0.0},
        };

        const auto expected = ket::Matrix2X2 {
            {1.0, 0.0},
            {3.0, 0.0},
            {2.0, 0.0},
            {4.0, 0.0},
        };

        const auto actual = ket::conjugate_transpose(mat);
        REQUIRE(ket::almost_eq(actual, expected));
    }

    SECTION("complex matrix")
    {
        const auto mat = ket::Matrix2X2 {
            {1.0, 2.0},
            {3.0, 4.0},
            {5.0, 6.0},
            {7.0, 8.0},
        };

        const auto expected = ket::Matrix2X2 {
            {1.0, -2.0},
            {5.0, -6.0},
            {3.0, -4.0},
            {7.0, -8.0},
        };

        const auto actual = ket::conjugate_transpose(mat);
        REQUIRE(ket::almost_eq(actual, expected));
    }
}

TEST_CASE("square root of 2x2 matrix")
{
    SECTION("X gate")
    {
        const auto x_gate = ket::Matrix2X2 {
            {0.0, 0.0},
            {1.0, 0.0},
            {1.0, 0.0},
            {0.0, 0.0}
        };

        const auto sx_gate = ket::matrix_square_root(x_gate);
        const auto squared = sx_gate * sx_gate;

        REQUIRE(ket::almost_eq(squared, x_gate));
    }
}

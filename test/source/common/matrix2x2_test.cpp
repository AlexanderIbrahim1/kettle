#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/common/matrix2x2.hpp"

TEST_CASE("conjugate transpose")
{
    SECTION("real matrix")
    {
        const auto mat = ket::Matrix2X2 {
            .elem00={1.0, 0.0},
            .elem01={2.0, 0.0},
            .elem10={3.0, 0.0},
            .elem11={4.0, 0.0},
        };

        const auto expected = ket::Matrix2X2 {
            .elem00={1.0, 0.0},
            .elem01={3.0, 0.0},
            .elem10={2.0, 0.0},
            .elem11={4.0, 0.0},
        };

        const auto actual = ket::conjugate_transpose(mat);
        REQUIRE(ket::almost_eq(actual, expected));
    }

    SECTION("complex matrix")
    {
        const auto mat = ket::Matrix2X2 {
            .elem00={1.0, 2.0},
            .elem01={3.0, 4.0},
            .elem10={5.0, 6.0},
            .elem11={7.0, 8.0},
        };

        const auto expected = ket::Matrix2X2 {
            .elem00={1.0, -2.0},
            .elem01={5.0, -6.0},
            .elem10={3.0, -4.0},
            .elem11={7.0, -8.0},
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
            .elem00={0.0, 0.0},
            .elem01={1.0, 0.0},
            .elem10={1.0, 0.0},
            .elem11={0.0, 0.0}
        };

        const auto sx_gate = ket::matrix_square_root(x_gate);
        const auto squared = sx_gate * sx_gate;

        REQUIRE(ket::almost_eq(squared, x_gate));
    }
}

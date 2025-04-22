#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/common/mathtools.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/random_u_gates.hpp"

TEST_CASE("generate_random_unitary2x2()")
{
    const auto unitary = ket::generate_random_unitary2x2();
    const auto identity = ket::Matrix2X2 {1.0, 0.0, 0.0, 1.0};

    REQUIRE(ket::almost_eq(unitary * ket::conjugate_transpose(unitary), identity));
}

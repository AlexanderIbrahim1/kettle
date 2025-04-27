#include <catch2/catch_test_macros.hpp>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/random_u_gates.hpp"

TEST_CASE("generate_random_unitary2x2()")
{
    const auto unitary = ket::generate_random_unitary2x2();
    const auto identity = ket::Matrix2X2 {.elem00=1.0, .elem01=0.0, .elem10=0.0, .elem11=1.0};

    REQUIRE(ket::almost_eq(unitary * ket::conjugate_transpose(unitary), identity));
}

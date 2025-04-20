#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/gates/random_u_gates.hpp"

TEST_CASE("generate_random_unitary2x2()")
{
    const auto unitary = mqis::generate_random_unitary2x2();
    const auto identity = mqis::Matrix2X2 {1.0, 0.0, 0.0, 1.0};

    REQUIRE(mqis::almost_eq(unitary * mqis::conjugate_transpose(unitary), identity));
}

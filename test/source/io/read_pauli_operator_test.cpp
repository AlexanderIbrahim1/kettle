#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "kettle/operator/pauli/pauli_operator.hpp"
#include "kettle/io/read_pauli_operator.hpp"

using PT = ket::PauliTerm;

TEST_CASE("read_pauli_operator()")
{
    auto sstream = std::stringstream {
        " 1.100000000000e+01    0.000000000000e+00   :                                                         \n"
        "-1.875000000000e-01    0.000000000000e+00   :   (0, X)   (1, X)   (2, X)   (3, X)   (4, X)   (5, X)   \n"
        " 1.875000000000e-01    0.000000000000e+00   :   (0, X)   (1, X)   (2, X)   (3, Z)                     \n"
    };

    const auto pauli_op = ket::read_pauli_operator(sstream, 6);

    const auto expected = ket::PauliOperator {
        {.coefficient={11.0000, 0.0}, .pauli_string={PT::I, PT::I, PT::I, PT::I, PT::I, PT::I}},
        {.coefficient={-0.1875, 0.0}, .pauli_string={PT::X, PT::X, PT::X, PT::X, PT::X, PT::X}},
        {.coefficient={ 0.1875, 0.0}, .pauli_string={PT::X, PT::X, PT::X, PT::Z, PT::I, PT::I}},
    };

    REQUIRE(ket::almost_eq(pauli_op, expected));
}

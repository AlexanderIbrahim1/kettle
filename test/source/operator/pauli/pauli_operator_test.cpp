#include <cmath>
#include <complex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/common/mathtools.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/operator/pauli/pauli_operator.hpp"
#include "kettle/state/state.hpp"


using PT = ket::PauliTerm;


TEST_CASE("expectation value of SparsePauliString, 1-qubit")
{
    struct TestCase
    {
        PT term;
        ket::QuantumState statevector;
        std::complex<double> expected;
    };

    const auto x_plus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}};
    const auto x_minus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {-M_SQRT1_2, 0.0}}};
    const auto y_plus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {0.0, M_SQRT1_2}}};
    const auto y_minus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {0.0, -M_SQRT1_2}}};
    const auto z_plus = ket::QuantumState {"0"};
    const auto z_minus = ket::QuantumState {"1"};

    auto testcase = GENERATE_COPY(
        TestCase(PT::X, x_plus, 1.0),
        TestCase(PT::X, x_minus, -1.0),
        TestCase(PT::X, y_plus, 0.0),
        TestCase(PT::X, y_minus, 0.0),
        TestCase(PT::X, z_plus, 0.0),
        TestCase(PT::X, z_minus, 0.0),
        TestCase(PT::Y, x_plus, 00),
        TestCase(PT::Y, x_minus, 0.0),
        TestCase(PT::Y, y_plus, 1.0),
        TestCase(PT::Y, y_minus, -1.0),
        TestCase(PT::Y, z_plus, 0.0),
        TestCase(PT::Y, z_minus, 0.0),
        TestCase(PT::Z, x_plus, 0.0),
        TestCase(PT::Z, x_minus, 0.0),
        TestCase(PT::Z, y_plus, 0.0),
        TestCase(PT::Z, y_minus, 0.0),
        TestCase(PT::Z, z_plus, 1.0),
        TestCase(PT::Z, z_minus, -1.0)
    );

    auto pauli_string = ket::SparsePauliString {1};
    pauli_string.add(0, testcase.term);

    const auto expval = ket::expectation_value(pauli_string, testcase.statevector);

    REQUIRE(ket::almost_eq(expval, testcase.expected));
}


TEST_CASE("expectation value of SparsePauliString, 2-qubit")
{
    struct TestCase
    {
        PT term0;
        PT term1;
        ket::QuantumState statevector;
        std::complex<double> expected;
    };

    const auto x_plus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}};
    const auto x_minus = ket::QuantumState {{{M_SQRT1_2, 0.0}, {-M_SQRT1_2, 0.0}}};
    const auto z_plus = ket::QuantumState {"0"};
    const auto z_minus = ket::QuantumState {"1"};

    auto testcase = GENERATE_COPY(
        TestCase(PT::Z, PT::Z, ket::tensor_product(z_plus, z_plus), 1.0),
        TestCase(PT::Z, PT::Z, ket::tensor_product(z_plus, z_minus), -1.0),
        TestCase(PT::Z, PT::Z, ket::tensor_product(z_minus, z_plus), -1.0),
        TestCase(PT::Z, PT::Z, ket::tensor_product(z_minus, z_minus), 1.0),
        TestCase(PT::X, PT::X, ket::tensor_product(z_plus, z_plus), 0.0),
        TestCase(PT::X, PT::X, ket::tensor_product(z_plus, z_minus), 0.0),
        TestCase(PT::X, PT::X, ket::tensor_product(z_minus, z_plus), 0.0),
        TestCase(PT::X, PT::X, ket::tensor_product(z_minus, z_minus), 0.0),
        TestCase(PT::X, PT::Z, ket::tensor_product(x_plus, z_plus), 1.0),
        TestCase(PT::X, PT::Z, ket::tensor_product(x_plus, z_minus), -1.0),
        TestCase(PT::X, PT::X, ket::tensor_product(x_plus, z_plus), 0.0),
        TestCase(PT::X, PT::X, ket::tensor_product(x_plus, x_minus), -1.0)
    );

    auto pauli_string = ket::SparsePauliString {2};
    pauli_string.add(0, testcase.term0);
    pauli_string.add(1, testcase.term1);

    const auto expval = ket::expectation_value(pauli_string, testcase.statevector);

    REQUIRE(ket::almost_eq(expval, testcase.expected));
}

// make it easier to construct SparsePauliString first
// TEST_CASE("expectation value of PauliOperator")
// {
//     SECTION("Z + X")
//     {
//         auto pauli_op = ket::PauliOperator {1};
//     }
// }

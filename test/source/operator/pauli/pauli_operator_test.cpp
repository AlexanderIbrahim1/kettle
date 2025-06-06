#include <cmath>
#include <complex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/operator/pauli/pauli_operator.hpp"
#include "kettle/simulation/simulate.hpp"
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

TEST_CASE("expectation value of PauliOperator")
{
    SECTION("<0|(Z + X)|0>")
    {
        auto pauli_op = ket::PauliOperator {
            {.coefficient={1.0, 0.0}, .pauli_string={PT::Z}},
            {.coefficient={1.0, 0.0}, .pauli_string={PT::X}},
        };

        const auto statevector = ket::QuantumState {"0"};

        const auto expval = ket::expectation_value(pauli_op, statevector);

        REQUIRE(ket::almost_eq(expval, {1.0, 0.0}));
    }

    SECTION("<+|(Z - 2 * X)|+>")
    {
        auto pauli_op = ket::PauliOperator {
            {.coefficient={1.0, 0.0}, .pauli_string={PT::Z}},
            {.coefficient={-2.0, 0.0}, .pauli_string={PT::X}},
        };

        const auto statevector = ket::QuantumState {{{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}};

        const auto expval = ket::expectation_value(pauli_op, statevector);

        REQUIRE(ket::almost_eq(expval, {-2.0, 0.0}));
    }

    SECTION("<BELL[0+]|(ZZ + XX)|BELL[0+]>")
    {
        auto pauli_op = ket::PauliOperator {
            {.coefficient={1.0, 0.0}, .pauli_string={PT::Z, PT::Z}},
            {.coefficient={1.0, 0.0}, .pauli_string={PT::X, PT::X}},
        };

        const auto bell_state = []() {
            auto circuit = ket::QuantumCircuit {2};
            circuit.add_h_gate(0);
            circuit.add_cx_gate(0, 1);

            auto state = ket::QuantumState {"00"};
            ket::simulate(circuit, state);

            return state;
        }();

        const auto expval = ket::expectation_value(pauli_op, bell_state);

        REQUIRE(ket::almost_eq(expval, {2.0, 0.0}));
    }

    SECTION("<GHZ|(ZZZ + XXX)|GHZ>")
    {
        auto pauli_op = ket::PauliOperator {
            {.coefficient={1.0, 0.0}, .pauli_string={PT::Z, PT::Z, PT::Z}},
            {.coefficient={1.0, 0.0}, .pauli_string={PT::X, PT::X, PT::X}},
        };

        const auto ghz_state = []() {
            auto circuit = ket::QuantumCircuit {3};
            circuit.add_h_gate(0);
            circuit.add_cx_gate(0, 1);
            circuit.add_cx_gate(0, 2);

            auto state = ket::QuantumState {"000"};
            ket::simulate(circuit, state);

            return state;
        }();

        const auto expval = ket::expectation_value(pauli_op, ghz_state);

        REQUIRE(ket::almost_eq(expval, {1.0, 0.0}));
    }

    SECTION("<++|(XI - iIX)|++>")
    {
        auto pauli_op = ket::PauliOperator {
            {.coefficient={1.0, 0.0}, .pauli_string={PT::X, PT::I}},
            {.coefficient={0.0, -1.0}, .pauli_string={PT::I, PT::X}},
        };

        const auto plus_plus_state = []() {
            auto circuit = ket::QuantumCircuit {2};
            circuit.add_h_gate({0, 1});

            auto state = ket::QuantumState {"00"};
            ket::simulate(circuit, state);

            return state;
        }();

        const auto expval = ket::expectation_value(pauli_op, plus_plus_state);

        REQUIRE(ket::almost_eq(expval, {1.0, -1.0}));
    }
}

TEST_CASE("PauliOperator comparison")
{
    SECTION("equal")
    {
        const auto pauli_op0 = ket::PauliOperator {
            {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
            {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
        };

        const auto pauli_op1 = ket::PauliOperator {
            {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
            {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
        };

        REQUIRE(ket::almost_eq(pauli_op0, pauli_op1));
    }

    SECTION("not equal")
    {
        SECTION("different coefficients")
        {
            const auto pauli_op0 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
                {.coefficient={3.0, 4.0}, .pauli_string={PT::I, PT::X}},
            };

            const auto pauli_op1 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
            };

            REQUIRE(!ket::almost_eq(pauli_op0, pauli_op1));
        }

        SECTION("different terms")
        {
            const auto pauli_op0 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
                {.coefficient={1.0, 2.0}, .pauli_string={PT::Y, PT::Z}},
            };

            const auto pauli_op1 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
            };

            REQUIRE(!ket::almost_eq(pauli_op0, pauli_op1));
        }

        SECTION("different number of terms")
        {
            const auto pauli_op0 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
                {.coefficient={1.0, 2.0}, .pauli_string={PT::Y, PT::Z}},
            };

            const auto pauli_op1 = ket::PauliOperator {
                {.coefficient={1.0, 2.0}, .pauli_string={PT::I, PT::X}},
            };

            REQUIRE(!ket::almost_eq(pauli_op0, pauli_op1));
        }
    }
}

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/state.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/gates/fourier.hpp"


TEST_CASE("Forward QFT on |0> state")
{
    SECTION("1 qubit")
    {
        auto state = mqis::QuantumState {"0"};
        auto circuit = mqis::QuantumCircuit {1};

        mqis::apply_forward_fourier_transform(circuit, {0});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(2.0);
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }

    SECTION("2 qubits")
    {
        auto state = mqis::QuantumState {"00"};
        auto circuit = mqis::QuantumCircuit {2};

        mqis::apply_forward_fourier_transform(circuit, {0, 1});

        // clang-format off
        const auto norm = 1.0 / 2.0;
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }

    SECTION("3 qubits")
    {
        auto state = mqis::QuantumState {"000"};
        auto circuit = mqis::QuantumCircuit {3};

        mqis::apply_forward_fourier_transform(circuit, {0, 1, 2});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(8.0);
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }
}

TEST_CASE("basic Forward QFT")
{
    SECTION("2 qubits")
    {
        // |10> -> (1/2) [|00> + |10> - |01> - |11>]
        auto state = mqis::QuantumState {"10"};
        auto circuit = mqis::QuantumCircuit {2};

        mqis::apply_forward_fourier_transform(circuit, {0, 1});

        // clang-format off
        const auto norm = 1.0 / 2.0;
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0},
                {-norm, 0.0},
                {-norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }
}

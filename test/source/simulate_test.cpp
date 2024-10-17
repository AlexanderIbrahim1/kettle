#include <cmath>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/complex.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

TEST_CASE("simulate X gate")
{
    SECTION("one qubit, X(0)")
    {
        // expectation
        //
        // |0> -> |1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)")
    {
        // expectation
        //
        // |00> -> |01>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)X(0)")
    {
        // expectation
        //
        // |00> -> |10> -> |11>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);
        circuit.add_x_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate H gate")
{
    SECTION("one qubit, H(0)")
    {
        // expectation
        //
        // |0> -> (1/sqrt2)|0> + (1/sqrt2)|1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2)|00> + (1/sqrt2)|10>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |01>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        //      -> (1/2) |00> + (1/2) |01> + (1/2) |10> + (1/2) |11>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/gates/swap.hpp"

TEST_CASE("control swap gate on 3-qubit circuit")
{
    struct TestInfo
    {
        std::string input_bitstring;
        std::string expected_bitstring;
    };

    SECTION("control qubit is 0")
    {
        // clang-format off
        const auto info = GENERATE(
            TestInfo {"000", "000"},
            TestInfo {"100", "100"},
            TestInfo {"010", "010"},
            TestInfo {"110", "101"},
            TestInfo {"001", "001"},
            TestInfo {"101", "110"},
            TestInfo {"011", "011"},
            TestInfo {"111", "111"}
        );
        // clang-format on

        auto state = mqis::QuantumState {info.input_bitstring};
        auto expected = mqis::QuantumState {info.expected_bitstring};
        auto circuit = mqis::QuantumCircuit {3};

        SECTION("swap qubits are 1 and 2")
        {
            mqis::apply_control_swap(circuit, 0, 1, 2);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }

        SECTION("swap qubits are 2 and 1")
        {
            mqis::apply_control_swap(circuit, 0, 2, 1);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }
    }

    SECTION("control qubit is 1")
    {
        // clang-format off
        const auto info = GENERATE(
            TestInfo {"000", "000"},
            TestInfo {"100", "100"},
            TestInfo {"010", "010"},
            TestInfo {"110", "011"},  // changes
            TestInfo {"001", "001"},
            TestInfo {"101", "101"},
            TestInfo {"011", "110"},  // changes
            TestInfo {"111", "111"}
        );
        // clang-format on

        auto state = mqis::QuantumState {info.input_bitstring};
        auto expected = mqis::QuantumState {info.expected_bitstring};
        auto circuit = mqis::QuantumCircuit {3};

        SECTION("swap qubits are 0 and 2")
        {
            mqis::apply_control_swap(circuit, 1, 0, 2);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }

        SECTION("swap qubits are 2 and 0")
        {
            mqis::apply_control_swap(circuit, 1, 2, 0);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }
    }

    SECTION("control qubit is 2")
    {
        // clang-format off
        const auto info = GENERATE(
            TestInfo {"000", "000"},
            TestInfo {"100", "100"},
            TestInfo {"010", "010"},
            TestInfo {"110", "110"},
            TestInfo {"001", "001"},
            TestInfo {"101", "011"},  // changes
            TestInfo {"011", "101"},  // changes
            TestInfo {"111", "111"}
        );
        // clang-format on

        auto state = mqis::QuantumState {info.input_bitstring};
        auto expected = mqis::QuantumState {info.expected_bitstring};
        auto circuit = mqis::QuantumCircuit {3};

        SECTION("swap qubits are 0 and 1")
        {
            mqis::apply_control_swap(circuit, 2, 0, 1);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }

        SECTION("swap qubits are 1 and 0")
        {
            mqis::apply_control_swap(circuit, 2, 1, 0);
            mqis::simulate(circuit, state);
            REQUIRE(mqis::almost_eq(state, expected));
        }
    }
}

TEST_CASE("control swap gate on 4-qubit circuit")
{
    struct TestInfo
    {
        std::string input_bitstring;
        std::string expected_bitstring;
    };

    // clang-format off
    const auto info = GENERATE(
        TestInfo {"0000", "0000"},
        TestInfo {"1000", "1000"},
        TestInfo {"0100", "0100"},
        TestInfo {"1100", "1001"},
        TestInfo {"0010", "0010"},
        TestInfo {"1010", "1010"},
        TestInfo {"0110", "0110"},
        TestInfo {"1110", "1011"},
        TestInfo {"0001", "0001"},
        TestInfo {"1001", "1100"},
        TestInfo {"0101", "0101"},
        TestInfo {"1101", "1101"},
        TestInfo {"0011", "0011"},
        TestInfo {"1011", "1110"},
        TestInfo {"0111", "0111"},
        TestInfo {"1111", "1111"}
    );
    // clang-format on
 
    auto state = mqis::QuantumState {info.input_bitstring};
    auto expected = mqis::QuantumState {info.expected_bitstring};
    auto circuit = mqis::QuantumCircuit {4};
 
    mqis::apply_control_swap(circuit, 0, 1, 3);
    mqis::simulate(circuit, state);
    REQUIRE(mqis::almost_eq(state, expected));
}

TEST_CASE("control swap gate throws exceptions on invalid inputs")
{
    auto circuit = mqis::QuantumCircuit {3};

    SECTION("swap qubits are identical")
    {
        REQUIRE_THROWS_AS(mqis::apply_control_swap(circuit, 0, 1, 1), std::runtime_error);
    }

    SECTION("control qubit matches a swap qubit")
    {
        SECTION("matches first swap qubit") {
            REQUIRE_THROWS_AS(mqis::apply_control_swap(circuit, 0, 0, 1), std::runtime_error);
        }

        SECTION("matches second swap qubit") {
            REQUIRE_THROWS_AS(mqis::apply_control_swap(circuit, 0, 1, 0), std::runtime_error);
        }
    }
}

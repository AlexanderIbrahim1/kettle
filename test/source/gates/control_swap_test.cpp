#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/gates/control_swap.hpp"
#include "mini-qiskit/common/print.hpp"

TEST_CASE("control swap gate on 3-qubit circuit")
{
    struct TestInfo
    {
        std::string input_bitstring;
        std::string expected_bitstring;
    };

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

    auto circuit = mqis::QuantumCircuit {3};
    mqis::apply_control_swap(circuit, 0, 1, 2);

    mqis::simulate(circuit, state);

    auto expected = mqis::QuantumState {info.expected_bitstring};

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

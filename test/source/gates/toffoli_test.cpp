#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/gates/toffoli.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

TEST_CASE("toffoli gate with 3 qubits")
{
    struct TestInfo
    {
        std::string initial_bitstring;
        std::pair<std::size_t, std::size_t> control_qubits;
        std::size_t target_qubit;
        mqis::QuantumState expected;
    };

    const auto info = GENERATE(
        TestInfo {
            "000", {0, 1},
             2, mqis::QuantumState {"000"}
    },
        TestInfo {"100", {0, 1}, 2, mqis::QuantumState {"100"}},
        TestInfo {"010", {0, 1}, 2, mqis::QuantumState {"010"}},
        TestInfo {"110", {0, 1}, 2, mqis::QuantumState {"111"}},
        TestInfo {"001", {0, 1}, 2, mqis::QuantumState {"001"}},
        TestInfo {"101", {0, 1}, 2, mqis::QuantumState {"101"}},
        TestInfo {"011", {0, 1}, 2, mqis::QuantumState {"011"}},
        TestInfo {"111", {0, 1}, 2, mqis::QuantumState {"110"}},
        TestInfo {"000", {0, 2}, 1, mqis::QuantumState {"000"}},
        TestInfo {"100", {0, 2}, 1, mqis::QuantumState {"100"}},
        TestInfo {"010", {0, 2}, 1, mqis::QuantumState {"010"}},
        TestInfo {"110", {0, 2}, 1, mqis::QuantumState {"110"}},
        TestInfo {"001", {0, 2}, 1, mqis::QuantumState {"001"}},
        TestInfo {"101", {0, 2}, 1, mqis::QuantumState {"111"}},
        TestInfo {"011", {0, 2}, 1, mqis::QuantumState {"011"}},
        TestInfo {"111", {0, 2}, 1, mqis::QuantumState {"101"}},
        TestInfo {"000", {1, 2}, 0, mqis::QuantumState {"000"}},
        TestInfo {"100", {1, 2}, 0, mqis::QuantumState {"100"}},
        TestInfo {"010", {1, 2}, 0, mqis::QuantumState {"010"}},
        TestInfo {"110", {1, 2}, 0, mqis::QuantumState {"110"}},
        TestInfo {"001", {1, 2}, 0, mqis::QuantumState {"001"}},
        TestInfo {"101", {1, 2}, 0, mqis::QuantumState {"101"}},
        TestInfo {"011", {1, 2}, 0, mqis::QuantumState {"111"}},
        TestInfo {"111", {1, 2}, 0, mqis::QuantumState {"011"}}
    );

    auto state = mqis::QuantumState {info.initial_bitstring};
    auto circuit = mqis::QuantumCircuit {3};
    mqis::apply_toffoli_gate(circuit, info.control_qubits, info.target_qubit);

    mqis::simulate(circuit, state);

    REQUIRE(mqis::almost_eq(state, info.expected));
}

TEST_CASE("toffoli gate with 4 qubits")
{
    struct TestInfo
    {
        std::string initial_bitstring;
        std::pair<std::size_t, std::size_t> control_qubits;
        std::size_t target_qubit;
        mqis::QuantumState expected;
    };

    const auto info = GENERATE(
        TestInfo {
            "0000", {0, 1},
             2, mqis::QuantumState {"0000"}
    },
        TestInfo {"1000", {0, 1}, 2, mqis::QuantumState {"1000"}},
        TestInfo {"0100", {0, 1}, 2, mqis::QuantumState {"0100"}},
        TestInfo {"1100", {0, 1}, 2, mqis::QuantumState {"1110"}},
        TestInfo {"0010", {0, 1}, 2, mqis::QuantumState {"0010"}},
        TestInfo {"1010", {0, 1}, 2, mqis::QuantumState {"1010"}},
        TestInfo {"0110", {0, 1}, 2, mqis::QuantumState {"0110"}},
        TestInfo {"1110", {0, 1}, 2, mqis::QuantumState {"1100"}},
        TestInfo {"0001", {0, 1}, 2, mqis::QuantumState {"0001"}},
        TestInfo {"1001", {0, 1}, 2, mqis::QuantumState {"1001"}},
        TestInfo {"0101", {0, 1}, 2, mqis::QuantumState {"0101"}},
        TestInfo {"1101", {0, 1}, 2, mqis::QuantumState {"1111"}},
        TestInfo {"0011", {0, 1}, 2, mqis::QuantumState {"0011"}},
        TestInfo {"1011", {0, 1}, 2, mqis::QuantumState {"1011"}},
        TestInfo {"0111", {0, 1}, 2, mqis::QuantumState {"0111"}},
        TestInfo {"1111", {0, 1}, 2, mqis::QuantumState {"1101"}},
        TestInfo {"0000", {0, 1}, 3, mqis::QuantumState {"0000"}},
        TestInfo {"1000", {0, 1}, 3, mqis::QuantumState {"1000"}},
        TestInfo {"0100", {0, 1}, 3, mqis::QuantumState {"0100"}},
        TestInfo {"1100", {0, 1}, 3, mqis::QuantumState {"1101"}},
        TestInfo {"0010", {0, 1}, 3, mqis::QuantumState {"0010"}},
        TestInfo {"1010", {0, 1}, 3, mqis::QuantumState {"1010"}},
        TestInfo {"0110", {0, 1}, 3, mqis::QuantumState {"0110"}},
        TestInfo {"1110", {0, 1}, 3, mqis::QuantumState {"1111"}},
        TestInfo {"0001", {0, 1}, 3, mqis::QuantumState {"0001"}},
        TestInfo {"1001", {0, 1}, 3, mqis::QuantumState {"1001"}},
        TestInfo {"0101", {0, 1}, 3, mqis::QuantumState {"0101"}},
        TestInfo {"1101", {0, 1}, 3, mqis::QuantumState {"1100"}},
        TestInfo {"0011", {0, 1}, 3, mqis::QuantumState {"0011"}},
        TestInfo {"1011", {0, 1}, 3, mqis::QuantumState {"1011"}},
        TestInfo {"0111", {0, 1}, 3, mqis::QuantumState {"0111"}},
        TestInfo {"1111", {0, 1}, 3, mqis::QuantumState {"1110"}}
    );

    auto state = mqis::QuantumState {info.initial_bitstring};
    auto circuit = mqis::QuantumCircuit {4};
    mqis::apply_toffoli_gate(circuit, info.control_qubits, info.target_qubit);

    mqis::simulate(circuit, state);

    REQUIRE(mqis::almost_eq(state, info.expected));
}

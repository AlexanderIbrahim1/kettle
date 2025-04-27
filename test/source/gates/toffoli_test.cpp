#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/toffoli.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"

TEST_CASE("toffoli gate with 3 qubits")
{
    struct TestInfo
    {
        std::string initial_bitstring;
        std::pair<std::size_t, std::size_t> control_qubits;
        std::size_t target_qubit;
        ket::QuantumState expected;
    };

    // clang-format off
    const auto info = GENERATE(
        TestInfo {"000", {0, 1}, 2, ket::QuantumState {"000"}},
        TestInfo {"100", {0, 1}, 2, ket::QuantumState {"100"}},
        TestInfo {"010", {0, 1}, 2, ket::QuantumState {"010"}},
        TestInfo {"110", {0, 1}, 2, ket::QuantumState {"111"}},
        TestInfo {"001", {0, 1}, 2, ket::QuantumState {"001"}},
        TestInfo {"101", {0, 1}, 2, ket::QuantumState {"101"}},
        TestInfo {"011", {0, 1}, 2, ket::QuantumState {"011"}},
        TestInfo {"111", {0, 1}, 2, ket::QuantumState {"110"}},
        TestInfo {"000", {0, 2}, 1, ket::QuantumState {"000"}},
        TestInfo {"100", {0, 2}, 1, ket::QuantumState {"100"}},
        TestInfo {"010", {0, 2}, 1, ket::QuantumState {"010"}},
        TestInfo {"110", {0, 2}, 1, ket::QuantumState {"110"}},
        TestInfo {"001", {0, 2}, 1, ket::QuantumState {"001"}},
        TestInfo {"101", {0, 2}, 1, ket::QuantumState {"111"}},
        TestInfo {"011", {0, 2}, 1, ket::QuantumState {"011"}},
        TestInfo {"111", {0, 2}, 1, ket::QuantumState {"101"}},
        TestInfo {"000", {1, 2}, 0, ket::QuantumState {"000"}},
        TestInfo {"100", {1, 2}, 0, ket::QuantumState {"100"}},
        TestInfo {"010", {1, 2}, 0, ket::QuantumState {"010"}},
        TestInfo {"110", {1, 2}, 0, ket::QuantumState {"110"}},
        TestInfo {"001", {1, 2}, 0, ket::QuantumState {"001"}},
        TestInfo {"101", {1, 2}, 0, ket::QuantumState {"101"}},
        TestInfo {"011", {1, 2}, 0, ket::QuantumState {"111"}},
        TestInfo {"111", {1, 2}, 0, ket::QuantumState {"011"}}
    );
    // clang-format on

    auto state = ket::QuantumState {info.initial_bitstring};
    auto circuit = ket::QuantumCircuit {3};
    ket::apply_toffoli_gate(circuit, info.control_qubits, info.target_qubit);

    ket::simulate(circuit, state);

    REQUIRE(ket::almost_eq(state, info.expected));
}

TEST_CASE("toffoli gate with 4 qubits")
{
    struct TestInfo
    {
        std::string initial_bitstring;
        std::pair<std::size_t, std::size_t> control_qubits;
        std::size_t target_qubit;
        ket::QuantumState expected;
    };

    // clang-format off
    const auto info = GENERATE(
        TestInfo {"0000", {0, 1}, 2, ket::QuantumState {"0000"}},
        TestInfo {"1000", {0, 1}, 2, ket::QuantumState {"1000"}},
        TestInfo {"0100", {0, 1}, 2, ket::QuantumState {"0100"}},
        TestInfo {"1100", {0, 1}, 2, ket::QuantumState {"1110"}},
        TestInfo {"0010", {0, 1}, 2, ket::QuantumState {"0010"}},
        TestInfo {"1010", {0, 1}, 2, ket::QuantumState {"1010"}},
        TestInfo {"0110", {0, 1}, 2, ket::QuantumState {"0110"}},
        TestInfo {"1110", {0, 1}, 2, ket::QuantumState {"1100"}},
        TestInfo {"0001", {0, 1}, 2, ket::QuantumState {"0001"}},
        TestInfo {"1001", {0, 1}, 2, ket::QuantumState {"1001"}},
        TestInfo {"0101", {0, 1}, 2, ket::QuantumState {"0101"}},
        TestInfo {"1101", {0, 1}, 2, ket::QuantumState {"1111"}},
        TestInfo {"0011", {0, 1}, 2, ket::QuantumState {"0011"}},
        TestInfo {"1011", {0, 1}, 2, ket::QuantumState {"1011"}},
        TestInfo {"0111", {0, 1}, 2, ket::QuantumState {"0111"}},
        TestInfo {"1111", {0, 1}, 2, ket::QuantumState {"1101"}},
        TestInfo {"0000", {0, 1}, 3, ket::QuantumState {"0000"}},
        TestInfo {"1000", {0, 1}, 3, ket::QuantumState {"1000"}},
        TestInfo {"0100", {0, 1}, 3, ket::QuantumState {"0100"}},
        TestInfo {"1100", {0, 1}, 3, ket::QuantumState {"1101"}},
        TestInfo {"0010", {0, 1}, 3, ket::QuantumState {"0010"}},
        TestInfo {"1010", {0, 1}, 3, ket::QuantumState {"1010"}},
        TestInfo {"0110", {0, 1}, 3, ket::QuantumState {"0110"}},
        TestInfo {"1110", {0, 1}, 3, ket::QuantumState {"1111"}},
        TestInfo {"0001", {0, 1}, 3, ket::QuantumState {"0001"}},
        TestInfo {"1001", {0, 1}, 3, ket::QuantumState {"1001"}},
        TestInfo {"0101", {0, 1}, 3, ket::QuantumState {"0101"}},
        TestInfo {"1101", {0, 1}, 3, ket::QuantumState {"1100"}},
        TestInfo {"0011", {0, 1}, 3, ket::QuantumState {"0011"}},
        TestInfo {"1011", {0, 1}, 3, ket::QuantumState {"1011"}},
        TestInfo {"0111", {0, 1}, 3, ket::QuantumState {"0111"}},
        TestInfo {"1111", {0, 1}, 3, ket::QuantumState {"1110"}}
    );
    // clang-format on

    auto state = ket::QuantumState {info.initial_bitstring};
    auto circuit = ket::QuantumCircuit {4};
    ket::apply_toffoli_gate(circuit, info.control_qubits, info.target_qubit);

    ket::simulate(circuit, state);

    REQUIRE(ket::almost_eq(state, info.expected));
}

TEST_CASE("apply_toffoli_gate() and apply_doubly_controlled_gate() match")
{
    const std::string init_bitstring = GENERATE("000", "100", "010", "110", "001", "101", "011", "111");

    auto circuit0 = ket::QuantumCircuit {3};
    ket::apply_toffoli_gate(circuit0, {0, 1}, 2);

    auto circuit1 = ket::QuantumCircuit {3};
    ket::apply_doubly_controlled_gate(circuit0, ket::x_gate(), {0, 1}, 2);

    auto state0 = ket::QuantumState {init_bitstring};
    auto state1 = ket::QuantumState {init_bitstring};

    ket::simulate(circuit0, state0);
    ket::simulate(circuit1, state1);

    REQUIRE(ket::almost_eq(state0, state1));
}

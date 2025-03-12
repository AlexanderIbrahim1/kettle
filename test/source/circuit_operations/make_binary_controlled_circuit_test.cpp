#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp"

TEST_CASE("make_binary_controlled_circuit()")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    const auto angle = M_PI_4;
    const auto t_gate = mqis::p_gate(angle);

    // create the circuit manually, by repeating the gates
    auto manual_circuit = mqis::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{angle, 0, 3}});
    manual_circuit.add_cp_gate({{angle, 1, 3}, {angle, 1, 3}});
    manual_circuit.add_cp_gate({{angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuit = mqis::QuantumCircuit {1};
    subcircuit.add_u_gate(t_gate, 0);
    auto binary_made_circuit = mqis::make_binary_controlled_circuit_naive(subcircuit, 4, {0, 1, 2}, {3});

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(manual_circuit, state0);
    mqis::simulate(binary_made_circuit, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

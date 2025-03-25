#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/circuit_operations/build_decomposed_circuit.hpp"
#include "mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp"
#include "mini-qiskit/decomposed/read_decomposition_file.hpp"

#include "../test_utils/powers_of_diagonal_unitary.hpp"

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

TEST_CASE("make_binary_controlled_circuit_from_binary_powers() for single qubit gate")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    const auto angle = 1.2345;

    // create the circuit manually, by repeating the gates
    auto manual_circuit = mqis::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{angle, 0, 3}});
    manual_circuit.add_cp_gate({{angle, 1, 3}, {angle, 1, 3}});
    manual_circuit.add_cp_gate({{angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuits = std::vector<mqis::QuantumCircuit> {};
    for (std::size_t i {0}; i < 3; ++i) {
        const auto power_angle = static_cast<double>(1ul << i) * angle;

        auto subcircuit = mqis::QuantumCircuit {1};
        subcircuit.add_u_gate(mqis::p_gate(power_angle), 0);
        subcircuits.emplace_back(std::move(subcircuit));
    }

    auto binary_made_circuit = mqis::make_binary_controlled_circuit_from_binary_powers(subcircuits, 4, {0, 1, 2}, {3});

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(manual_circuit, state0);
    mqis::simulate(binary_made_circuit, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

TEST_CASE("make_binary_controlled_circuit_from_binary_powers() for double qubit gate")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    auto streams = std::vector<std::stringstream> {};
    streams.emplace_back(get_gate_pow_1_stream());
    streams.emplace_back(get_gate_pow_2_stream());

    const auto circuits_pow = [&]() {
        auto output = std::vector<mqis::QuantumCircuit> {};

        for (auto& stream : streams) {
            const auto gates = mqis::read_decomposed_gate_info(stream);
            output.emplace_back(mqis::make_circuit_from_decomposed_gates(gates));
        }

        return output;
    }();

    const auto binary_made_circuit_naive = mqis::make_binary_controlled_circuit_naive(circuits_pow[0], 4, {0, 1}, {2, 3});
    const auto binary_made_circuit_power = mqis::make_binary_controlled_circuit_from_binary_powers(circuits_pow, 4, {0, 1}, {2, 3});

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(binary_made_circuit_naive, state0);
    mqis::simulate(binary_made_circuit_power, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

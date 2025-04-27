#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/circuit_operations/make_binary_controlled_circuit.hpp"

TEST_CASE("make_binary_controlled_circuit()")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    const auto angle = M_PI_4;
    const auto t_gate = ket::p_gate(angle);

    // create the circuit manually, by repeating the gates
    auto manual_circuit = ket::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{0, 3, angle}});
    manual_circuit.add_cp_gate({{1, 3, angle}, {1, 3, angle}});
    manual_circuit.add_cp_gate({{2, 3, angle}, {2, 3, angle}, {2, 3, angle}, {2, 3, angle}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuit = ket::QuantumCircuit {1};
    subcircuit.add_u_gate(t_gate, 0);
    auto binary_made_circuit = ket::make_binary_controlled_circuit_naive(subcircuit, 4, {0, 1, 2}, {3});

    auto state0 = ket::QuantumState {init_bitstring};
    auto state1 = ket::QuantumState {init_bitstring};

    ket::simulate(manual_circuit, state0);
    ket::simulate(binary_made_circuit, state1);

    REQUIRE(ket::almost_eq(state0, state1));
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
    auto manual_circuit = ket::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{0, 3, angle}});
    manual_circuit.add_cp_gate({{1, 3, angle}, {1, 3, angle}});
    manual_circuit.add_cp_gate({{2, 3, angle}, {2, 3, angle}, {2, 3, angle}, {2, 3, angle}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuits = std::vector<ket::QuantumCircuit> {};
    for (std::size_t i {0}; i < 3; ++i) {
        const auto power_angle = static_cast<double>(1UL << i) * angle;

        auto subcircuit = ket::QuantumCircuit {1};
        subcircuit.add_u_gate(ket::p_gate(power_angle), 0);
        subcircuits.emplace_back(std::move(subcircuit));
    }

    auto binary_made_circuit = ket::make_binary_controlled_circuit_from_binary_powers(subcircuits, 4, {0, 1, 2}, {3});

    auto state0 = ket::QuantumState {init_bitstring};
    auto state1 = ket::QuantumState {init_bitstring};

    ket::simulate(manual_circuit, state0);
    ket::simulate(binary_made_circuit, state1);

    REQUIRE(ket::almost_eq(state0, state1));
}

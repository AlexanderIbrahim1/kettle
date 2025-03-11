// TODO: remove
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/common/print.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/gates/toffoli.hpp"
#include "mini-qiskit/circuit_operations/make_controlled_circuit.hpp"
#include "mini-qiskit/circuit_operations/compare_circuits.hpp"

TEST_CASE("make_controlled_circuit()")
{
    SECTION("single-qubit gates")
    {
        auto subcircuit = mqis::QuantumCircuit {1};
        auto expected = mqis::QuantumCircuit {2};

        SECTION("single x-gate")
        {
            subcircuit.add_x_gate(0);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(mqis::x_gate(), 0, 1);

            REQUIRE(mqis::almost_eq(new_circuit, expected));
        }

        SECTION("single rx-gate")
        {
            const auto angle = 1.2345;
            subcircuit.add_rx_gate(angle, 0);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(mqis::rx_gate(angle), 0, 1);

            REQUIRE(mqis::almost_eq(new_circuit, expected));
        }

        SECTION("single h-gate")
        {
            subcircuit.add_h_gate(0);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(mqis::h_gate(), 0, 1);

            REQUIRE(mqis::almost_eq(new_circuit, expected));
        }

        SECTION("single u-gate")
        {
            subcircuit.add_u_gate(mqis::sqrt_x_gate(), 0);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(mqis::sqrt_x_gate(), 0, 1);

            REQUIRE(mqis::almost_eq(new_circuit, expected));
        }
    }

    SECTION("double-qubit gates")
    {
        auto subcircuit = mqis::QuantumCircuit {2};
        auto expected = mqis::QuantumCircuit {3};

        const auto init_bitstring = std::string {
            GENERATE("000", "100", "010", "110", "001", "101", "011", "111")
        };

        auto state0 = mqis::QuantumState {init_bitstring};
        auto state1 = mqis::QuantumState {init_bitstring};


        // NOTE: for these cases, we generate two different circuits that both do the same thing to all
        // 8 possible initial computational basis states

        SECTION("single cx-gate")
        {
            subcircuit.add_cx_gate(0, 1);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            mqis::apply_toffoli_gate(expected, {0, 1}, 2);

            mqis::simulate(new_circuit, state0);
            mqis::simulate(expected, state1);

            REQUIRE(mqis::almost_eq(state0, state1));
        }

        SECTION("single crx-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_crx_gate(angle, 0, 1);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            mqis::apply_multiplicity_controlled_u_gate(expected, mqis::rx_gate(angle), 2, {0, 1});

            mqis::simulate(new_circuit, state0);
            mqis::simulate(expected, state1);

            REQUIRE(mqis::almost_eq(state0, state1));
        }

        SECTION("single cp-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_cp_gate(angle, 0, 1);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            mqis::apply_multiplicity_controlled_u_gate(expected, mqis::p_gate(angle), 2, {0, 1});

            mqis::simulate(new_circuit, state0);
            mqis::simulate(expected, state1);

            REQUIRE(mqis::almost_eq(state0, state1));
        }

        SECTION("single cu-gate")
        {
            subcircuit.add_cu_gate(mqis::sqrt_x_gate(), 0, 1);
            auto new_circuit = mqis::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            mqis::apply_multiplicity_controlled_u_gate(expected, mqis::sqrt_x_gate(), 2, {0, 1});

            mqis::simulate(new_circuit, state0);
            mqis::simulate(expected, state1);

            REQUIRE(mqis::almost_eq(state0, state1));
        }
    }

    SECTION("throws if measurement gate is found")
    {
        auto subcircuit = mqis::QuantumCircuit {1};
        subcircuit.add_m_gate(0);

        REQUIRE_THROWS_AS(mqis::make_controlled_circuit(subcircuit, 2, 0, {1}), std::runtime_error);
    }
}

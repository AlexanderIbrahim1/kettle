#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/state/state.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/multiplicity_controlled_u_gate.hpp"
#include "kettle/gates/toffoli.hpp"
#include "kettle/circuit_operations/make_controlled_circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"

TEST_CASE("make_controlled_circuit()")
{
    SECTION("single-qubit gates")
    {
        auto subcircuit = ket::QuantumCircuit {1};
        auto expected = ket::QuantumCircuit {2};

        SECTION("single x-gate")
        {
            subcircuit.add_x_gate(0);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(ket::x_gate(), 0, 1);

            REQUIRE(ket::almost_eq(new_circuit, expected));
        }

        SECTION("single rx-gate")
        {
            const auto angle = 1.2345;
            subcircuit.add_rx_gate(0, angle);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(ket::rx_gate(angle), 0, 1);

            REQUIRE(ket::almost_eq(new_circuit, expected));
        }

        SECTION("single h-gate")
        {
            subcircuit.add_h_gate(0);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(ket::h_gate(), 0, 1);

            REQUIRE(ket::almost_eq(new_circuit, expected));
        }

        SECTION("single u-gate")
        {
            subcircuit.add_u_gate(ket::sx_gate(), 0);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 2, 0, {1});

            expected.add_cu_gate(ket::sx_gate(), 0, 1);

            REQUIRE(ket::almost_eq(new_circuit, expected));
        }
    }

    SECTION("double-qubit gates")
    {
        auto subcircuit = ket::QuantumCircuit {2};
        auto expected = ket::QuantumCircuit {3};

        const auto init_bitstring = std::string {
            GENERATE("000", "100", "010", "110", "001", "101", "011", "111")
        };

        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = ket::QuantumState {init_bitstring};


        // NOTE: for these cases, we generate two different circuits that both do the same thing to all
        // 8 possible initial computational basis states

        SECTION("single cx-gate")
        {
            subcircuit.add_cx_gate(0, 1);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            ket::apply_toffoli_gate(expected, {0, 1}, 2);

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single crx-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_crx_gate(0, 1, angle);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::rx_gate(angle), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single cp-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_cp_gate(0, 1, angle);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::p_gate(angle), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single cu-gate")
        {
            subcircuit.add_cu_gate(ket::sx_gate(), 0, 1);
            auto new_circuit = ket::make_controlled_circuit(subcircuit, 3, 0, {1, 2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::sx_gate(), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }
    }

    SECTION("several gates")
    {
        const auto angle = double {1.2345};

        const auto init_bitstring = std::string {
            GENERATE(
                "00000", "10000", "01000", "11000", "00100", "10100", "01100", "11100",
                "00010", "10010", "01010", "11010", "00110", "10110", "01110", "11110",
                "00001", "10001", "01001", "11001", "00101", "10101", "01101", "11101",
                "00011", "10011", "01011", "11011", "00111", "10111", "01111", "11111"
            )
        };

        auto subcircuit = ket::QuantumCircuit {3};
        subcircuit.add_x_gate(0);
        subcircuit.add_x_gate(1);
        subcircuit.add_h_gate(1);
        subcircuit.add_h_gate(2);
        subcircuit.add_rx_gate(2, angle);
        subcircuit.add_cx_gate(1, 2);

        auto new_circuit = ket::make_controlled_circuit(subcircuit, 5, 0, {2, 3, 4});

        auto expected = ket::QuantumCircuit {5};
        expected.add_cu_gate(ket::x_gate(), 0, 2);
        expected.add_cu_gate(ket::x_gate(), 0, 3);
        expected.add_cu_gate(ket::h_gate(), 0, 3);
        expected.add_cu_gate(ket::h_gate(), 0, 4);
        expected.add_cu_gate(ket::rx_gate(angle), 0, 4);
        ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 4, {0, 3});

        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = ket::QuantumState {init_bitstring};

        ket::simulate(new_circuit, state0);
        ket::simulate(expected, state1);

        REQUIRE(ket::almost_eq(state0, state1));
    }

    SECTION("throws if measurement gate is found")
    {
        auto subcircuit = ket::QuantumCircuit {1};
        subcircuit.add_m_gate(0);

        REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 2, 0, {1}), std::runtime_error);
    }
}

TEST_CASE("throwing with make_controlled_circuit()")
{
    auto subcircuit = ket::QuantumCircuit {2};
    subcircuit.add_cx_gate({{0, 1}, {1, 0}});

    SECTION("throws when all indices are not unique")
    {
        REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 3, 0, {1, 1}), std::runtime_error);
    }

    SECTION("throws when an invalid number of indices are given")
    {
        SECTION("too few")
        {
            REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 3, 0, {1}), std::runtime_error);
        }

        SECTION("too many")
        {
            REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 3, 0, {1, 2, 3}), std::runtime_error);
        }
    }

    SECTION("throws when a mapped qubit is the control qubit")
    {
        REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 3, 0, {0, 1}), std::runtime_error);
    }

    SECTION("throws when not all indices fit onto the new circuit")
    {
        REQUIRE_THROWS_AS(ket::make_controlled_circuit(subcircuit, 2, 0, {1, 2}), std::runtime_error);
    }
}

TEST_CASE("make_multiplicity_controlled_circuit()")
{
    SECTION("single-qubit gates")
    {
        auto subcircuit = ket::QuantumCircuit {1};
        auto expected = ket::QuantumCircuit {3};

        const auto init_bitstring = std::string {
            GENERATE("000", "100", "010", "110", "001", "101", "011", "111")
        };

        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = ket::QuantumState {init_bitstring};

        SECTION("single x-gate")
        {
            subcircuit.add_x_gate(0);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single rx-gate")
        {
            const auto angle = 1.2345;
            subcircuit.add_rx_gate(0, angle);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::rx_gate(angle), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single h-gate")
        {
            subcircuit.add_h_gate(0);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::h_gate(), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single u-gate")
        {
            subcircuit.add_u_gate(ket::sx_gate(), 0);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::sx_gate(), 2, {0, 1});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }
    }

    SECTION("double-qubit gates")
    {
        auto subcircuit = ket::QuantumCircuit {2};
        auto expected = ket::QuantumCircuit {4};

        const auto init_bitstring = std::string {
            GENERATE(
                "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
                "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
            )
        };

        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = ket::QuantumState {init_bitstring};

        // NOTE: for these cases, we generate two different circuits that both do the same thing to all
        // 8 possible initial computational basis states

        SECTION("single cx-gate")
        {
            subcircuit.add_cx_gate(0, 1);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 3});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 3, {0, 1, 2});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single crx-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_crx_gate(0, 1, angle);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 3});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::rx_gate(angle), 3, {0, 1, 2});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single cp-gate")
        {
            const auto angle = double {1.2345};
            subcircuit.add_cp_gate(0, 1, angle);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 3});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::p_gate(angle), 3, {0, 1, 2});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }

        SECTION("single cu-gate")
        {
            subcircuit.add_cu_gate(ket::sx_gate(), 0, 1);
            auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 3});

            ket::apply_multiplicity_controlled_u_gate(expected, ket::sx_gate(), 3, {0, 1, 2});

            ket::simulate(new_circuit, state0);
            ket::simulate(expected, state1);

            REQUIRE(ket::almost_eq(state0, state1));
        }
    }

    SECTION("several gates")
    {
        const auto angle = double {1.2345};

        const auto init_bitstring = std::string {
            GENERATE(
                "00000", "10000", "01000", "11000", "00100", "10100", "01100", "11100",
                "00010", "10010", "01010", "11010", "00110", "10110", "01110", "11110",
                "00001", "10001", "01001", "11001", "00101", "10101", "01101", "11101",
                "00011", "10011", "01011", "11011", "00111", "10111", "01111", "11111"
            )
        };

        auto subcircuit = ket::QuantumCircuit {3};
        subcircuit.add_x_gate(0);
        subcircuit.add_x_gate(1);
        subcircuit.add_h_gate(1);
        subcircuit.add_h_gate(2);
        subcircuit.add_rx_gate(2, angle);
        subcircuit.add_cx_gate(1, 2);

        auto new_circuit = ket::make_multiplicity_controlled_circuit(subcircuit, 5, {0, 1}, {2, 3, 4});

        auto expected = ket::QuantumCircuit {5};
        ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 2, {0, 1});
        ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 3, {0, 1});
        ket::apply_multiplicity_controlled_u_gate(expected, ket::h_gate(), 3, {0, 1});
        ket::apply_multiplicity_controlled_u_gate(expected, ket::h_gate(), 4, {0, 1});
        ket::apply_multiplicity_controlled_u_gate(expected, ket::rx_gate(angle), 4, {0, 1});
        ket::apply_multiplicity_controlled_u_gate(expected, ket::x_gate(), 4, {0, 1, 3});

        auto state0 = ket::QuantumState {init_bitstring};
        auto state1 = ket::QuantumState {init_bitstring};

        ket::simulate(new_circuit, state0);
        ket::simulate(expected, state1);

        REQUIRE(ket::almost_eq(state0, state1));
    }

    SECTION("throws if measurement gate is found")
    {
        auto subcircuit = ket::QuantumCircuit {1};
        subcircuit.add_m_gate(0);

        REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2}), std::runtime_error);
    }
}

TEST_CASE("throwing with make_multiplicity_controlled_circuit()")
{
    auto subcircuit = ket::QuantumCircuit {2};
    subcircuit.add_cx_gate({{0, 1}, {1, 0}});

    SECTION("throws when all mapped indices are not unique")
    {
        REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 2}), std::runtime_error);
    }

    SECTION("throws when all control indices are not unique")
    {
        REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 0}, {2, 3}), std::runtime_error);
    }

    SECTION("throws when an invalid number of indices are given")
    {
        SECTION("too few")
        {
            REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2}), std::runtime_error);
        }

        SECTION("too many")
        {
            REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {2, 3, 4}), std::runtime_error);
        }
    }

    SECTION("throws when a mapped qubit is the control qubit")
    {
        REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 4, {0, 1}, {0, 2}), std::runtime_error);
    }

    SECTION("throws when not all indices fit onto the new circuit")
    {
        REQUIRE_THROWS_AS(ket::make_multiplicity_controlled_circuit(subcircuit, 3, {0, 1}, {2, 3}), std::runtime_error);
    }
}

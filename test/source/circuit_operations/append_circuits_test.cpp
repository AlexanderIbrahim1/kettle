#include <catch2/catch_test_macros.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/circuit_operations/append_circuits.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"


TEST_CASE("append_circuits working")
{
    SECTION("basic test")
    {
        auto circuit0 = ket::QuantumCircuit {1};
        circuit0.add_x_gate(0);

        auto circuit1 = ket::QuantumCircuit {1};
        circuit1.add_x_gate(0);

        const auto combined_circuit01 = ket::append_circuits(circuit0, circuit1);

        auto expected = ket::QuantumCircuit {1};
        expected.add_x_gate({0, 0});

        REQUIRE(ket::almost_eq(combined_circuit01, expected));
    }

    SECTION("several gates")
    {
        auto circuit0 = ket::QuantumCircuit {3};
        auto circuit1 = ket::QuantumCircuit {3};

        const auto add_left_gates = [&](ket::QuantumCircuit& circuit) {
            circuit.add_x_gate(0);
            circuit.add_h_gate(1);
            circuit.add_y_gate({0, 1, 2});
            circuit.add_rx_gate({{0, M_PI_4}, {2, M_1_PI}});
        };

        const auto add_right_gates = [&](ket::QuantumCircuit& circuit) {
            circuit.add_h_gate({1, 2});
            circuit.add_z_gate(0);
            circuit.add_u_gate(ket::x_gate(), 2);
        };

        add_left_gates(circuit0);
        add_right_gates(circuit1);
        const auto combined_circuit01 = ket::append_circuits(circuit0, circuit1);

        SECTION("matching gates on combined circuit")
        {
            auto expected = ket::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);

            REQUIRE(ket::almost_eq(combined_circuit01, expected));
        }

        SECTION("not matching gates on combined circuit")
        {
            auto expected = ket::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);
            expected.add_x_gate(1);

            REQUIRE(!ket::almost_eq(combined_circuit01, expected));
        }
    }
}

TEST_CASE("extend_circuit working")
{
    SECTION("basic test")
    {
        auto circuit0 = ket::QuantumCircuit {1};
        circuit0.add_x_gate(0);

        auto circuit1 = ket::QuantumCircuit {1};
        circuit1.add_x_gate(0);

        ket::extend_circuit(circuit0, circuit1);

        auto expected = ket::QuantumCircuit {1};
        expected.add_x_gate({0, 0});

        REQUIRE(ket::almost_eq(circuit0, expected));
    }

    SECTION("several gates")
    {
        auto circuit0 = ket::QuantumCircuit {3};
        auto circuit1 = ket::QuantumCircuit {3};

        const auto add_left_gates = [&](ket::QuantumCircuit& circuit) {
            circuit.add_x_gate(0);
            circuit.add_h_gate(1);
            circuit.add_y_gate({0, 1, 2});
            circuit.add_rx_gate({{0, M_PI_4}, {2, M_1_PI}});
        };

        const auto add_right_gates = [&](ket::QuantumCircuit& circuit) {
            circuit.add_h_gate({1, 2});
            circuit.add_z_gate(0);
            circuit.add_u_gate(ket::x_gate(), 2);
        };

        add_left_gates(circuit0);
        add_right_gates(circuit1);
        ket::extend_circuit(circuit0, circuit1);

        SECTION("matching gates on combined circuit")
        {
            auto expected = ket::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);

            REQUIRE(ket::almost_eq(circuit0, expected));
        }

        SECTION("not matching gates on combined circuit")
        {
            auto expected = ket::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);
            expected.add_x_gate(1);

            REQUIRE(!ket::almost_eq(circuit0, expected));
        }
    }
}

TEST_CASE("extend_circuit() with control flow")
{
    const auto if_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_x_gate(0); return circ; }();
    const auto else_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_h_gate(0); return circ; }();

    SECTION("if_statement()")
    {
        auto total_circuit = ket::QuantumCircuit {2};
        total_circuit.add_x_gate(0);
        total_circuit.add_m_gate(0);
        total_circuit.add_if_statement(0, if_subcirc);
        total_circuit.add_y_gate(1);
        total_circuit.add_if_statement(0, if_subcirc);
        total_circuit.add_y_gate(1);

        SECTION("left circuit has if statement")
        {
            auto left_circuit = ket::QuantumCircuit {2};
            left_circuit.add_x_gate(0);
            left_circuit.add_m_gate(0);
            left_circuit.add_if_statement(0, if_subcirc);
            left_circuit.add_y_gate(1);
            left_circuit.add_if_statement(0, if_subcirc);

            auto right_circuit = ket::QuantumCircuit {2};
            right_circuit.add_y_gate(1);

            ket::extend_circuit(left_circuit, right_circuit);

            // NOLINTNEXTLINE(readability-suspicious-call-argument)
            REQUIRE(ket::almost_eq(total_circuit, left_circuit));
        }

        SECTION("right circuit has if statement")
        {
            auto left_circuit = ket::QuantumCircuit {2};
            left_circuit.add_x_gate(0);
            left_circuit.add_m_gate(0);

            auto right_circuit = ket::QuantumCircuit {2};
            right_circuit.add_if_statement(0, if_subcirc);
            right_circuit.add_y_gate(1);
            right_circuit.add_if_statement(0, if_subcirc);
            right_circuit.add_y_gate(1);

            ket::extend_circuit(left_circuit, right_circuit);

            // NOLINTNEXTLINE(readability-suspicious-call-argument)
            REQUIRE(ket::almost_eq(total_circuit, left_circuit));
        }

        SECTION("both left and right circuits have if statement")
        {
            auto left_circuit = ket::QuantumCircuit {2};
            left_circuit.add_x_gate(0);
            left_circuit.add_m_gate(0);
            left_circuit.add_if_statement(0, if_subcirc);

            auto right_circuit = ket::QuantumCircuit {2};
            right_circuit.add_y_gate(1);
            right_circuit.add_if_statement(0, if_subcirc);
            right_circuit.add_y_gate(1);

            ket::extend_circuit(left_circuit, right_circuit);

            // NOLINTNEXTLINE(readability-suspicious-call-argument)
            REQUIRE(ket::almost_eq(total_circuit, left_circuit));
        }
    }

    SECTION("if_else_statement()")
    {
        auto total_circuit = ket::QuantumCircuit {2};
        total_circuit.add_x_gate(0);
        total_circuit.add_m_gate(0);
        total_circuit.add_if_else_statement(0, if_subcirc, else_subcirc);
        total_circuit.add_y_gate(1);
        total_circuit.add_if_else_statement(0, if_subcirc, else_subcirc);
        total_circuit.add_y_gate(1);

        SECTION("both left and right circuits have if statement")
        {
            auto left_circuit = ket::QuantumCircuit {2};
            left_circuit.add_x_gate(0);
            left_circuit.add_m_gate(0);
            left_circuit.add_if_else_statement(0, if_subcirc, else_subcirc);

            auto right_circuit = ket::QuantumCircuit {2};
            right_circuit.add_y_gate(1);
            right_circuit.add_if_else_statement(0, if_subcirc, else_subcirc);
            right_circuit.add_y_gate(1);

            ket::extend_circuit(left_circuit, right_circuit);

            // NOLINTNEXTLINE(readability-suspicious-call-argument)
            REQUIRE(ket::almost_eq(total_circuit, left_circuit));
        }
    }
}

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"
#include "kettle/parameter/parameter.hpp"

TEST_CASE("almost_eq() with control flow statements")
{
    auto circuit0 = ket::QuantumCircuit {2};
    circuit0.add_x_gate(0);
    circuit0.add_y_gate(1);
    circuit0.add_m_gate(0);

    auto circuit1 = ket::QuantumCircuit {2};
    circuit1.add_x_gate(0);
    circuit1.add_y_gate(1);
    circuit1.add_m_gate(0);

    auto if_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_x_gate(0); return circ; }();
    auto else_subcirc = [] { auto circ = ket::QuantumCircuit{2}; circ.add_h_gate(0); return circ; }();

    SECTION("if_statement()")
    {
        circuit0.add_if_statement(0, if_subcirc);
        circuit0.add_z_gate(1);

        circuit1.add_if_statement(0, if_subcirc);
        circuit1.add_z_gate(1);

        SECTION("equal")
        {
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("not equal")
        {
            circuit0.add_h_gate(0);
            REQUIRE(!ket::almost_eq(circuit0, circuit1));
        }
    }

    SECTION("if_else_statement()")
    {
        circuit0.add_if_else_statement(0, if_subcirc, else_subcirc);
        circuit0.add_z_gate(1);

        circuit1.add_if_else_statement(0, if_subcirc, else_subcirc);
        circuit1.add_z_gate(1);

        SECTION("equal")
        {
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("not equal")
        {
            circuit0.add_h_gate(0);
            REQUIRE(!ket::almost_eq(circuit0, circuit1));
        }
    }
}

TEST_CASE("almost_eq(); circuit loggers do not affect comparison")
{
    auto circuit0 = ket::QuantumCircuit {1};
    auto circuit1 = ket::QuantumCircuit {1};

    SECTION("one circuit logger on left, before gate")
    {
        circuit0.add_classical_register_circuit_logger();
        circuit0.add_x_gate(0);

        circuit1.add_x_gate(0);

        REQUIRE(ket::almost_eq(circuit0, circuit1));
    }

    SECTION("one circuit logger on left, after gate")
    {
        circuit0.add_x_gate(0);
        circuit0.add_classical_register_circuit_logger();

        circuit1.add_x_gate(0);

        REQUIRE(ket::almost_eq(circuit0, circuit1));
    }

    SECTION("one circuit logger on right, before gate")
    {
        circuit0.add_x_gate(0);

        circuit1.add_classical_register_circuit_logger();
        circuit1.add_x_gate(0);

        REQUIRE(ket::almost_eq(circuit0, circuit1));
    }

    SECTION("one circuit logger on right, after gate")
    {
        circuit0.add_x_gate(0);

        circuit1.add_x_gate(0);
        circuit1.add_classical_register_circuit_logger();

        REQUIRE(ket::almost_eq(circuit0, circuit1));
    }

    SECTION("circuit loggers on both left and right")
    {
        circuit1.add_classical_register_circuit_logger();
        circuit0.add_x_gate(0);
        circuit1.add_classical_register_circuit_logger();
        circuit1.add_classical_register_circuit_logger();
        circuit0.add_h_gate(0);

        circuit1.add_x_gate(0);
        circuit1.add_h_gate(0);
        circuit1.add_classical_register_circuit_logger();

        REQUIRE(ket::almost_eq(circuit0, circuit1));
    }
}

TEST_CASE("compare parameterized circuits")
{
    auto circuit0 = ket::QuantumCircuit {2};
    circuit0.add_rx_gate(0, 0.1234, ket::param::parameterized {});
    circuit0.add_rx_gate(0, 1.2345);
    circuit0.add_rx_gate(1, 2.3456, ket::param::parameterized {});

    auto circuit1 = ket::QuantumCircuit {2};
    circuit1.add_rx_gate(0, 0.1234, ket::param::parameterized {});
    circuit1.add_rx_gate(0, 1.2345, ket::param::parameterized {});
    circuit1.add_rx_gate(1, 2.3456);

    REQUIRE(ket::almost_eq(circuit0, circuit1));
}

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"

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

#include <catch2/catch_test_macros.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/circuit_operations/append_circuits.hpp"
#include "mini-qiskit/circuit_operations/compare_circuits.hpp"


TEST_CASE("append_circuits working")
{
    SECTION("basic test")
    {
        auto circuit0 = mqis::QuantumCircuit {1};
        circuit0.add_x_gate(0);

        auto circuit1 = mqis::QuantumCircuit {1};
        circuit1.add_x_gate(0);

        const auto combined_circuit01 = mqis::append_circuits(circuit0, circuit1);

        auto expected = mqis::QuantumCircuit {1};
        expected.add_x_gate({0, 0});

        REQUIRE(mqis::almost_eq(combined_circuit01, expected));
    }

    SECTION("several gates")
    {
        auto circuit0 = mqis::QuantumCircuit {3};
        auto circuit1 = mqis::QuantumCircuit {3};

        const auto add_left_gates = [&](mqis::QuantumCircuit& circuit) {
            circuit.add_x_gate(0);
            circuit.add_h_gate(1);
            circuit.add_y_gate({0, 1, 2});
            circuit.add_rx_gate({{0, M_PI_4}, {2, M_1_PI}});
        };

        const auto add_right_gates = [&](mqis::QuantumCircuit& circuit) {
            circuit.add_h_gate({1, 2});
            circuit.add_z_gate(0);
            circuit.add_u_gate(mqis::x_gate(), 2);
        };

        add_left_gates(circuit0);
        add_right_gates(circuit1);
        const auto combined_circuit01 = mqis::append_circuits(circuit0, circuit1);

        SECTION("matching gates on combined circuit")
        {
            auto expected = mqis::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);

            REQUIRE(mqis::almost_eq(combined_circuit01, expected));
        }

        SECTION("not matching gates on combined circuit")
        {
            auto expected = mqis::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);
            expected.add_x_gate(1);

            REQUIRE(!mqis::almost_eq(combined_circuit01, expected));
        }
    }

    // TODO: remove, this is no longer a restriction
    // SECTION("throws due to measurement conflicts")
    // {
    //     auto circuit0 = mqis::QuantumCircuit {3};
    //     auto circuit1 = mqis::QuantumCircuit {3};

    //     SECTION("adding another measurement")
    //     {
    //         circuit0.add_m_gate(0);
    //         circuit1.add_m_gate(0);

    //         REQUIRE_THROWS_AS(mqis::append_circuits(circuit0, circuit1), std::runtime_error);
    //     }

    //     SECTION("adding a different gate")
    //     {
    //         circuit0.add_m_gate(0);
    //         circuit1.add_x_gate(0);

    //         REQUIRE_THROWS_AS(mqis::append_circuits(circuit0, circuit1), std::runtime_error);
    //     }
    // }

    // TODO: remove, this is no longer a restriction
    // SECTION("does not throw due to measurements on other qubits")
    // {
    //     auto circuit0 = mqis::QuantumCircuit {3};
    //     auto circuit1 = mqis::QuantumCircuit {3};

    //     circuit0.add_m_gate({0, 1});
    //     circuit1.add_x_gate(2);
    //     circuit1.add_m_gate(2);

    //     REQUIRE_NOTHROW(mqis::append_circuits(circuit0, circuit1));
    // }
}

TEST_CASE("extend_circuit working")
{
    SECTION("basic test")
    {
        auto circuit0 = mqis::QuantumCircuit {1};
        circuit0.add_x_gate(0);

        auto circuit1 = mqis::QuantumCircuit {1};
        circuit1.add_x_gate(0);

        mqis::extend_circuit(circuit0, circuit1);

        auto expected = mqis::QuantumCircuit {1};
        expected.add_x_gate({0, 0});

        REQUIRE(mqis::almost_eq(circuit0, expected));
    }

    SECTION("several gates")
    {
        auto circuit0 = mqis::QuantumCircuit {3};
        auto circuit1 = mqis::QuantumCircuit {3};

        const auto add_left_gates = [&](mqis::QuantumCircuit& circuit) {
            circuit.add_x_gate(0);
            circuit.add_h_gate(1);
            circuit.add_y_gate({0, 1, 2});
            circuit.add_rx_gate({{0, M_PI_4}, {2, M_1_PI}});
        };

        const auto add_right_gates = [&](mqis::QuantumCircuit& circuit) {
            circuit.add_h_gate({1, 2});
            circuit.add_z_gate(0);
            circuit.add_u_gate(mqis::x_gate(), 2);
        };

        add_left_gates(circuit0);
        add_right_gates(circuit1);
        mqis::extend_circuit(circuit0, circuit1);

        SECTION("matching gates on combined circuit")
        {
            auto expected = mqis::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);

            REQUIRE(mqis::almost_eq(circuit0, expected));
        }

        SECTION("not matching gates on combined circuit")
        {
            auto expected = mqis::QuantumCircuit {3};
            add_left_gates(expected);
            add_right_gates(expected);
            expected.add_x_gate(1);

            REQUIRE(!mqis::almost_eq(circuit0, expected));
        }
    }

    // TODO: remove, this is no longer a restriction
    // SECTION("throws due to measurement conflicts")
    // {
    //     auto circuit0 = mqis::QuantumCircuit {3};
    //     auto circuit1 = mqis::QuantumCircuit {3};

    //     SECTION("adding another measurement")
    //     {
    //         circuit0.add_m_gate(0);
    //         circuit1.add_m_gate(0);

    //         REQUIRE_THROWS_AS(mqis::extend_circuit(circuit0, circuit1), std::runtime_error);
    //     }

    //     SECTION("adding a different gate")
    //     {
    //         circuit0.add_m_gate(0);
    //         circuit1.add_x_gate(0);

    //         REQUIRE_THROWS_AS(mqis::extend_circuit(circuit0, circuit1), std::runtime_error);
    //     }
    // }

    // TODO: remove, this is no longer a restriction
    // SECTION("does not throw due to measurements on other qubits")
    // {
    //     auto circuit0 = mqis::QuantumCircuit {3};
    //     auto circuit1 = mqis::QuantumCircuit {3};

    //     circuit0.add_m_gate({0, 1});
    //     circuit1.add_x_gate(2);
    //     circuit1.add_m_gate(2);

    //     REQUIRE_NOTHROW(mqis::extend_circuit(circuit0, circuit1));
    // }
}

#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/circuit_operations/compare_circuits.hpp"

static constexpr auto ABS_TOL = double {1.0e-6};

TEST_CASE("add multiple X gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 1")
    {
        const auto indices = std::vector<std::size_t> {1};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 1);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[0]) == 1);
        REQUIRE(circuit[0].gate == mqis::Gate::X);
    }

    SECTION("add 0, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 2};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 2);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[1]) == 2);
        REQUIRE(circuit[1].gate == mqis::Gate::X);
    }

    SECTION("add 0, 1, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 1, 2};
        circuit.add_x_gate(indices);

        REQUIRE(number_of_gates(circuit) == 3);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[1]) == 1);
        REQUIRE(circuit[1].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[2]) == 2);
        REQUIRE(circuit[2].gate == mqis::Gate::X);
    }

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_x_gate({0, 1, 2});

        REQUIRE(number_of_gates(circuit) == 3);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[0]) == 0);
        REQUIRE(circuit[0].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[1]) == 1);
        REQUIRE(circuit[1].gate == mqis::Gate::X);

        REQUIRE(impl_mqis::unpack_one_target_gate(circuit[2]) == 2);
        REQUIRE(circuit[2].gate == mqis::Gate::X);
    }
}

TEST_CASE("add multiple RX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_rx_gate({
            {0.25, 0},
            {0.5,  1},
            {0.75, 2}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto rx_gate0 = impl_mqis::unpack_one_target_one_angle_gate(circuit[0]);
        REQUIRE(std::get<0>(rx_gate0) == 0);
        REQUIRE_THAT(std::get<1>(rx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
        REQUIRE(circuit[0].gate == mqis::Gate::RX);

        const auto rx_gate1 = impl_mqis::unpack_one_target_one_angle_gate(circuit[1]);
        REQUIRE(std::get<0>(rx_gate1) == 1);
        REQUIRE_THAT(std::get<1>(rx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
        REQUIRE(circuit[1].gate == mqis::Gate::RX);

        const auto rx_gate2 = impl_mqis::unpack_one_target_one_angle_gate(circuit[2]);
        REQUIRE(std::get<0>(rx_gate2) == 2);
        REQUIRE_THAT(std::get<1>(rx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
        REQUIRE(circuit[2].gate == mqis::Gate::RX);
    }
}

TEST_CASE("add multiple CX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_cx_gate({
            {0, 1},
            {1, 2},
            {2, 0}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto cx_gate0 = impl_mqis::unpack_one_control_one_target_gate(circuit[0]);
        REQUIRE(std::get<0>(cx_gate0) == 0);
        REQUIRE(std::get<1>(cx_gate0) == 1);
        REQUIRE(circuit[0].gate == mqis::Gate::CX);

        const auto cx_gate1 = impl_mqis::unpack_one_control_one_target_gate(circuit[1]);
        REQUIRE(std::get<0>(cx_gate1) == 1);
        REQUIRE(std::get<1>(cx_gate1) == 2);
        REQUIRE(circuit[1].gate == mqis::Gate::CX);

        const auto cx_gate2 = impl_mqis::unpack_one_control_one_target_gate(circuit[2]);
        REQUIRE(std::get<0>(cx_gate2) == 2);
        REQUIRE(std::get<1>(cx_gate2) == 0);
        REQUIRE(circuit[2].gate == mqis::Gate::CX);
    }
}

TEST_CASE("add multiple CRX gates")
{
    auto circuit = mqis::QuantumCircuit {3};

    const auto number_of_gates = [](const mqis::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_crx_gate({
            {0.25, 0, 1},
            {0.5,  1, 2},
            {0.75, 2, 0}
        });

        REQUIRE(number_of_gates(circuit) == 3);

        const auto crx_gate0 = impl_mqis::unpack_one_control_one_target_one_angle_gate(circuit[0]);
        REQUIRE(std::get<0>(crx_gate0) == 0);
        REQUIRE(std::get<1>(crx_gate0) == 1);
        REQUIRE_THAT(std::get<2>(crx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
        REQUIRE(circuit[0].gate == mqis::Gate::CRX);

        const auto crx_gate1 = impl_mqis::unpack_one_control_one_target_one_angle_gate(circuit[1]);
        REQUIRE(std::get<0>(crx_gate1) == 1);
        REQUIRE(std::get<1>(crx_gate1) == 2);
        REQUIRE_THAT(std::get<2>(crx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
        REQUIRE(circuit[1].gate == mqis::Gate::CRX);

        const auto crx_gate2 = impl_mqis::unpack_one_control_one_target_one_angle_gate(circuit[2]);
        REQUIRE(std::get<0>(crx_gate2) == 2);
        REQUIRE(std::get<1>(crx_gate2) == 0);
        REQUIRE_THAT(std::get<2>(crx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
        REQUIRE(circuit[2].gate == mqis::Gate::CRX);
    }
}

TEST_CASE("QuantumCircuit: almost_eq()")
{
    SECTION("not equals")
    {
        SECTION("different number of qubits")
        {
            auto circuit0 = mqis::QuantumCircuit {2};
            auto circuit1 = mqis::QuantumCircuit {3};

            SECTION("empty")
            {
                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }

            SECTION("add_x_gate()")
            {
                circuit0.add_x_gate(0);
                circuit1.add_x_gate(0);
                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }
        }

        SECTION("different measure bitmasks")
        {
            auto circuit0 = mqis::QuantumCircuit {2};
            auto circuit1 = mqis::QuantumCircuit {2};

            SECTION("one circuit has measurements")
            {
                circuit0.add_m_gate(0);
                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }

            SECTION("both circuits have measurements on different qubits")
            {
                circuit0.add_m_gate(0);
                circuit0.add_m_gate(1);
                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }
        }

        SECTION("different number of gates")
        {
            auto circuit0 = mqis::QuantumCircuit {2};
            auto circuit1 = mqis::QuantumCircuit {2};

            circuit0.add_x_gate(0);
            circuit1.add_x_gate({0, 1});

            REQUIRE(!mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("different gates")
        {
            auto circuit0 = mqis::QuantumCircuit {2};
            auto circuit1 = mqis::QuantumCircuit {2};

            circuit0.add_x_gate(0);
            circuit1.add_h_gate(0);

            REQUIRE(!mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("different values on same gate")
        {
            auto circuit0 = mqis::QuantumCircuit {2};
            auto circuit1 = mqis::QuantumCircuit {2};

            SECTION("add_x_gate()")
            {
                circuit0.add_x_gate(0);
                circuit1.add_x_gate(1);

                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }

            SECTION("add_rx_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_rx_gate(M_PI, 0);
                    circuit1.add_rx_gate(M_PI_2, 0);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_rx_gate(M_PI_2, 0);
                    circuit1.add_rx_gate(M_PI_2, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_h_gate()")
            {
                circuit0.add_h_gate(0);
                circuit1.add_h_gate(1);

                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }

            SECTION("add_cx_gate()")
            {
                circuit0.add_cx_gate(0, 1);
                circuit1.add_cx_gate(1, 0);

                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }

            SECTION("add_crx_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_crx_gate(M_PI_4, 0, 1);
                    circuit1.add_crx_gate(M_PI_2, 0, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_crx_gate(M_PI_2, 1, 0);
                    circuit1.add_crx_gate(M_PI_2, 0, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_cp_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_cp_gate(M_PI_4, 0, 1);
                    circuit1.add_cp_gate(M_PI_2, 0, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_cp_gate(M_PI_2, 1, 0);
                    circuit1.add_cp_gate(M_PI_2, 0, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_u_gate()")
            {
                SECTION("different gate")
                {
                    circuit0.add_u_gate(mqis::x_gate(), 0);
                    circuit1.add_u_gate(mqis::y_gate(), 0);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_u_gate(mqis::x_gate(), 0);
                    circuit1.add_u_gate(mqis::x_gate(), 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_cu_gate()")
            {
                SECTION("different gate")
                {
                    circuit0.add_cu_gate(mqis::x_gate(), 0, 1);
                    circuit1.add_cu_gate(mqis::y_gate(), 0, 1);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_cu_gate(mqis::x_gate(), 0, 1);
                    circuit1.add_cu_gate(mqis::x_gate(), 1, 0);

                    REQUIRE(!mqis::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_m_gate()")
            {
                circuit0.add_m_gate(0);
                circuit1.add_m_gate(1);

                REQUIRE(!mqis::almost_eq(circuit0, circuit1));
            }
        }
    }

    SECTION("completely identical")
    {
        auto circuit0 = mqis::QuantumCircuit {2};
        auto circuit1 = mqis::QuantumCircuit {2};

        SECTION("empty")
        {
            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_x_gate()")
        {
            circuit0.add_x_gate(0);
            circuit1.add_x_gate(0);
            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_rx_gate()")
        {
            circuit0.add_rx_gate(M_PI, 1);
            circuit1.add_rx_gate(M_PI, 1);
            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_h_gate()")
        {
            circuit0.add_h_gate(0);
            circuit1.add_h_gate(0);
            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cx_gate()")
        {
            circuit0.add_cx_gate(0, 1);
            circuit1.add_cx_gate(0, 1);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_crx_gate()")
        {
            circuit0.add_crx_gate(M_PI_4, 0, 1);
            circuit1.add_crx_gate(M_PI_4, 0, 1);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cp_gate()")
        {
            circuit0.add_cp_gate(M_PI_4, 1, 0);
            circuit1.add_cp_gate(M_PI_4, 1, 0);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_u_gate()")
        {
            circuit0.add_u_gate(mqis::x_gate(), 0);
            circuit1.add_u_gate(mqis::x_gate(), 0);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cu_gate()")
        {
            circuit0.add_cu_gate(mqis::y_gate(), 0, 1);
            circuit1.add_cu_gate(mqis::y_gate(), 0, 1);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_m_gate()")
        {
            circuit0.add_m_gate(0);
            circuit1.add_m_gate(0);

            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }

        SECTION("add_h_gate() and add_x_gate()")
        {
            circuit0.add_h_gate(0);
            circuit0.add_x_gate(1);
            circuit1.add_h_gate(0);
            circuit1.add_x_gate(1);
            REQUIRE(mqis::almost_eq(circuit0, circuit1));
        }
    }
}

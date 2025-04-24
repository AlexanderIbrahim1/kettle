#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"

static constexpr auto ABS_TOL = double {1.0e-6};


TEST_CASE("add multiple X gates")
{
    using G = ket::Gate;
    namespace comp = impl_ket::compare;

    auto circuit = ket::QuantumCircuit {3};

    const auto number_of_elements = [](const ket::QuantumCircuit& circ)
    {
        return std::distance(circ.begin(), circ.end());
    };

    SECTION("add 1")
    {
        const auto indices = std::vector<std::size_t> {1};
        circuit.add_x_gate(indices);

        const auto expected0 = impl_ket::create_one_target_gate(G::X, 1);

        REQUIRE(number_of_elements(circuit) == 1);
        REQUIRE(comp::is_1t_gate_equal(circuit[0].get_gate(), expected0));
    }

    SECTION("add 0, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 2};
        circuit.add_x_gate(indices);

        const auto expected0 = impl_ket::create_one_target_gate(G::X, 0);
        const auto expected1 = impl_ket::create_one_target_gate(G::X, 2);

        REQUIRE(number_of_elements(circuit) == 2);
        REQUIRE(comp::is_1t_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1t_gate_equal(circuit[1].get_gate(), expected1));
    }

    SECTION("add 0, 1, 2")
    {
        const auto indices = std::vector<std::size_t> {0, 1, 2};
        circuit.add_x_gate(indices);

        const auto expected0 = impl_ket::create_one_target_gate(G::X, 0);
        const auto expected1 = impl_ket::create_one_target_gate(G::X, 1);
        const auto expected2 = impl_ket::create_one_target_gate(G::X, 2);

        REQUIRE(number_of_elements(circuit) == 3);
        REQUIRE(comp::is_1t_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1t_gate_equal(circuit[1].get_gate(), expected1));
        REQUIRE(comp::is_1t_gate_equal(circuit[2].get_gate(), expected2));
    }

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_x_gate({0, 1, 2});

        const auto expected0 = impl_ket::create_one_target_gate(G::X, 0);
        const auto expected1 = impl_ket::create_one_target_gate(G::X, 1);
        const auto expected2 = impl_ket::create_one_target_gate(G::X, 2);

        REQUIRE(number_of_elements(circuit) == 3);
        REQUIRE(comp::is_1t_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1t_gate_equal(circuit[1].get_gate(), expected1));
        REQUIRE(comp::is_1t_gate_equal(circuit[2].get_gate(), expected2));
    }
}

TEST_CASE("add multiple RX gates")
{
    using G = ket::Gate;
    namespace comp = impl_ket::compare;

    auto circuit = ket::QuantumCircuit {3};

    const auto number_of_elements = [](const ket::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_rx_gate({{0, 0.25}, {1, 0.5}, {2, 0.75}});

        const auto expected0 = impl_ket::create_one_target_one_angle_gate(G::RX, 0, 0.25);
        const auto expected1 = impl_ket::create_one_target_one_angle_gate(G::RX, 1, 0.50);
        const auto expected2 = impl_ket::create_one_target_one_angle_gate(G::RX, 2, 0.75);

        REQUIRE(number_of_elements(circuit) == 3);
        REQUIRE(comp::is_1t1a_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1t1a_gate_equal(circuit[1].get_gate(), expected1));
        REQUIRE(comp::is_1t1a_gate_equal(circuit[2].get_gate(), expected2));
//
//        const auto rx_gate0 = impl_ket::unpack_one_target_one_angle_gate(circuit[0]);
//        REQUIRE(std::get<0>(rx_gate0) == 0);
//        REQUIRE_THAT(std::get<1>(rx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
//        REQUIRE(circuit[0].gate == ket::Gate::RX);
//
//        const auto rx_gate1 = impl_ket::unpack_one_target_one_angle_gate(circuit[1]);
//        REQUIRE(std::get<0>(rx_gate1) == 1);
//        REQUIRE_THAT(std::get<1>(rx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
//        REQUIRE(circuit[1].gate == ket::Gate::RX);
//
//        const auto rx_gate2 = impl_ket::unpack_one_target_one_angle_gate(circuit[2]);
//        REQUIRE(std::get<0>(rx_gate2) == 2);
//        REQUIRE_THAT(std::get<1>(rx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
//        REQUIRE(circuit[2].gate == ket::Gate::RX);
    }
}

TEST_CASE("add multiple CX gates")
{
    using G = ket::Gate;
    namespace comp = impl_ket::compare;

    auto circuit = ket::QuantumCircuit {3};

    const auto number_of_elements = [](const ket::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_cx_gate({{0, 1}, {1, 2}, {2, 0}});

        const auto expected0 = impl_ket::create_one_control_one_target_gate(G::CX, 0, 1);
        const auto expected1 = impl_ket::create_one_control_one_target_gate(G::CX, 1, 2);
        const auto expected2 = impl_ket::create_one_control_one_target_gate(G::CX, 2, 0);

        REQUIRE(number_of_elements(circuit) == 3);
        REQUIRE(comp::is_1c1t_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1c1t_gate_equal(circuit[1].get_gate(), expected1));
        REQUIRE(comp::is_1c1t_gate_equal(circuit[2].get_gate(), expected2));
//
//        const auto cx_gate0 = impl_ket::unpack_one_control_one_target_gate(circuit[0]);
//        REQUIRE(std::get<0>(cx_gate0) == 0);
//        REQUIRE(std::get<1>(cx_gate0) == 1);
//        REQUIRE(circuit[0].gate == ket::Gate::CX);
//
//        const auto cx_gate1 = impl_ket::unpack_one_control_one_target_gate(circuit[1]);
//        REQUIRE(std::get<0>(cx_gate1) == 1);
//        REQUIRE(std::get<1>(cx_gate1) == 2);
//        REQUIRE(circuit[1].gate == ket::Gate::CX);
//
//        const auto cx_gate2 = impl_ket::unpack_one_control_one_target_gate(circuit[2]);
//        REQUIRE(std::get<0>(cx_gate2) == 2);
//        REQUIRE(std::get<1>(cx_gate2) == 0);
//        REQUIRE(circuit[2].gate == ket::Gate::CX);
    }
}

TEST_CASE("add multiple CRX gates")
{
    using G = ket::Gate;
    namespace comp = impl_ket::compare;

    auto circuit = ket::QuantumCircuit {3};

    const auto number_of_elements = [](const ket::QuantumCircuit& circ)
    { return std::distance(circ.begin(), circ.end()); };

    SECTION("add 0, 1, 2 via initializer list")
    {
        circuit.add_crx_gate({{0, 1, 0.25}, {1, 2, 0.5}, {2, 0, 0.75}});

        const auto expected0 = impl_ket::create_one_control_one_target_one_angle_gate(G::CRX, 0, 1, 0.25);
        const auto expected1 = impl_ket::create_one_control_one_target_one_angle_gate(G::CRX, 1, 2, 0.50);
        const auto expected2 = impl_ket::create_one_control_one_target_one_angle_gate(G::CRX, 2, 0, 0.75);

        REQUIRE(number_of_elements(circuit) == 3);
        REQUIRE(comp::is_1c1t1a_gate_equal(circuit[0].get_gate(), expected0));
        REQUIRE(comp::is_1c1t1a_gate_equal(circuit[1].get_gate(), expected1));
        REQUIRE(comp::is_1c1t1a_gate_equal(circuit[2].get_gate(), expected2));
//
//        REQUIRE(number_of_elements(circuit) == 3);
//
//        const auto crx_gate0 = impl_ket::unpack_one_control_one_target_one_angle_gate(circuit[0]);
//        REQUIRE(std::get<0>(crx_gate0) == 0);
//        REQUIRE(std::get<1>(crx_gate0) == 1);
//        REQUIRE_THAT(std::get<2>(crx_gate0), Catch::Matchers::WithinAbs(0.25, ABS_TOL));
//        REQUIRE(circuit[0].gate == ket::Gate::CRX);
//
//        const auto crx_gate1 = impl_ket::unpack_one_control_one_target_one_angle_gate(circuit[1]);
//        REQUIRE(std::get<0>(crx_gate1) == 1);
//        REQUIRE(std::get<1>(crx_gate1) == 2);
//        REQUIRE_THAT(std::get<2>(crx_gate1), Catch::Matchers::WithinAbs(0.5, ABS_TOL));
//        REQUIRE(circuit[1].gate == ket::Gate::CRX);
//
//        const auto crx_gate2 = impl_ket::unpack_one_control_one_target_one_angle_gate(circuit[2]);
//        REQUIRE(std::get<0>(crx_gate2) == 2);
//        REQUIRE(std::get<1>(crx_gate2) == 0);
//        REQUIRE_THAT(std::get<2>(crx_gate2), Catch::Matchers::WithinAbs(0.75, ABS_TOL));
//        REQUIRE(circuit[2].gate == ket::Gate::CRX);
    }
}

TEST_CASE("QuantumCircuit: almost_eq()")
{
    SECTION("not equals")
    {
        SECTION("different number of qubits")
        {
            auto circuit0 = ket::QuantumCircuit {2};
            auto circuit1 = ket::QuantumCircuit {3};

            SECTION("empty")
            {
                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }

            SECTION("add_x_gate()")
            {
                circuit0.add_x_gate(0);
                circuit1.add_x_gate(0);
                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }
        }

        SECTION("different measure bitmasks")
        {
            auto circuit0 = ket::QuantumCircuit {2};
            auto circuit1 = ket::QuantumCircuit {2};

            SECTION("one circuit has measurements")
            {
                circuit0.add_m_gate(0);
                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }

            SECTION("both circuits have measurements on different qubits")
            {
                circuit0.add_m_gate(0);
                circuit0.add_m_gate(1);
                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }
        }

        SECTION("different number of gates")
        {
            auto circuit0 = ket::QuantumCircuit {2};
            auto circuit1 = ket::QuantumCircuit {2};

            circuit0.add_x_gate(0);
            circuit1.add_x_gate({0, 1});

            REQUIRE(!ket::almost_eq(circuit0, circuit1));
        }

        SECTION("different gates")
        {
            auto circuit0 = ket::QuantumCircuit {2};
            auto circuit1 = ket::QuantumCircuit {2};

            circuit0.add_x_gate(0);
            circuit1.add_h_gate(0);

            REQUIRE(!ket::almost_eq(circuit0, circuit1));
        }

        SECTION("different values on same gate")
        {
            auto circuit0 = ket::QuantumCircuit {2};
            auto circuit1 = ket::QuantumCircuit {2};

            SECTION("add_x_gate()")
            {
                circuit0.add_x_gate(0);
                circuit1.add_x_gate(1);

                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }

            SECTION("add_rx_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_rx_gate(0, M_PI);
                    circuit1.add_rx_gate(0, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_rx_gate(0, M_PI_2);
                    circuit1.add_rx_gate(1, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_h_gate()")
            {
                circuit0.add_h_gate(0);
                circuit1.add_h_gate(1);

                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }

            SECTION("add_cx_gate()")
            {
                circuit0.add_cx_gate(0, 1);
                circuit1.add_cx_gate(1, 0);

                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }

            SECTION("add_crx_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_crx_gate(0, 1, M_PI_4);
                    circuit1.add_crx_gate(0, 1, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_crx_gate(1, 0, M_PI_2);
                    circuit1.add_crx_gate(0, 1, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_cp_gate()")
            {
                SECTION("different angle")
                {
                    circuit0.add_cp_gate(0, 1, M_PI_4);
                    circuit1.add_cp_gate(0, 1, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_cp_gate(1, 0, M_PI_2);
                    circuit1.add_cp_gate(0, 1, M_PI_2);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_u_gate()")
            {
                SECTION("different gate")
                {
                    circuit0.add_u_gate(ket::x_gate(), 0);
                    circuit1.add_u_gate(ket::y_gate(), 0);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_u_gate(ket::x_gate(), 0);
                    circuit1.add_u_gate(ket::x_gate(), 1);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_cu_gate()")
            {
                SECTION("different gate")
                {
                    circuit0.add_cu_gate(ket::x_gate(), 0, 1);
                    circuit1.add_cu_gate(ket::y_gate(), 0, 1);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }

                SECTION("different qubit")
                {
                    circuit0.add_cu_gate(ket::x_gate(), 0, 1);
                    circuit1.add_cu_gate(ket::x_gate(), 1, 0);

                    REQUIRE(!ket::almost_eq(circuit0, circuit1));
                }
            }

            SECTION("add_m_gate()")
            {
                circuit0.add_m_gate(0);
                circuit1.add_m_gate(1);

                REQUIRE(!ket::almost_eq(circuit0, circuit1));
            }
        }
    }

    SECTION("completely identical")
    {
        auto circuit0 = ket::QuantumCircuit {2};
        auto circuit1 = ket::QuantumCircuit {2};

        SECTION("empty")
        {
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_x_gate()")
        {
            circuit0.add_x_gate(0);
            circuit1.add_x_gate(0);
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_rx_gate()")
        {
            circuit0.add_rx_gate(1, M_PI);
            circuit1.add_rx_gate(1, M_PI);
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_h_gate()")
        {
            circuit0.add_h_gate(0);
            circuit1.add_h_gate(0);
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cx_gate()")
        {
            circuit0.add_cx_gate(0, 1);
            circuit1.add_cx_gate(0, 1);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_crx_gate()")
        {
            circuit0.add_crx_gate(0, 1, M_PI_4);
            circuit1.add_crx_gate(0, 1, M_PI_4);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cp_gate()")
        {
            circuit0.add_cp_gate(1, 0, M_PI_4);
            circuit1.add_cp_gate(1, 0, M_PI_4);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_u_gate()")
        {
            circuit0.add_u_gate(ket::x_gate(), 0);
            circuit1.add_u_gate(ket::x_gate(), 0);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_cu_gate()")
        {
            circuit0.add_cu_gate(ket::y_gate(), 0, 1);
            circuit1.add_cu_gate(ket::y_gate(), 0, 1);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_m_gate()")
        {
            circuit0.add_m_gate(0);
            circuit1.add_m_gate(0);

            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }

        SECTION("add_h_gate() and add_x_gate()")
        {
            circuit0.add_h_gate(0);
            circuit0.add_x_gate(1);
            circuit1.add_h_gate(0);
            circuit1.add_x_gate(1);
            REQUIRE(ket::almost_eq(circuit0, circuit1));
        }
    }
}

TEST_CASE("CircuitElement")
{
    SECTION("construct with ket::GateInfo")
    {
        const auto ginfo = impl_ket::create_one_target_gate(ket::Gate::X, 0);
        const auto circuit_element = impl_ket::CircuitElement {ginfo};

        REQUIRE(circuit_element.is_gate());
        REQUIRE(!circuit_element.is_control_flow());

        const auto gate_from_circuit_element = circuit_element.get_gate();
        REQUIRE(gate_from_circuit_element.gate == ginfo.gate);
        REQUIRE(impl_ket::unpack_single_qubit_gate_index(gate_from_circuit_element) == 0);
    }

//    // TODO reimplement
//    SECTION("construct with ControlFlowInstruction")
//    {
//        auto cfi = impl_ket::ControlFlowInstruction {
//            impl_ket::SingleBitControlFlowFunction {0, impl_ket::ControlBooleanKind::IF},
//            nullptr
//        };
//
//        const auto circuit_element = impl_ket::CircuitElement {std::move(cfi)};
//
//        REQUIRE(circuit_element.is_control_flow());
//        REQUIRE(!circuit_element.is_gate());
//        REQUIRE_NOTHROW(circuit_element.get_control_flow());
//    }
}

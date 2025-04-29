#include <functional>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <kettle/circuit_operations/compare_circuits.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/io/read_tangelo_file.hpp>
#include <kettle/io/write_tangelo_file.hpp>


static auto number_of_elements(const ket::QuantumCircuit& circuit) -> std::size_t
{
    return static_cast<std::size_t>(std::distance(circuit.begin(), circuit.end()));
}


TEST_CASE("read_tangelo_file()")
{
    using G = ket::Gate;
    namespace comp = impl_ket::compare;

    SECTION("single h gate")
    {
        auto stream = std::stringstream {
            "H         target : [4]   \n"
        };

        const auto actual = ket::read_tangelo_circuit(9, stream, 0);
        const auto expected = impl_ket::create_one_target_gate(G::H, 4);

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(comp::is_1t_gate_equal(actual[0].get_gate(), expected));
    }

    SECTION("multiple gates")
    {
        auto stream = std::stringstream {
            "Circuit object. Size 339200                            \n"
            "                                                       \n"
            "H         target : [4]                                 \n"
            "RX        target : [5]   parameter : 1.5707963267948966\n"
            "CNOT      target : [4]   control : [2]                 \n"
            "RZ        target : [5]   parameter : 12.533816585267923\n"
        };

        const auto actual = ket::read_tangelo_circuit(9, stream, 2);

        const auto expected0 = impl_ket::create_one_target_gate(G::H, 4);
        const auto expected1 = impl_ket::create_one_target_one_angle_gate(G::RX, 5, 1.5707963267948966);
        const auto expected2 = impl_ket::create_one_control_one_target_gate(G::CX, 2, 4);
        const auto expected3 = impl_ket::create_one_target_one_angle_gate(G::RZ, 5, 12.533816585267923);

        REQUIRE(number_of_elements(actual) == 4);
        REQUIRE(comp::is_1t_gate_equal(actual[0].get_gate(), expected0));
        REQUIRE(comp::is_1t1a_gate_equal(actual[1].get_gate(), expected1));
        REQUIRE(comp::is_1c1t_gate_equal(actual[2].get_gate(), expected2));
        REQUIRE(comp::is_1t1a_gate_equal(actual[3].get_gate(), expected3));
    }

    SECTION("single SWAP gate")
    {
        auto stream = std::stringstream {
            "SWAP      target : [12, 9]\n"
        };

        const auto actual = ket::read_tangelo_circuit(13, stream, 0);
        const auto gate0 = actual[0].get_gate();
        const auto gate1 = actual[1].get_gate();
        const auto gate2 = actual[2].get_gate();

        REQUIRE(number_of_elements(actual) == 3);
        REQUIRE(gate0.gate == ket::Gate::CX);
        REQUIRE(gate1.gate == ket::Gate::CX);
        REQUIRE(gate2.gate == ket::Gate::CX);

        const auto [q_left_0, q_right_0] = impl_ket::unpack_one_control_one_target_gate(gate0);
        const auto [q_left_1, q_right_1] = impl_ket::unpack_one_control_one_target_gate(gate1);
        const auto [q_left_2, q_right_2] = impl_ket::unpack_one_control_one_target_gate(gate2);

        REQUIRE(q_left_0 == q_right_1);
        REQUIRE(q_right_1 == q_left_2);
        REQUIRE(q_right_0 == q_left_1);
        REQUIRE(q_left_1 == q_right_2);
    }

    SECTION("single PHASE gate")
    {
        auto stream = std::stringstream {
            "PHASE     target : [11]   parameter : -1.3474016644659843\n"
        };

        const auto actual = ket::read_tangelo_circuit(13, stream, 0);
        const auto expected0 = impl_ket::create_one_target_one_angle_gate(G::P, 11, -1.3474016644659843);

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(comp::is_1t1a_gate_equal(actual[0].get_gate(), expected0));
    }

    SECTION("parse_one_control_one_target_one_angle_gate()")
    {
        struct TestCase
        {
            std::string stream_contents;
            ket::Gate gate;
            std::function<std::tuple<std::size_t, std::size_t, double>(ket::GateInfo)> unpack_func;
        };

        const auto unpack = impl_ket::unpack_one_control_one_target_one_angle_gate;
        const auto create = impl_ket::create_one_control_one_target_one_angle_gate;

        auto testcase = GENERATE_REF(
            TestCase {
                "CPHASE    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CP,
                unpack
            },
            TestCase {
                "CRX    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRX,
                unpack
            },
            TestCase {
                "CRY    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRY,
                unpack
            },
            TestCase {
                "CRZ    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRZ,
                unpack
            }
        );

        auto stream = std::stringstream {testcase.stream_contents};

        const auto actual = ket::read_tangelo_circuit(13, stream, 0);
        const auto expected = create(testcase.gate, 12, 9, -0.39269908169872414);

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(comp::is_1c1t1a_gate_equal(actual[0].get_gate(), expected));
    }

    SECTION("parse_m_gate()")
    {
        auto stream = std::stringstream {"M         target : [1]   bit : [4]\n"};
        const auto actual = ket::read_tangelo_circuit(13, stream, 0);
        const auto expected = impl_ket::create_m_gate(1, 4);

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(comp::is_m_gate_equal(actual[0].get_gate(), expected));
    }

    SECTION("parse_u_gate()")
    {
        auto stream = std::stringstream {
            "U         target : [1]\n"
            "    [1.234, -4.321]   [2.345, -5.432]\n"
            "    [3.456, -6.543]   [4.567, -7.654]\n"
        };
        const auto actual = ket::read_tangelo_circuit(10, stream, 0);
        const auto gate = actual[0].get_gate();

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(gate.gate == ket::Gate::U);

        const auto [target, unitary_ptr] = impl_ket::unpack_u_gate(gate);
        
        const auto expected_matrix = ket::Matrix2X2 {
            .elem00={1.234, -4.321},
            .elem01={2.345, -5.432},
            .elem10={3.456, -6.543},
            .elem11={4.567, -7.654}
        };

        REQUIRE(target == 1);
        REQUIRE(ket::almost_eq(*unitary_ptr, expected_matrix));
    }

    SECTION("parse_cu_gate()")
    {
        auto stream = std::stringstream {
            "CU        target : [1]   control : [2]\n"
            "    [1.234, -4.321]   [2.345, -5.432]\n"
            "    [3.456, -6.543]   [4.567, -7.654]\n"
        };
        const auto actual = ket::read_tangelo_circuit(10, stream, 0);
        const auto gate = actual[0].get_gate();

        REQUIRE(number_of_elements(actual) == 1);
        REQUIRE(gate.gate == ket::Gate::CU);

        const auto [control, target, unitary_ptr] = impl_ket::unpack_cu_gate(gate);
        
        const auto expected_matrix = ket::Matrix2X2 {
            .elem00={1.234, -4.321},
            .elem01={2.345, -5.432},
            .elem10={3.456, -6.543},
            .elem11={4.567, -7.654}
        };

        REQUIRE(target == 1);
        REQUIRE(control == 2);
        REQUIRE(ket::almost_eq(*unitary_ptr, expected_matrix));
    }
}

TEST_CASE("read_tangelo_file() with control flow")
{
    const auto x_and_x_subcircuit = []()
    {
        auto circ = ket::QuantumCircuit {3};
        circ.add_x_gate({0, 2});
        return circ;
    };

    const auto x_and_h_subcircuit = []()
    {
        auto circ = ket::QuantumCircuit {3};
        circ.add_x_gate(1);
        circ.add_h_gate(2);
        return circ;
    };

    const auto cx_and_h_subcircuit = []()
    {
        auto circ = ket::QuantumCircuit {3};
        circ.add_cx_gate(1, 2);
        circ.add_h_gate(2);
        return circ;
    };

    // this circuit was taken directly from one of the user-facing examples
    const auto original = [&]() {
        auto circuit = ket::QuantumCircuit {3};
        circuit.add_x_gate({0, 1});
        circuit.add_h_gate({0, 1, 2});

        circuit.add_m_gate({0, 1});

        circuit.add_if_statement(0, x_and_x_subcircuit());

        circuit.add_y_gate(0);
        circuit.add_z_gate(1);

        circuit.add_if_else_statement(1, x_and_h_subcircuit(), cx_and_h_subcircuit());

        circuit.add_y_gate(0);
        circuit.add_z_gate(1);

        return circuit;
    }();

    auto sstream = std::stringstream {};
    ket::write_tangelo_circuit(original, sstream);

    const auto reconstructed = ket::read_tangelo_circuit(3, sstream, 0);

    REQUIRE(ket::almost_eq(original, reconstructed));
}

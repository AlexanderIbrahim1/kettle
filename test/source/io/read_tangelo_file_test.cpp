#include <functional>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <kettle/gates/primitive_gate.hpp>
#include <kettle/io/read_tangelo_file.hpp>


TEST_CASE("read_tangelo_file()")
{
    SECTION("single h gate")
    {
        auto stream = std::stringstream {
            "H         target : [4]   \n"
        };

        const auto actual = ket::read_tangelo_circuit(9, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == ket::Gate::H);

        const auto target_qubit = impl_ket::unpack_one_target_gate(actual[0]);
        REQUIRE(target_qubit == 4);
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

        REQUIRE(std::distance(actual.begin(), actual.end()) == 4);

        SECTION("H gate")
        {
            REQUIRE(actual[0].gate == ket::Gate::H);
            const auto target_qubit = impl_ket::unpack_one_target_gate(actual[0]);
            REQUIRE(target_qubit == 4);
        }

        SECTION("RX gate")
        {
            REQUIRE(actual[1].gate == ket::Gate::RX);
            const auto [target_qubit, angle] = impl_ket::unpack_one_target_one_angle_gate(actual[1]);
            REQUIRE(target_qubit == 5);
            REQUIRE_THAT(angle, Catch::Matchers::WithinRel(1.5707963267948966));
        }

        SECTION("CX gate")
        {
            REQUIRE(actual[2].gate == ket::Gate::CX);
            const auto [control_qubit, target_qubit] = impl_ket::unpack_one_control_one_target_gate(actual[2]);
            REQUIRE(target_qubit == 4);
            REQUIRE(control_qubit == 2);
        }

        SECTION("RZ gate")
        {
            REQUIRE(actual[3].gate == ket::Gate::RZ);
            const auto [target_qubit, angle] = impl_ket::unpack_one_target_one_angle_gate(actual[3]);
            REQUIRE(target_qubit == 5);
            REQUIRE_THAT(angle, Catch::Matchers::WithinRel(12.533816585267923));
        }
    }

    SECTION("single SWAP gate")
    {
        auto stream = std::stringstream {
            "SWAP      target : [12, 9]\n"
        };

        const auto actual = ket::read_tangelo_circuit(13, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 3);
        REQUIRE(actual[0].gate == ket::Gate::CX);
        REQUIRE(actual[1].gate == ket::Gate::CX);
        REQUIRE(actual[2].gate == ket::Gate::CX);

        const auto [q_left_0, q_right_0] = impl_ket::unpack_one_control_one_target_gate(actual[0]);
        const auto [q_left_1, q_right_1] = impl_ket::unpack_one_control_one_target_gate(actual[1]);
        const auto [q_left_2, q_right_2] = impl_ket::unpack_one_control_one_target_gate(actual[2]);

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

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == ket::Gate::P);

        const auto [target_qubit, angle] = impl_ket::unpack_one_target_one_angle_gate(actual[0]);
        REQUIRE(target_qubit == 11);
        REQUIRE_THAT(angle, Catch::Matchers::WithinRel(-1.3474016644659843));
    }

    SECTION("parse_one_target_one_control_one_angle_gate()")
    {
        struct TestCase
        {
            std::string stream_contents;
            ket::Gate gate;
            std::function<std::tuple<std::size_t, std::size_t, double>(ket::GateInfo)> unpack_func;
        };

        auto testcase = GENERATE_REF(
            TestCase {
                "CPHASE    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CP,
                impl_ket::unpack_one_control_one_target_one_angle_gate
            },
            TestCase {
                "CRX    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRX,
                impl_ket::unpack_one_control_one_target_one_angle_gate
            },
            TestCase {
                "CRY    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRY,
                impl_ket::unpack_one_control_one_target_one_angle_gate
            },
            TestCase {
                "CRZ    target : [9]   control : [12]   parameter : -0.39269908169872414\n",
                ket::Gate::CRZ,
                impl_ket::unpack_one_control_one_target_one_angle_gate
            }
        );

        auto stream = std::stringstream {testcase.stream_contents};
        const auto actual = ket::read_tangelo_circuit(13, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == testcase.gate);

        const auto [control_qubit, target_qubit, angle] = testcase.unpack_func(actual[0]);
        REQUIRE(control_qubit == 12);
        REQUIRE(target_qubit == 9);
        REQUIRE_THAT(angle, Catch::Matchers::WithinRel(-0.39269908169872414));
    }

    SECTION("parse_m_gate()")
    {
        auto stream = std::stringstream {"M         target : [1]   bit : [4]\n"};
        const auto actual = ket::read_tangelo_circuit(13, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == ket::Gate::M);

        const auto [qubit, bit] = impl_ket::unpack_m_gate(actual[0]);
        REQUIRE(qubit == 1);
        REQUIRE(bit == 4);
    }

    SECTION("parse_u_gate()")
    {
        auto stream = std::stringstream {
            "U         target : [1]\n"
            "    [1.234, -4.321]   [2.345, -5.432]\n"
            "    [3.456, -6.543]   [4.567, -7.654]\n"
        };
        const auto actual = ket::read_tangelo_circuit(10, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == ket::Gate::U);

        const auto [target, matrix_index] = impl_ket::unpack_u_gate(actual[0]);
        const auto matrix = actual.unitary_gate(matrix_index);
        
        const auto expected_matrix = ket::Matrix2X2 {
            {1.234, -4.321},
            {2.345, -5.432},
            {3.456, -6.543},
            {4.567, -7.654}
        };

        REQUIRE(target == 1);
        REQUIRE(ket::almost_eq(matrix, expected_matrix));
    }

    SECTION("parse_cu_gate()")
    {
        auto stream = std::stringstream {
            "CU        target : [1]   control : [2]\n"
            "    [1.234, -4.321]   [2.345, -5.432]\n"
            "    [3.456, -6.543]   [4.567, -7.654]\n"
        };
        const auto actual = ket::read_tangelo_circuit(10, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == ket::Gate::CU);

        const auto [control, target, matrix_index] = impl_ket::unpack_cu_gate(actual[0]);
        const auto matrix = actual.unitary_gate(matrix_index);
        
        const auto expected_matrix = ket::Matrix2X2 {
            {1.234, -4.321},
            {2.345, -5.432},
            {3.456, -6.543},
            {4.567, -7.654}
        };

        REQUIRE(target == 1);
        REQUIRE(control == 2);
        REQUIRE(ket::almost_eq(matrix, expected_matrix));
    }
}

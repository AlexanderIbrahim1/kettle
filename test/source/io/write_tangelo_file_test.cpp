#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/io/write_tangelo_file.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/io/write_tangelo_file_internal.hpp"

namespace cre = ket::internal::create;


TEST_CASE("format_one_target_gate_()")
{
    using G = ket::Gate;

    struct TestCase
    {
        ket::Gate gate;
        std::size_t target;
        std::string expected;
    };

    const auto testcase = GENERATE(
        TestCase {G::H , 5, "H         target : [5]\n"},
        TestCase {G::X , 1, "X         target : [1]\n"},
        TestCase {G::Y , 2, "Y         target : [2]\n"},
        TestCase {G::Z , 3, "Z         target : [3]\n"},
        TestCase {G::SX, 0, "SX        target : [0]\n"}
    );

    const auto gate_info = cre::create_one_target_gate(testcase.gate, testcase.target);
    const auto actual = ket::internal::format_one_target_gate_(gate_info);

    REQUIRE(actual == testcase.expected);
}

TEST_CASE("format_one_control_one_target_gate_()")
{
    using G = ket::Gate;

    struct TestCase
    {
        ket::Gate gate;
        std::size_t target;
        std::size_t control;
        std::string expected;
    };

    const auto testcase = GENERATE(
        TestCase {G::CH , 5, 3, "CH        target : [5]   control : [3]\n"},
        TestCase {G::CX , 1, 2, "CX        target : [1]   control : [2]\n"},
        TestCase {G::CY , 2, 0, "CY        target : [2]   control : [0]\n"},
        TestCase {G::CZ , 3, 1, "CZ        target : [3]   control : [1]\n"},
        TestCase {G::CSX, 0, 3, "CSX       target : [0]   control : [3]\n"}
    );

    const auto gate_info = cre::create_one_control_one_target_gate(
        testcase.gate,
        testcase.control,
        testcase.target
    );

    const auto actual = ket::internal::format_one_control_one_target_gate_(gate_info);

    REQUIRE(actual == testcase.expected);
}

TEST_CASE("format_one_target_one_angle_gate_()")
{
    using G = ket::Gate;

    struct TestCase
    {
        ket::Gate gate;
        std::size_t target;
        double angle;
        std::string expected;
    };

    const auto value = 1.1234567812345677;

    // NOTE: the floating-point values here are a little finnicky; don't change them
    const auto testcase = GENERATE_COPY(
        TestCase {G::RX, 1,  value, "RX        target : [1]   parameter : 1.1234567812345677\n"},
        TestCase {G::RY, 2,  value, "RY        target : [2]   parameter : 1.1234567812345677\n"},
        TestCase {G::RZ, 3,  value, "RZ        target : [3]   parameter : 1.1234567812345677\n"},
        TestCase {G::P,  0, -value, "P         target : [0]   parameter : -1.1234567812345677\n"}
    );

    const auto gate_info = cre::create_one_target_one_angle_gate(
        testcase.gate,
        testcase.target,
        testcase.angle
    );

    const auto actual = ket::internal::format_one_target_one_angle_gate_(gate_info);

    REQUIRE(actual == testcase.expected);
}

TEST_CASE("format_one_control_one_target_one_angle_gate_()")
{
    using G = ket::Gate;

    struct TestCase
    {
        ket::Gate gate;
        std::size_t target;
        std::size_t control;
        double angle;
        std::string expected;
    };

    const auto value = 1.1234567812345677;

    // NOTE: the floating-point values here are a little finnicky; don't change them
    const auto testcase = GENERATE_COPY(
        TestCase {G::CRX, 1, 3,  value, "CRX       target : [1]   control : [3]   parameter : 1.1234567812345677\n"},
        TestCase {G::CRY, 2, 2,  value, "CRY       target : [2]   control : [2]   parameter : 1.1234567812345677\n"},
        TestCase {G::CRZ, 3, 0,  value, "CRZ       target : [3]   control : [0]   parameter : 1.1234567812345677\n"},
        TestCase {G::CP,  0, 1, -value, "CP        target : [0]   control : [1]   parameter : -1.1234567812345677\n"}
    );

    const auto gate_info = cre::create_one_control_one_target_one_angle_gate(
        testcase.gate,
        testcase.control,
        testcase.target,
        testcase.angle
    );

    const auto actual = ket::internal::format_one_control_one_target_one_angle_gate_(gate_info);

    REQUIRE(actual == testcase.expected);
}

TEST_CASE("format_m_gate_()")
{
    using G = ket::Gate;

    struct TestCase
    {
        ket::Gate gate;
        std::size_t qubit;
        std::size_t bit;
        std::string expected;
    };

    const auto testcase = GENERATE_COPY(
        TestCase {G::CRX, 1, 3, "M         target : [1]   bit : [3]\n"},
        TestCase {G::CRY, 2, 2, "M         target : [2]   bit : [2]\n"}
    );

    const auto gate_info = cre::create_m_gate(testcase.qubit, testcase.bit);
    const auto actual = ket::internal::format_m_gate_(gate_info);

    REQUIRE(actual == testcase.expected);
}

TEST_CASE("write_tangelo_file() ignores circuit loggers")
{
    auto without_logger = ket::QuantumCircuit {2};
    without_logger.add_x_gate(0);
    without_logger.add_h_gate(1);

    auto with_logger = ket::QuantumCircuit {2};
    with_logger.add_x_gate(0);
    with_logger.add_classical_register_circuit_logger();
    with_logger.add_h_gate(1);

    auto without_stream = std::stringstream {};
    ket::write_tangelo_circuit(without_logger, without_stream);

    auto with_stream = std::stringstream {};
    ket::write_tangelo_circuit(with_logger, with_stream);

    REQUIRE(without_stream.str() == with_stream.str());
}

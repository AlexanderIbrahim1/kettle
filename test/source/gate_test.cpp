#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/primitive_gate.hpp"

TEST_CASE("Test create and unpack gate functions")
{
    SECTION("X gate")
    {
        const auto qubit_index = std::size_t {2};
        const auto gate_info = impl_mqis::create_one_target_gate<mqis::Gate::X>(qubit_index);
        const auto unpacked_index = impl_mqis::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == qubit_index);
    }

    SECTION("RX gate")
    {
        const auto theta = 1.57;
        const auto qubit_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_one_target_one_angle_gate<mqis::Gate::RX>(theta, qubit_index);
        const auto [unpacked_theta, unpacked_index] = impl_mqis::unpack_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_index == qubit_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("H gate")
    {
        const auto qubit_index = std::size_t {0};
        const auto gate_info = impl_mqis::create_one_target_gate<mqis::Gate::H>(qubit_index);
        const auto unpacked_index = impl_mqis::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == qubit_index);
    }

    SECTION("CX gate")
    {
        const auto source_index = std::size_t {2};
        const auto target_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_cx_gate(source_index, target_index);
        const auto [unpacked_source, unpacked_target] = impl_mqis::unpack_cx_gate(gate_info);

        REQUIRE(unpacked_source == source_index);
        REQUIRE(unpacked_target == target_index);
    }

    SECTION("CRX gate")
    {
        const auto source_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = impl_mqis::create_one_control_one_target_one_angle_gate<mqis::Gate::CRX>(source_index, target_index, theta);
        const auto [unpacked_source, unpacked_target, unpacked_theta] = impl_mqis::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_source == source_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("CP gate")
    {
        const auto source_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = impl_mqis::create_one_control_one_target_one_angle_gate<mqis::Gate::CP>(source_index, target_index, theta);
        const auto [unpacked_source, unpacked_target, unpacked_theta] = impl_mqis::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_source == source_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("M gate")
    {
        const auto qubit_index = std::size_t {6};
        const auto bit_index = std::size_t {7};
        const auto gate_info = impl_mqis::create_m_gate(qubit_index, bit_index);
        const auto [unpacked_qubit, unpacked_bit] = impl_mqis::unpack_m_gate(gate_info);

        REQUIRE(unpacked_qubit == qubit_index);
        REQUIRE(unpacked_bit == bit_index);
    }

    SECTION("U gate")
    {
        const auto qubit_index = std::size_t {0};
        const auto gate_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_u_gate(qubit_index, gate_index);
        const auto [unpacked_qubit_index, unpacked_gate_index] = impl_mqis::unpack_u_gate(gate_info);

        REQUIRE(unpacked_qubit_index == qubit_index);
        REQUIRE(unpacked_gate_index == gate_index);
    }

    SECTION("CU gate")
    {
        const auto source_index = std::size_t {0};
        const auto target_index = std::size_t {1};
        const auto gate_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_cu_gate(source_index, target_index, gate_index);
        const auto [u_source_index, u_target_index, u_gate_index] = impl_mqis::unpack_cu_gate(gate_info);

        REQUIRE(u_source_index == source_index);
        REQUIRE(u_target_index == target_index);
        REQUIRE(u_gate_index == gate_index);
    }
}

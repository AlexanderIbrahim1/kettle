#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/gates/primitive_gate.hpp"

TEST_CASE("Test create and unpack gate functions")
{
    SECTION("X gate")
    {
        const auto target_index = std::size_t {2};
        const auto gate_info = impl_mqis::create_one_target_gate<mqis::Gate::X>(target_index);
        const auto unpacked_index = impl_mqis::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
    }

    SECTION("RX gate")
    {
        const auto theta = 1.57;
        const auto target_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_one_target_one_angle_gate<mqis::Gate::RX>(theta, target_index);
        const auto [unpacked_index, unpacked_theta] = impl_mqis::unpack_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("H gate")
    {
        const auto target_index = std::size_t {0};
        const auto gate_info = impl_mqis::create_one_target_gate<mqis::Gate::H>(target_index);
        const auto unpacked_index = impl_mqis::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
    }

    SECTION("CX gate")
    {
        const auto control_index = std::size_t {2};
        const auto target_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_one_control_one_target_gate<mqis::Gate::CX>(control_index, target_index);
        const auto [unpacked_control, unpacked_target] = impl_mqis::unpack_one_control_one_target_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
    }

    SECTION("CRX gate")
    {
        const auto control_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = impl_mqis::create_one_control_one_target_one_angle_gate<mqis::Gate::CRX>(control_index, target_index, theta);
        const auto [unpacked_control, unpacked_target, unpacked_theta] = impl_mqis::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("CP gate")
    {
        const auto control_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = impl_mqis::create_one_control_one_target_one_angle_gate<mqis::Gate::CP>(control_index, target_index, theta);
        const auto [unpacked_control, unpacked_target, unpacked_theta] = impl_mqis::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("M gate")
    {
        const auto target_index = std::size_t {6};
        const auto bit_index = std::size_t {7};
        const auto gate_info = impl_mqis::create_m_gate(target_index, bit_index);
        const auto [unpacked_qubit, unpacked_bit] = impl_mqis::unpack_m_gate(gate_info);

        REQUIRE(unpacked_qubit == target_index);
        REQUIRE(unpacked_bit == bit_index);
    }

    SECTION("U gate")
    {
        const auto target_index = std::size_t {0};
        const auto gate_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_u_gate(target_index, gate_index);
        const auto [unpacked_target_index, unpacked_gate_index] = impl_mqis::unpack_u_gate(gate_info);

        REQUIRE(unpacked_target_index == target_index);
        REQUIRE(unpacked_gate_index == gate_index);
    }

    SECTION("CU gate")
    {
        const auto control_index = std::size_t {0};
        const auto target_index = std::size_t {1};
        const auto gate_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_cu_gate(control_index, target_index, gate_index);
        const auto [u_control_index, u_target_index, u_gate_index] = impl_mqis::unpack_cu_gate(gate_info);

        REQUIRE(u_control_index == control_index);
        REQUIRE(u_target_index == target_index);
        REQUIRE(u_gate_index == gate_index);
    }
}

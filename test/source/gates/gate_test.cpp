#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/primitive_gate.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"

using G = ket::Gate;
namespace cre = ket::internal::create;

TEST_CASE("Test create and unpack gate functions")
{
    SECTION("X gate")
    {
        const auto target_index = std::size_t {2};
        const auto gate_info = cre::create_one_target_gate(G::X, target_index);
        const auto unpacked_index = cre::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
    }

    SECTION("RX gate")
    {
        const auto theta = 1.57;
        const auto target_index = std::size_t {3};
        const auto gate_info = cre::create_one_target_one_angle_gate(G::RX, target_index, theta);
        const auto [unpacked_index, unpacked_theta] = cre::unpack_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("H gate")
    {
        const auto target_index = std::size_t {0};
        const auto gate_info = cre::create_one_target_gate(G::H, target_index);
        const auto unpacked_index = cre::unpack_one_target_gate(gate_info);

        REQUIRE(unpacked_index == target_index);
    }

    SECTION("CX gate")
    {
        const auto control_index = std::size_t {2};
        const auto target_index = std::size_t {3};
        const auto gate_info = cre::create_one_control_one_target_gate(G::CX, control_index, target_index);
        const auto [unpacked_control, unpacked_target] = cre::unpack_one_control_one_target_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
    }

    SECTION("CRX gate")
    {
        const auto control_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = cre::create_one_control_one_target_one_angle_gate(G::CRX, control_index, target_index, theta);
        const auto [unpacked_control, unpacked_target, unpacked_theta] = cre::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("CP gate")
    {
        const auto control_index = std::size_t {4};
        const auto target_index = std::size_t {5};
        const auto theta = 1.23;
        const auto gate_info = cre::create_one_control_one_target_one_angle_gate(G::CP, control_index, target_index, theta);
        const auto [unpacked_control, unpacked_target, unpacked_theta] = cre::unpack_one_control_one_target_one_angle_gate(gate_info);

        REQUIRE(unpacked_control == control_index);
        REQUIRE(unpacked_target == target_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("M gate")
    {
        const auto target_index = std::size_t {6};
        const auto bit_index = std::size_t {7};
        const auto gate_info = cre::create_m_gate(target_index, bit_index);
        const auto [unpacked_qubit, unpacked_bit] = cre::unpack_m_gate(gate_info);

        REQUIRE(unpacked_qubit == target_index);
        REQUIRE(unpacked_bit == bit_index);
    }

    SECTION("U gate")
    {
        const auto target_index = std::size_t {0};
        const auto unitary_ptr = ket::ClonePtr<ket::Matrix2X2> {ket::x_gate()};

        const auto gate_info = cre::create_u_gate(target_index, unitary_ptr);
        const auto [unpacked_target_index, unpacked_unitary_ptr] = cre::unpack_u_gate(gate_info);

        REQUIRE(unpacked_target_index == target_index);
        REQUIRE(ket::almost_eq(*unitary_ptr, *unpacked_unitary_ptr));
    }

    SECTION("CU gate")
    {
        const auto control_index = std::size_t {0};
        const auto target_index = std::size_t {1};

        const auto unitary_ptr = ket::ClonePtr<ket::Matrix2X2> {ket::x_gate()};

        const auto gate_info = cre::create_cu_gate(control_index, target_index, unitary_ptr);
        const auto [u_control_index, u_target_index, u_unitary_ptr] = cre::unpack_cu_gate(gate_info);

        REQUIRE(u_control_index == control_index);
        REQUIRE(u_target_index == target_index);
        REQUIRE(ket::almost_eq(*unitary_ptr, *u_unitary_ptr));
    }
}

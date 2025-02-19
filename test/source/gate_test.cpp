#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/gate.hpp"

TEST_CASE("Test create and unpack gate functions")
{
    SECTION("X gate")
    {
        const auto qubit_index = std::size_t {2};
        const auto gate_info = impl_mqis::create_x_gate(qubit_index);
        const auto unpacked_index = impl_mqis::unpack_x_gate(gate_info);

        REQUIRE(unpacked_index == qubit_index);
    }

    SECTION("RX gate")
    {
        const auto theta = 1.57;
        const auto qubit_index = std::size_t {3};
        const auto gate_info = impl_mqis::create_rx_gate(theta, qubit_index);
        const auto [unpacked_theta, unpacked_index] = impl_mqis::unpack_rx_gate(gate_info);

        REQUIRE(unpacked_index == qubit_index);
        REQUIRE_THAT(unpacked_theta, Catch::Matchers::WithinRel(theta));
    }

    SECTION("H gate")
    {
        const auto qubit_index = std::size_t {0};
        const auto gate_info = impl_mqis::create_h_gate(qubit_index);
        const auto unpacked_index = impl_mqis::unpack_h_gate(gate_info);

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
        const auto gate_info = impl_mqis::create_crx_gate(source_index, target_index, theta);
        const auto [unpacked_source, unpacked_target, unpacked_theta] = impl_mqis::unpack_crx_gate(gate_info);

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
}

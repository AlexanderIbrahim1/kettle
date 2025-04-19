#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/gates/matrix2x2_gate_decomposition.hpp"
#include "mini-qiskit/gates/primitive_gate.hpp"


TEST_CASE("decomp_to_single_primitive_gate_()")
{
    using Info = impl_mqis::PrimitiveGateInfo;

    struct TestCase
    {
        mqis::Matrix2X2 input;
        std::optional<Info> expected;
    };

    SECTION("unparameterized primitive gates")
    {
        const auto testcase = GENERATE(
            TestCase {mqis::h_gate(), Info {mqis::Gate::H, {}}},
            TestCase {mqis::x_gate(), Info {mqis::Gate::X, {}}},
            TestCase {mqis::y_gate(), Info {mqis::Gate::Y, {}}},
            TestCase {mqis::z_gate(), Info {mqis::Gate::Z, {}}},
            TestCase {mqis::sx_gate(), Info {mqis::Gate::SX, {}}},
            TestCase {mqis::h_gate() * mqis::rx_gate(1.2345), std::nullopt}
        );

        const auto output = impl_mqis::decomp_to_single_primitive_gate_(testcase.input);

        REQUIRE(output.has_value());
        REQUIRE(output->gate == testcase.expected->gate);
        REQUIRE(output->parameter == std::nullopt);
    }

    SECTION("parameterized primitive gates")
    {
        constexpr auto abs_tol = 1.0e-6;

        const auto between_0_and_2pi = [](double x) -> double {
            // a primitive way of putting the minimum image of the angle within [0, 2pi);
            // none of the unit tests are expected to create very large angles anyways
            while (x < 0.0) {
                x += 2.0 * M_PI;
            }

            while (x >= 2.0 * M_PI) {
                x -= 2.0 * M_PI;
            }

            return x;
        };

        const auto angle = GENERATE(0.01, 0.25 * M_PI, 1.5 * M_PI, 1.99 * M_PI);

        const auto testcase = GENERATE_COPY(
            TestCase {mqis::rx_gate(angle), Info {mqis::Gate::RX, angle}},
            TestCase {mqis::ry_gate(angle), Info {mqis::Gate::RY, angle}},
            TestCase {mqis::rz_gate(angle), Info {mqis::Gate::RZ, angle}},
            TestCase {mqis::p_gate(angle), Info {mqis::Gate::P, angle}}
        );

        const auto output = impl_mqis::decomp_to_single_primitive_gate_(testcase.input);

        REQUIRE(output.has_value());
        REQUIRE(output->gate == testcase.expected->gate);

        const auto output_angle = between_0_and_2pi(output->parameter.value());
        const auto expected_angle = between_0_and_2pi(testcase.expected->parameter.value());

        REQUIRE_THAT(output_angle, Catch::Matchers::WithinAbs(expected_angle, abs_tol));
    }
}

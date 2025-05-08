#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/state/state.hpp"
#include "kettle/state/random.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/primitive_gate.hpp"

#include "kettle_internal/gates/matrix2x2_gate_decomposition.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"

namespace cre = ket::internal::create;


/*
    A primitive way of putting the minimum image of the angle within [0, 2pi);
    none of the unit tests are expected to create very large angles anyways
*/
static constexpr auto between_0_and_period(double x, double period) -> double {
    while (x < 0.0) {
        x += period;
    }

    while (x >= period) {
        x -= period;
    }

    return x;
}


TEST_CASE("decomp_to_single_primitive_gate_()")
{
    using Info = ket::internal::PrimitiveGateInfo_;

    struct TestCase
    {
        ket::Matrix2X2 input;
        std::optional<Info> expected;
    };

    SECTION("unparameterized primitive gates")
    {
        SECTION("successful decomposition")
        {
            const auto testcase = GENERATE(
                TestCase {ket::h_gate(), Info {ket::Gate::H, {}}},
                TestCase {ket::x_gate(), Info {ket::Gate::X, {}}},
                TestCase {ket::y_gate(), Info {ket::Gate::Y, {}}},
                TestCase {ket::z_gate(), Info {ket::Gate::Z, {}}},
                TestCase {ket::sx_gate(), Info {ket::Gate::SX, {}}}
            );

            const auto output = ket::internal::decomp_to_single_primitive_gate_(testcase.input);

            REQUIRE(output.has_value());
            REQUIRE(output->gate == testcase.expected->gate);
            REQUIRE(output->parameter == std::nullopt);
        }

        SECTION("unsuccessful decomposition")
        {
            auto testcase = TestCase {.input=ket::h_gate() * ket::rx_gate(1.2345), .expected=std::nullopt};

            const auto output = ket::internal::decomp_to_single_primitive_gate_(testcase.input);
            REQUIRE(!output.has_value());
        }
    }

    SECTION("parameterized primitive gates")
    {
        constexpr auto abs_tol = 1.0e-6;

        const auto angle = GENERATE(0.01, 0.25 * M_PI, 1.5 * M_PI, 1.99 * M_PI);

        const auto testcase = GENERATE_COPY(
            TestCase {ket::rx_gate(angle), Info {ket::Gate::RX, angle}},
            TestCase {ket::ry_gate(angle), Info {ket::Gate::RY, angle}},
            TestCase {ket::rz_gate(angle), Info {ket::Gate::RZ, angle}},
            TestCase {ket::p_gate(angle), Info {ket::Gate::P, angle}}
        );

        const auto output = ket::internal::decomp_to_single_primitive_gate_(testcase.input);

        REQUIRE(output.has_value());
        REQUIRE(output->gate == testcase.expected->gate);

        const auto output_angle = between_0_and_period(output->parameter.value(), 2.0 * M_PI);
        const auto expected_angle = between_0_and_period(testcase.expected->parameter.value(), 2.0 * M_PI);

        REQUIRE_THAT(output_angle, Catch::Matchers::WithinAbs(expected_angle, abs_tol));
    }
}

TEST_CASE("decompose to primtive gates; general")
{
    double angle0 = M_PI * GENERATE(0.01, 0.25, 0.75, 1.1, 1.75);
    double angle1 = M_PI * GENERATE(0.01, 0.25, 0.75, 1.1, 1.75);
    double angle2 = M_PI * GENERATE(0.01, 0.25, 0.75, 1.1, 1.75);
    double angle_global = M_PI * GENERATE(0.01, 0.25, 0.75, 1.1, 1.75);

    const auto unitary = [&]() {
        auto matrix = ket::rz_gate(angle2) * ket::ry_gate(angle1) * ket::rz_gate(angle0);
        const auto global_phase = std::complex<double> {std::cos(angle_global), std::sin(angle_global)};
        matrix *= global_phase;

        return matrix;
    }();

    SECTION("one target gates")
    {
        const auto target = std::size_t {0};
        const auto decomp_gates = ket::internal::decomp_to_one_target_primitive_gates_(target, unitary);

        // make sure the expect gates come out
        REQUIRE(decomp_gates.size() == 4);
        REQUIRE(decomp_gates[0].gate == ket::Gate::RZ);
        REQUIRE(decomp_gates[1].gate == ket::Gate::RY);
        REQUIRE(decomp_gates[2].gate == ket::Gate::RZ);
        REQUIRE(decomp_gates[3].gate == ket::Gate::P);

        // construct a circuit from the unitary matrix
        auto circuit0 = ket::QuantumCircuit {1};
        circuit0.add_u_gate(unitary, 0);

        // construct a circuit from the decomposed gates
        const auto [target0, angle_g0] = cre::unpack_one_target_one_angle_gate(decomp_gates[0]);
        const auto [target1, angle_g1] = cre::unpack_one_target_one_angle_gate(decomp_gates[1]);
        const auto [target2, angle_g2] = cre::unpack_one_target_one_angle_gate(decomp_gates[2]);
        const auto [target3, angle_g3] = cre::unpack_one_target_one_angle_gate(decomp_gates[3]);

        auto circuit1 = ket::QuantumCircuit {1};
        circuit1.add_rz_gate(target0, angle_g0);
        circuit1.add_ry_gate(target0, angle_g1);
        circuit1.add_rz_gate(target0, angle_g2);
        circuit1.add_p_gate(target0, angle_g3);

        // simulate the same state through both circuits, and make sure the outcome is the same
        auto state0 = ket::generate_random_state(1);
        auto state1 = state0;

        ket::simulate(circuit0, state0);
        ket::simulate(circuit1, state1);

        REQUIRE(ket::almost_eq(state0, state1));
    }

    SECTION("one control one target gates")
    {
        const auto target = std::size_t {0};
        const auto control = std::size_t {1};
        const auto decomp_gates = ket::internal::decomp_to_one_control_one_target_primitive_gates_(control, target, unitary);

        // make sure the expect gates come out
        REQUIRE(decomp_gates.size() == 4);
        REQUIRE(decomp_gates[0].gate == ket::Gate::CRZ);
        REQUIRE(decomp_gates[1].gate == ket::Gate::CRY);
        REQUIRE(decomp_gates[2].gate == ket::Gate::CRZ);
        REQUIRE(decomp_gates[3].gate == ket::Gate::CP);

        // construct a circuit from the unitary matrix
        auto circuit0 = ket::QuantumCircuit {2};
        circuit0.add_cu_gate(unitary, control, target);

        // construct a circuit from the decomposed gates
        const auto [control0, target0, angle_g0] = cre::unpack_one_control_one_target_one_angle_gate(decomp_gates[0]);
        const auto [control1, target1, angle_g1] = cre::unpack_one_control_one_target_one_angle_gate(decomp_gates[1]);
        const auto [control2, target2, angle_g2] = cre::unpack_one_control_one_target_one_angle_gate(decomp_gates[2]);
        const auto [control3, target3, angle_g3] = cre::unpack_one_control_one_target_one_angle_gate(decomp_gates[3]);

        auto circuit1 = ket::QuantumCircuit {2};
        circuit1.add_crz_gate(control0, target0, angle_g0);
        circuit1.add_cry_gate(control1, target1, angle_g1);
        circuit1.add_crz_gate(control2, target2, angle_g2);
        circuit1.add_cp_gate(control3, target3, angle_g3);

        // simulate the same state through both circuits, and make sure the outcome is the same
        auto state0 = ket::generate_random_state(2);
        auto state1 = state0;

        ket::simulate(circuit0, state0);
        ket::simulate(circuit1, state1);

        REQUIRE(ket::almost_eq(state0, state1));
    }
}

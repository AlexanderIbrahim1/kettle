#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/operations.hpp"
#include "mini-qiskit/state.hpp"

constexpr static auto ABS_TOL = double {1.0e-6};

// WARNING: BUG
// - the logical operators don't take ownership of the Matcher objects
// - so when you write
//     "Catch::Matchers::WithinRel(value, REL_TOL) || Catch::Matchers::WithinAbs(value, ABS_TOL)"
//   - the object for WithinRel ends up pointing to junk
// auto within_either(double value) -> const auto
// {
//     return Catch::Matchers::WithinRel(value, REL_TOL) || Catch::Matchers::WithinAbs(value, ABS_TOL);
// }

TEST_CASE("Swap states operation")
{
    // Initial state: |00> = {{1.0, 0.0}, {0.0, 0.0}}
    auto quantum_state = mqis::QuantumState {
        {{1.0, 0.0}, {0.0, 0.0}}
    };

    impl_mqis::swap_states(quantum_state, 0, 1);

    const auto& coeff_0 = quantum_state[0];
    const auto& coeff_1 = quantum_state[1];

    REQUIRE_THAT(coeff_0.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_0.imag(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.real(), Catch::Matchers::WithinAbs(1.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.imag(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
}

TEST_CASE("Superpose states operation")
{
    // Initial state: superposition: |+> = {{1.0/sqrt2, 0.0}, {1.0/sqrt2, 0.0}}
    auto quantum_state = mqis::QuantumState {
        {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}
    };

    impl_mqis::superpose_states(quantum_state, 0, 1);

    const auto& coeff_0 = quantum_state[0];
    const auto& coeff_1 = quantum_state[1];

    REQUIRE_THAT(coeff_0.real(), Catch::Matchers::WithinAbs(1.0, ABS_TOL));
    REQUIRE_THAT(coeff_0.imag(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.imag(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
}

TEST_CASE("Turn states operation")
{
    // Initial state: |00> = {{1.0, 0.0}, {0.0, 0.0}}
    auto quantum_state = mqis::QuantumState {
        {{1.0, 0.0}, {0.0, 0.0}}
    };

    impl_mqis::turn_states(quantum_state, 0, 1, M_PI);

    const auto& coeff_0 = quantum_state[0];
    const auto& coeff_1 = quantum_state[1];

    REQUIRE_THAT(coeff_0.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_0.imag(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.imag(), Catch::Matchers::WithinAbs(-1.0, ABS_TOL));
}

TEST_CASE("Phase turn states operation")
{
    // Initial state: |+> = {{1.0/sqrt2, 0.0}, {1.0/sqrt2, 0.0}}
    auto quantum_state = mqis::QuantumState {
        {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}
    };

    impl_mqis::phaseturn_states(quantum_state, 0, 1, M_PI);

    const auto& coeff_0 = quantum_state[0];
    const auto& coeff_1 = quantum_state[1];

    REQUIRE_THAT(coeff_0.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_0.imag(), Catch::Matchers::WithinAbs(-M_SQRT1_2, ABS_TOL));
    REQUIRE_THAT(coeff_1.real(), Catch::Matchers::WithinAbs(0.0, ABS_TOL));
    REQUIRE_THAT(coeff_1.imag(), Catch::Matchers::WithinAbs(M_SQRT1_2, ABS_TOL));
}

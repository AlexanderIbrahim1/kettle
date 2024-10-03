#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/state.hpp"

TEST_CASE("QuantumState with 3 qubits") {
    const auto n_qubits = 3;
    const auto state = mqis::QuantumState {n_qubits};

    // Check that there are 8 states (2^3)
    REQUIRE(state.n_states() == 8);

    // Check that the first state is Complex{1.0, 0.0}
    REQUIRE_THAT(state[0].real, Catch::Matchers::WithinRel(1.0));
    REQUIRE_THAT(state[0].imag, Catch::Matchers::WithinRel(0.0));

    // Check that the rest are Complex{0.0, 0.0}
    for (std::size_t i = 1; i < state.n_states(); ++i) {
        REQUIRE_THAT(state[i].real, Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[i].imag, Catch::Matchers::WithinRel(0.0));
    }
}

TEST_CASE("QuantumState with two coefficients") {
    SECTION("One with {1.0, 0.0}, {0.0, 0.0}") {
        const auto coefficients = std::vector<mqis::Complex> {{1.0, 0.0}, {0.0, 0.0}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real, Catch::Matchers::WithinRel(1.0));
        REQUIRE_THAT(state[0].imag, Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].real, Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].imag, Catch::Matchers::WithinRel(0.0));
    }

    SECTION("One with {1.0/sqrt(2), 0.0}, {1.0/sqrt(2), 0.0}") {
        const auto coeff_val = 1.0 / std::sqrt(2);
        const auto coefficients = std::vector<mqis::Complex> {{coeff_val, 0.0}, {coeff_val, 0.0}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real, Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[0].imag, Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].real, Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].imag, Catch::Matchers::WithinRel(0.0));
    }

    SECTION("One with {1.0/sqrt(4), 1.0/sqrt(4)}, {1.0/sqrt(4), 1.0/sqrt(4)}") {
        const auto coeff_val = 1.0 / std::sqrt(4);
        const auto coefficients = std::vector<mqis::Complex> {{coeff_val, coeff_val}, {coeff_val, coeff_val}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real, Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[0].imag, Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].real, Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].imag, Catch::Matchers::WithinRel(coeff_val));
    }
}

TEST_CASE("QuantumState with four coefficients") {
    SECTION("One with {1.0, 0.0}, the rest {0.0, 0.0}") {
        const auto coefficients = std::vector<mqis::Complex> {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        REQUIRE_THAT(state[0].real, Catch::Matchers::WithinRel(1.0));
        REQUIRE_THAT(state[0].imag, Catch::Matchers::WithinRel(0.0));
        for (std::size_t i = 1; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real, Catch::Matchers::WithinRel(0.0));
            REQUIRE_THAT(state[i].imag, Catch::Matchers::WithinRel(0.0));
        }
    }

    SECTION("One with all {1.0/sqrt(4), 0.0}") {
        const auto coeff_val = 1.0 / std::sqrt(4);
        const auto coefficients = std::vector<mqis::Complex> {{coeff_val, 0.0}, {coeff_val, 0.0}, {coeff_val, 0.0}, {coeff_val, 0.0}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        for (std::size_t i = 0; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real, Catch::Matchers::WithinRel(coeff_val));
            REQUIRE_THAT(state[i].imag, Catch::Matchers::WithinRel(0.0));
        }
    }

    SECTION("One with all {1.0/sqrt(8), 1.0/sqrt(8)}") {
        const auto coeff_val = 1.0 / std::sqrt(8);
        const auto coefficients = std::vector<mqis::Complex> {{coeff_val, coeff_val}, {coeff_val, coeff_val}, {coeff_val, coeff_val}, {coeff_val, coeff_val}};
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        for (std::size_t i = 0; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real, Catch::Matchers::WithinRel(coeff_val));
            REQUIRE_THAT(state[i].imag, Catch::Matchers::WithinRel(coeff_val));
        }
    }
}

TEST_CASE("Invalid QuantumState creation throws exceptions") {
    SECTION("With coefficients {0.0, 0.0}, {0.0, 0.0}") {
        const auto coefficients = std::vector<mqis::Complex> {{0.0, 0.0}, {0.0, 0.0}};
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }

    SECTION("With coefficients {2.0, 0.0}, {0.0, 0.0}") {
        const auto coefficients = std::vector<mqis::Complex> {{2.0, 0.0}, {0.0, 0.0}};
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }

    SECTION("With coefficients {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}") {
        const auto coefficients = std::vector<mqis::Complex> {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }
}

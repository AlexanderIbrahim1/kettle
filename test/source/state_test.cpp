#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/state.hpp"

TEST_CASE("QuantumState endian representation")
{
    SECTION("2 qubits, state |10>")
    {
        const auto state_via_little = mqis::QuantumState {
            {{0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}},
            mqis::QuantumStateEndian::LITTLE
        };

        const auto state_via_big = mqis::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}},
            mqis::QuantumStateEndian::BIG
        };

        REQUIRE(mqis::almost_eq(state_via_little, state_via_big));
    }
}

TEST_CASE("QuantumState from string")
{
    SECTION("1 qubit")
    {
        SECTION("|0>, big endian")
        {
            const auto state = mqis::QuantumState {"0", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
        }

        SECTION("|1>, big endian")
        {
            const auto state = mqis::QuantumState {"1", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {1.0, 0.0}));
        }

        SECTION("|0>, little endian")
        {
            const auto state = mqis::QuantumState {"0", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
        }

        SECTION("|1>, little endian")
        {
            const auto state = mqis::QuantumState {"1", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {1.0, 0.0}));
        }
    }

    SECTION("2 qubits")
    {
        SECTION("|00>, big endian")
        {
            const auto state = mqis::QuantumState {"00", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|00>, little endian")
        {
            const auto state = mqis::QuantumState {"00", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|01>, big endian")
        {
            const auto state = mqis::QuantumState {"01", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|01>, little endian")
        {
            const auto state = mqis::QuantumState {"01", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|10>, big endian")
        {
            const auto state = mqis::QuantumState {"10", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|10>, little endian")
        {
            const auto state = mqis::QuantumState {"10", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {1.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {0.0, 0.0}));
        }

        SECTION("|11>, big endian")
        {
            const auto state = mqis::QuantumState {"11", mqis::QuantumStateEndian::BIG};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {1.0, 0.0}));
        }

        SECTION("|11>, little endian")
        {
            const auto state = mqis::QuantumState {"11", mqis::QuantumStateEndian::LITTLE};
            REQUIRE(mqis::almost_eq(state[0], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[1], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[2], {0.0, 0.0}));
            REQUIRE(mqis::almost_eq(state[3], {1.0, 0.0}));
        }
    }
}

TEST_CASE("QuantumState with 3 qubits")
{
    const auto n_qubits = 3;
    const auto state = mqis::QuantumState {n_qubits};

    // check that there are 8 states (2^3)
    REQUIRE(state.n_states() == 8);

    // check that the first state is std::complex<double>{1.0, 0.0}
    REQUIRE_THAT(state[0].real(), Catch::Matchers::WithinRel(1.0));
    REQUIRE_THAT(state[0].imag(), Catch::Matchers::WithinRel(0.0));

    // check that the rest are std::complex<double>{0.0, 0.0}
    for (std::size_t i = 1; i < state.n_states(); ++i) {
        REQUIRE_THAT(state[i].real(), Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[i].imag(), Catch::Matchers::WithinRel(0.0));
    }
}

TEST_CASE("QuantumState with two coefficients")
{
    SECTION("One with {1.0, 0.0}, {0.0, 0.0}")
    {
        const auto coefficients = std::vector<std::complex<double>> {
            {1.0, 0.0},
            {0.0, 0.0}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real(), Catch::Matchers::WithinRel(1.0));
        REQUIRE_THAT(state[0].imag(), Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].real(), Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].imag(), Catch::Matchers::WithinRel(0.0));
    }

    SECTION("One with {1.0/sqrt(2), 0.0}, {1.0/sqrt(2), 0.0}")
    {
        const auto coeff_val = 1.0 / std::sqrt(2);
        const auto coefficients = std::vector<std::complex<double>> {
            {coeff_val, 0.0},
            {coeff_val, 0.0}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real(), Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[0].imag(), Catch::Matchers::WithinRel(0.0));
        REQUIRE_THAT(state[1].real(), Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].imag(), Catch::Matchers::WithinRel(0.0));
    }

    SECTION("One with {1.0/sqrt(4), 1.0/sqrt(4)}, {1.0/sqrt(4), 1.0/sqrt(4)}")
    {
        const auto coeff_val = 1.0 / std::sqrt(4);
        const auto coefficients = std::vector<std::complex<double>> {
            {coeff_val, coeff_val},
            {coeff_val, coeff_val}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 2);
        REQUIRE_THAT(state[0].real(), Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[0].imag(), Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].real(), Catch::Matchers::WithinRel(coeff_val));
        REQUIRE_THAT(state[1].imag(), Catch::Matchers::WithinRel(coeff_val));
    }
}

TEST_CASE("QuantumState with four coefficients")
{
    SECTION("One with {1.0, 0.0}, the rest {0.0, 0.0}")
    {
        const auto coefficients = std::vector<std::complex<double>> {
            {1.0, 0.0},
            {0.0, 0.0},
            {0.0, 0.0},
            {0.0, 0.0}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        REQUIRE_THAT(state[0].real(), Catch::Matchers::WithinRel(1.0));
        REQUIRE_THAT(state[0].imag(), Catch::Matchers::WithinRel(0.0));
        for (std::size_t i = 1; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real(), Catch::Matchers::WithinRel(0.0));
            REQUIRE_THAT(state[i].imag(), Catch::Matchers::WithinRel(0.0));
        }
    }

    SECTION("One with all {1.0/sqrt(4), 0.0}")
    {
        const auto coeff_val = 1.0 / std::sqrt(4);
        const auto coefficients = std::vector<std::complex<double>> {
            {coeff_val, 0.0},
            {coeff_val, 0.0},
            {coeff_val, 0.0},
            {coeff_val, 0.0}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        for (std::size_t i = 0; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real(), Catch::Matchers::WithinRel(coeff_val));
            REQUIRE_THAT(state[i].imag(), Catch::Matchers::WithinRel(0.0));
        }
    }

    SECTION("One with all {1.0/sqrt(8), 1.0/sqrt(8)}")
    {
        const auto coeff_val = 1.0 / std::sqrt(8);
        const auto coefficients = std::vector<std::complex<double>> {
            {coeff_val, coeff_val},
            {coeff_val, coeff_val},
            {coeff_val, coeff_val},
            {coeff_val, coeff_val}
        };
        const auto state = mqis::QuantumState {coefficients};

        REQUIRE(state.n_states() == 4);
        for (std::size_t i = 0; i < state.n_states(); ++i) {
            REQUIRE_THAT(state[i].real(), Catch::Matchers::WithinRel(coeff_val));
            REQUIRE_THAT(state[i].imag(), Catch::Matchers::WithinRel(coeff_val));
        }
    }
}

TEST_CASE("Invalid QuantumState creation throws exceptions")
{
    SECTION("With coefficients {0.0, 0.0}, {0.0, 0.0}")
    {
        const auto coefficients = std::vector<std::complex<double>> {
            {0.0, 0.0},
            {0.0, 0.0}
        };
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }

    SECTION("With coefficients {2.0, 0.0}, {0.0, 0.0}")
    {
        const auto coefficients = std::vector<std::complex<double>> {
            {2.0, 0.0},
            {0.0, 0.0}
        };
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }

    SECTION("With coefficients {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}")
    {
        const auto coefficients = std::vector<std::complex<double>> {
            {1.0, 0.0},
            {0.0, 0.0},
            {0.0, 0.0}
        };
        REQUIRE_THROWS_AS(mqis::QuantumState {coefficients}, std::runtime_error);
    }
}

TEST_CASE("state_as_dynamic_bitset")
{
    struct InputAndOutput
    {
        std::size_t i_state;
        std::size_t n_qubits;
        std::vector<std::uint8_t> bits;
    };

    SECTION("little endian")
    {
        SECTION("1 qubit")
        {
            auto pair = GENERATE(InputAndOutput {0, 1, {0}}, InputAndOutput {1, 1, {1}});

            REQUIRE(mqis::state_as_dynamic_bitset_little_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }

        SECTION("2 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {
                    0, 2, {0, 0}
            },
                InputAndOutput {1, 2, {1, 0}},
                InputAndOutput {2, 2, {0, 1}},
                InputAndOutput {3, 2, {1, 1}}
            );

            REQUIRE(mqis::state_as_dynamic_bitset_little_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }

        SECTION("3 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {
                    0, 3, {0, 0, 0}
            },
                InputAndOutput {1, 3, {1, 0, 0}},
                InputAndOutput {2, 3, {0, 1, 0}},
                InputAndOutput {3, 3, {1, 1, 0}},
                InputAndOutput {4, 3, {0, 0, 1}},
                InputAndOutput {5, 3, {1, 0, 1}},
                InputAndOutput {6, 3, {0, 1, 1}},
                InputAndOutput {7, 3, {1, 1, 1}}
            );

            REQUIRE(mqis::state_as_dynamic_bitset_little_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }
    }

    SECTION("big endian")
    {
        SECTION("1 qubit")
        {
            auto pair = GENERATE(InputAndOutput {0, 1, {0}}, InputAndOutput {1, 1, {1}});

            REQUIRE(mqis::state_as_dynamic_bitset_big_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }

        SECTION("2 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {
                    0, 2, {0, 0}
            },
                InputAndOutput {1, 2, {0, 1}},
                InputAndOutput {2, 2, {1, 0}},
                InputAndOutput {3, 2, {1, 1}}
            );

            REQUIRE(mqis::state_as_dynamic_bitset_big_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }

        SECTION("3 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {
                    0, 3, {0, 0, 0}
            },
                InputAndOutput {1, 3, {0, 0, 1}},
                InputAndOutput {2, 3, {0, 1, 0}},
                InputAndOutput {3, 3, {0, 1, 1}},
                InputAndOutput {4, 3, {1, 0, 0}},
                InputAndOutput {5, 3, {1, 0, 1}},
                InputAndOutput {6, 3, {1, 1, 0}},
                InputAndOutput {7, 3, {1, 1, 1}}
            );

            REQUIRE(mqis::state_as_dynamic_bitset_big_endian(pair.i_state, pair.n_qubits) == pair.bits);
        }
    }
}

TEST_CASE("state_as_bitstring")
{
    struct InputAndOutput
    {
        std::size_t i_state;
        std::size_t n_qubits;
        std::string bitstring;
    };

    SECTION("little endian")
    {
        SECTION("1 qubit")
        {
            auto pair = GENERATE(InputAndOutput {0, 1, "0"}, InputAndOutput {1, 1, "1"});

            REQUIRE(mqis::state_as_bitstring_little_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }

        SECTION("2 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {0, 2, "00"},
                InputAndOutput {1, 2, "10"},
                InputAndOutput {2, 2, "01"},
                InputAndOutput {3, 2, "11"}
            );

            REQUIRE(mqis::state_as_bitstring_little_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }

        SECTION("3 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {0, 3, "000"},
                InputAndOutput {1, 3, "100"},
                InputAndOutput {2, 3, "010"},
                InputAndOutput {3, 3, "110"},
                InputAndOutput {4, 3, "001"},
                InputAndOutput {5, 3, "101"},
                InputAndOutput {6, 3, "011"},
                InputAndOutput {7, 3, "111"}
            );

            REQUIRE(mqis::state_as_bitstring_little_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }
    }

    SECTION("big endian")
    {
        SECTION("1 qubit")
        {
            auto pair = GENERATE(InputAndOutput {0, 1, "0"}, InputAndOutput {1, 1, "1"});

            REQUIRE(mqis::state_as_bitstring_big_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }

        SECTION("2 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {0, 2, "00"},
                InputAndOutput {1, 2, "01"},
                InputAndOutput {2, 2, "10"},
                InputAndOutput {3, 2, "11"}
            );

            REQUIRE(mqis::state_as_bitstring_big_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }

        SECTION("3 qubit")
        {
            auto pair = GENERATE(
                InputAndOutput {0, 3, "000"},
                InputAndOutput {1, 3, "001"},
                InputAndOutput {2, 3, "010"},
                InputAndOutput {3, 3, "011"},
                InputAndOutput {4, 3, "100"},
                InputAndOutput {5, 3, "101"},
                InputAndOutput {6, 3, "110"},
                InputAndOutput {7, 3, "111"}
            );

            REQUIRE(mqis::state_as_bitstring_big_endian(pair.i_state, pair.n_qubits) == pair.bitstring);
        }
    }
}

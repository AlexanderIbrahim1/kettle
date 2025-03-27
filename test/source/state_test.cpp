#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define REQUIRE_MSG(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
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

TEST_CASE("are_all_marginal_bits_on_right_")
{
    struct TestInfo
    {
        std::string input;
        bool expected;
    };

    const auto info = GENERATE(
        TestInfo {"", true},
        TestInfo {"0", true},
        TestInfo {"1", true},
        TestInfo {"01", true},
        TestInfo {"10", true},
        TestInfo {"00x", true},
        TestInfo {"10x", true},
        TestInfo {"01x", true},
        TestInfo {"01xx", true},
        TestInfo {"010010xx", true},
        TestInfo {"xx", true},
        TestInfo {"xxxx", true},
        TestInfo {"xx0x", false},
        TestInfo {"x00x", false},
        TestInfo {"xx1x", false},
        TestInfo {"xx1", false},
        TestInfo {"00x1", false}
    );

    REQUIRE(impl_mqis::are_all_marginal_bits_on_right_(info.input) == info.expected);
}

TEST_CASE("rstrip_marginal_bits")
{
    struct TestInfo
    {
        std::string input;
        std::string expected;
    };

    const auto info = GENERATE(
        TestInfo {"", ""},
        TestInfo {"0", "0"},
        TestInfo {"1", "1"},
        TestInfo {"01", "01"},
        TestInfo {"10", "10"},
        TestInfo {"00x", "00"},
        TestInfo {"10x", "10"},
        TestInfo {"01x", "01"},
        TestInfo {"01xx", "01"},
        TestInfo {"010010xx", "010010"},
        TestInfo {"xx", ""},
        TestInfo {"xxxx", ""}
    );

    REQUIRE(mqis::rstrip_marginal_bits(info.input) == info.expected);
}

TEST_CASE("bitstring_to_state_index_little_endian")
{
    struct TestInfo
    {
        std::string input;
        std::size_t expected;
    };

    const auto info = GENERATE(
        TestInfo {"00", 0},
        TestInfo {"10", 1},
        TestInfo {"01", 2},
        TestInfo {"11", 3},
        TestInfo {"000", 0},
        TestInfo {"100", 1},
        TestInfo {"010", 2},
        TestInfo {"110", 3},
        TestInfo {"001", 4},
        TestInfo {"101", 5},
        TestInfo {"011", 6},
        TestInfo {"111", 7}
    );

    REQUIRE(mqis::bitstring_to_state_index(info.input) == info.expected);
}

TEST_CASE("tensor product")
{
    using Amplitudes = std::vector<std::complex<double>>;

    SECTION("direct 1-qubit x 1-qubit")
    {
        const auto state0 = mqis::QuantumState {Amplitudes {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}};
        const auto state1 = mqis::QuantumState {Amplitudes {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}};
        const auto expected = mqis::QuantumState {Amplitudes {{0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}}};

        const auto actual = mqis::tensor_product(state0, state1);

        REQUIRE(mqis::almost_eq(actual, expected));
    }

    SECTION("unentangled 1-qubit x 1-qubit")
    {
        struct TestCase
        {
            std::function<void(void)> add_gates;
            std::string message;
        };

        auto state0 = mqis::QuantumState {"0"};
        auto state1 = mqis::QuantumState {"0"};
        auto product_state = mqis::QuantumState {"00"};

        auto circuit0 = mqis::QuantumCircuit {1};
        auto circuit1 = mqis::QuantumCircuit {1};
        auto product_circuit = mqis::QuantumCircuit {2};

        auto testcase = GENERATE_REF(
            TestCase {
                [&]() {
                    circuit0.add_h_gate(0);
                    circuit1.add_h_gate(0);
                    product_circuit.add_h_gate({0, 1});
                },
                "H on each circuit"
            },
            TestCase {
                [&]() {
                    circuit1.add_h_gate({0, 0});
                    product_circuit.add_h_gate({1, 1});
                },
                "H twice on circuit1"
            },
            TestCase {
                [&]() {
                    circuit0.add_h_gate(0);
                    circuit1.add_x_gate(0);
                    product_circuit.add_h_gate(0);
                    product_circuit.add_x_gate(1);
                },
                "H on circuit0, X on circuit1"
            },
            TestCase {
                [&]() {
                    circuit0.add_h_gate(0);
                    circuit0.add_x_gate(0);
                    circuit1.add_z_gate(0);
                    product_circuit.add_h_gate(0);
                    product_circuit.add_x_gate(0);
                    product_circuit.add_z_gate(1);
                },
                "H and X on circuit0, Z on circuit1"
            }
        );

        testcase.add_gates();

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);
        mqis::simulate(product_circuit, product_state);

        const auto tensor_product_state = mqis::tensor_product(state0, state1);

        REQUIRE_MSG(mqis::almost_eq(product_state, tensor_product_state), testcase.message);
    }

    SECTION("unentangled 2-qubit x 3-qubit")
    {
        struct TestCase
        {
            std::function<void(void)> add_gates;
            std::string message;
        };

        auto state0 = mqis::QuantumState {"00"};
        auto state1 = mqis::QuantumState {"000"};
        auto product_state = mqis::QuantumState {"00000"};

        auto circuit0 = mqis::QuantumCircuit {2};
        auto circuit1 = mqis::QuantumCircuit {3};
        auto product_circuit = mqis::QuantumCircuit {5};

        auto testcase = GENERATE_REF(
            TestCase {
                [&]() {
                    circuit0.add_h_gate({0, 1});
                    circuit1.add_h_gate({0, 1, 2});
                    product_circuit.add_h_gate({0, 1, 2, 3, 4});
                },
                "circuit0 : H(1)H(0), circuit1: H(2)H(1)H(0)"
            },
            TestCase {
                [&]() {
                    circuit0.add_h_gate({0, 1});
                    circuit1.add_x_gate({1, 2});
                    circuit1.add_h_gate(0);
                    circuit1.add_cx_gate(0, 1);
                    product_circuit.add_h_gate({0, 1});
                    product_circuit.add_x_gate({3, 4});
                    product_circuit.add_h_gate(2);
                    product_circuit.add_cx_gate(2, 3);
                },
                "circuit0 : H(1)H(0), circuit1: CX(0,1)H(0)X(2)X(1)"
            }
        );

        testcase.add_gates();

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);
        mqis::simulate(product_circuit, product_state);

        const auto tensor_product_state = mqis::tensor_product(state0, state1);

        REQUIRE_MSG(mqis::almost_eq(product_state, tensor_product_state), testcase.message);
    }
}


TEST_CASE("read_statevector()")
{
    auto stream = std::stringstream {
        "3                                                    \n"
        " (4.950737714883371443e-02+4.950737714883371443e-02j)\n"
        " (9.901475429766742886e-02+9.901475429766742886e-02j)\n"
        " (1.485221314465011433e-01+1.485221314465011433e-01j)\n"
        " (1.980295085953348577e-01+1.980295085953348577e-01j)\n"
        " (2.475368857441685444e-01+2.475368857441685444e-01j)\n"
        " (2.970442628930022866e-01+2.970442628930022866e-01j)\n"
        " (3.465516400418360288e-01+3.465516400418360288e-01j)\n"
        " (3.960590171906697154e-01+3.960590171906697154e-01j)\n"
    };

    const auto actual = mqis::read_statevector(stream);

    auto expected_amplitudes = []() {
        auto output = std::vector<std::complex<double>> {};
        output.reserve(8);

        for (std::size_t i {1}; i < 9; ++i) {
            const auto x = static_cast<double>(i) * 1.1;
            const auto value = std::complex<double> {x, x};
            output.push_back(value);
        }

        const auto norm_sq = std::accumulate(output.begin(), output.end(), 0.0, [](auto acc, auto elem) { return acc + std::norm(elem); });
        const auto norm = std::sqrt(norm_sq);

        std::transform(output.begin(), output.end(), output.begin(), [&](auto elem) { return elem / norm; });

        return output;
    }();

    const auto expected = mqis::QuantumState {std::move(expected_amplitudes)};

    REQUIRE(mqis::almost_eq(actual, expected));
}

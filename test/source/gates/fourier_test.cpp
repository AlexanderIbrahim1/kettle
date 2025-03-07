#include <cmath>
#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/gates/fourier.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

TEST_CASE("Forward QFT on |0> state")
{
    SECTION("1 qubit")
    {
        auto state = mqis::QuantumState {"0"};
        auto circuit = mqis::QuantumCircuit {1};

        mqis::apply_forward_fourier_transform(circuit, {0});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(2.0);
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }

    SECTION("2 qubits")
    {
        auto state = mqis::QuantumState {"00"};
        auto circuit = mqis::QuantumCircuit {2};

        mqis::apply_forward_fourier_transform(circuit, {0, 1});

        // clang-format off
        const auto norm = 1.0 / 2.0;
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }

    SECTION("3 qubits")
    {
        auto state = mqis::QuantumState {"000"};
        auto circuit = mqis::QuantumCircuit {3};

        mqis::apply_forward_fourier_transform(circuit, {0, 1, 2});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(8.0);
        const auto expected = mqis::QuantumState {
            {
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }
}

TEST_CASE("basic Forward QFT on 2-qubit computational basis states")
{
    struct TestPair
    {
        std::string input;
        mqis::QuantumState expected;
    };

    const auto norm = 1.0 / 2.0;

    // clang-format off
    auto pair = GENERATE_COPY(
        TestPair {"00", mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, { norm,  0.0}, { norm,   0.0}} }},
        TestPair {"10", mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, {-norm,  0.0}, {-norm,   0.0}} }},
        TestPair {"01", mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0, norm}, {  0.0, -norm}} }},
        TestPair {"11", mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0,-norm}, {  0.0,  norm}} }}
    );
    // clang-format on

    auto state = mqis::QuantumState {pair.input};
    auto circuit = mqis::QuantumCircuit {2};

    mqis::apply_forward_fourier_transform(circuit, {0, 1});
    mqis::simulate(circuit, state);

    REQUIRE(mqis::almost_eq(state, pair.expected));
}

TEST_CASE("basic Forward QFT on 3-qubit computational basis states")
{
    struct TestPair
    {
        int i_bitstring;
        std::string input;
    };

    // clang-format off
    auto pair = GENERATE_COPY(
        TestPair {0, "000"},
        TestPair {1, "001"},
        TestPair {2, "010"},
        TestPair {3, "011"},
        TestPair {4, "100"},
        TestPair {5, "101"},
        TestPair {6, "110"},
        TestPair {7, "111"}
    );
    // clang-format on

    const auto create_expected_state = [](const TestPair& test_pair)
    {
        const auto n_states = 8;
        const auto norm = 1.0 / std::sqrt(static_cast<double>(n_states));

        auto coefficients = std::vector<std::complex<double>> {};
        coefficients.reserve(n_states);

        for (int i {0}; i < n_states; ++i) {
            const auto angle = (M_PI / 4.0) * static_cast<double>(i * test_pair.i_bitstring);
            const auto real = norm * std::cos(angle);
            const auto imag = norm * std::sin(angle);

            coefficients.push_back({real, imag});
        }

        return mqis::QuantumState {coefficients, mqis::QuantumStateEndian::BIG};
    };

    auto state = mqis::QuantumState {pair.input, mqis::QuantumStateEndian::LITTLE};
    auto circuit = mqis::QuantumCircuit {3};

    mqis::apply_forward_fourier_transform(circuit, {0, 1, 2});
    mqis::simulate(circuit, state);

    const auto expected = create_expected_state(pair);

    REQUIRE(mqis::almost_eq(state, expected));
}

TEST_CASE("basic Forward QFT on 4-qubit computational basis states")
{
    struct TestPair
    {
        int i_bitstring;
        std::string input;
    };

    // clang-format off
    auto pair = GENERATE(
        TestPair {0, "0000"},
        TestPair {1, "0001"},
        TestPair {2, "0010"},
        TestPair {3, "0011"},
        TestPair {4, "0100"},
        TestPair {5, "0101"},
        TestPair {6, "0110"},
        TestPair {7, "0111"},
        TestPair {8, "1000"},
        TestPair {9, "1001"},
        TestPair {10, "1010"},
        TestPair {11, "1011"},
        TestPair {12, "1100"},
        TestPair {13, "1101"},
        TestPair {14, "1110"},
        TestPair {15, "1111"}
    );
    // clang-format on

    const auto create_expected_state = [](const TestPair& test_pair)
    {
        const auto n_states = 16;
        const auto norm = 1.0 / std::sqrt(static_cast<double>(n_states));

        auto coefficients = std::vector<std::complex<double>> {};
        coefficients.reserve(n_states);

        for (int i {0}; i < n_states; ++i) {
            const auto angle = (2.0 * M_PI / n_states) * static_cast<double>((i * test_pair.i_bitstring) % n_states);
            const auto real = norm * std::cos(angle);
            const auto imag = norm * std::sin(angle);

            coefficients.push_back({real, imag});
        }

        return mqis::QuantumState {coefficients, mqis::QuantumStateEndian::BIG};
    };

    auto state = mqis::QuantumState {pair.input, mqis::QuantumStateEndian::LITTLE};
    auto circuit = mqis::QuantumCircuit {4};

    mqis::apply_forward_fourier_transform(circuit, {0, 1, 2, 3});
    mqis::simulate(circuit, state);

    const auto expected = create_expected_state(pair);

    REQUIRE(mqis::almost_eq(state, expected));
}

TEST_CASE("inverse QFT after forward QFT")
{
    SECTION("2-qubit gates")
    {
        // clang-format off
        const auto init_bitstring = std::string {GENERATE(
            "00", "10", "01", "11",
            "000", "100", "010", "110", "001", "101", "011", "111",
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )};
        // clang-format on

        auto state = mqis::QuantumState {init_bitstring};
        auto expected = mqis::QuantumState {init_bitstring};

        const auto n_qubits = init_bitstring.size();
        auto qubit_indices = std::vector<std::size_t> (n_qubits);
        std::iota(qubit_indices.begin(), qubit_indices.end(), 0);

        auto circuit = mqis::QuantumCircuit {init_bitstring.size()};
        mqis::apply_forward_fourier_transform(circuit, qubit_indices);
        mqis::apply_inverse_fourier_transform(circuit, qubit_indices);

        REQUIRE(mqis::almost_eq(state, expected));
    }
}

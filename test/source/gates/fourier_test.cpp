#include <cmath>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/arange.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/statevector.hpp"

TEST_CASE("Forward QFT on |0> state")
{
    SECTION("1 qubit")
    {
        auto state = ket::Statevector {"0"};
        auto circuit = ket::QuantumCircuit {1};

        circuit.add_qft_gate({0});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(2.0);
        const auto expected = ket::Statevector {
            {
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, expected));
    }

    SECTION("2 qubits")
    {
        auto state = ket::Statevector {"00"};
        auto circuit = ket::QuantumCircuit {2};

        circuit.add_qft_gate({0, 1});

        // clang-format off
        const auto norm = 1.0 / 2.0;
        const auto expected = ket::Statevector {
            {
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0},
                {norm, 0.0}
            }
        };
        // clang-format on

        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, expected));
    }

    SECTION("3 qubits")
    {
        auto state = ket::Statevector {"000"};
        auto circuit = ket::QuantumCircuit {3};

        circuit.add_qft_gate({0, 1, 2});

        // clang-format off
        const auto norm = 1.0 / std::sqrt(8.0);
        const auto expected = ket::Statevector {
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

        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, expected));
    }
}

TEST_CASE("basic Forward QFT on 2-qubit computational basis states")
{
    struct TestPair
    {
        std::string input;
        ket::Statevector expected;
    };

    const auto norm = 1.0 / 2.0;

    // clang-format off
    auto pair = GENERATE_COPY(
        TestPair {"00", ket::Statevector { {{ norm,  0.0}, { norm,  0.0}, { norm,  0.0}, { norm,   0.0}} }},
        TestPair {"10", ket::Statevector { {{ norm,  0.0}, { norm,  0.0}, {-norm,  0.0}, {-norm,   0.0}} }},
        TestPair {"01", ket::Statevector { {{ norm,  0.0}, {-norm,  0.0}, {  0.0, norm}, {  0.0, -norm}} }},
        TestPair {"11", ket::Statevector { {{ norm,  0.0}, {-norm,  0.0}, {  0.0,-norm}, {  0.0,  norm}} }}
    );
    // clang-format on

    auto state = ket::Statevector {pair.input};
    auto circuit = ket::QuantumCircuit {2};

    circuit.add_qft_gate({0, 1});
    ket::simulate(circuit, state);

    REQUIRE(ket::almost_eq(state, pair.expected));
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

        return ket::Statevector {coefficients, ket::Endian::BIG};
    };

    auto state = ket::Statevector {pair.input, ket::Endian::LITTLE};
    auto circuit = ket::QuantumCircuit {3};

    circuit.add_qft_gate({0, 1, 2});
    ket::simulate(circuit, state);

    const auto expected = create_expected_state(pair);

    REQUIRE(ket::almost_eq(state, expected));
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

        return ket::Statevector {coefficients, ket::Endian::BIG};
    };

    auto state = ket::Statevector {pair.input, ket::Endian::LITTLE};
    auto circuit = ket::QuantumCircuit {4};

    circuit.add_qft_gate({0, 1, 2, 3});
    ket::simulate(circuit, state);

    const auto expected = create_expected_state(pair);

    REQUIRE(ket::almost_eq(state, expected));
}

TEST_CASE("inverse QFT after forward QFT")
{
    SECTION("2-qubit, 3-qubit, and 4-qubit gates")
    {
        // clang-format off
        const auto init_bitstring = std::string {GENERATE(
            "00", "10", "01", "11",
            "000", "100", "010", "110", "001", "101", "011", "111",
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )};
        // clang-format on

        auto state = ket::Statevector {init_bitstring};
        auto expected = ket::Statevector {init_bitstring};

        const auto qubit_indices = ket::arange(init_bitstring.size());

        auto circuit = ket::QuantumCircuit {init_bitstring.size()};
        circuit.add_qft_gate(qubit_indices);
        circuit.add_iqft_gate(qubit_indices);

        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, expected));
    }
}

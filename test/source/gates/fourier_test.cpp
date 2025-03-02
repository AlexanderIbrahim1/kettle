#include <iostream>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/state.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/common/complex.hpp"
#include "mini-qiskit/gates/fourier.hpp"


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

// TEST_CASE("basic Forward QFT on 3-qubit computational basis states")
// {
//     struct TestPair
//     {
//         int i_bitstring;
//         std::string input;
//     };
// 
//     // clang-format off
//     auto pair = GENERATE_COPY(
//         // TestPair {0, "000"},  // PASSES
//         // TestPair {1, "001"}
//         // TestPair {2, "010"}   // PASSES
//         // TestPair {3, "011"}
//         TestPair {4, "100"}   // WEIRD??? (I think the function isn't working properly)
//         // TestPair {5, "101"}
//         // TestPair {6, "110"}
//         // TestPair {7, "111"}
//     );
//     // clang-format on
// 
//     const auto create_expected_state = [](const TestPair& test_pair) {
//         const auto n_states = 8;
//         const auto norm = 1.0 / std::sqrt(static_cast<double>(n_states));
// 
//         auto coefficients = std::vector<mqis::Complex> {};
//         coefficients.reserve(n_states);
// 
//         for (int i {0}; i < n_states; ++i) {
//             const auto angle = (M_PI / 4.0) * static_cast<double>(i * test_pair.i_bitstring);
//             const auto real = norm * std::cos(angle);
//             const auto imag = norm * std::sin(angle);
// 
//             // std::cout << "(real, imag) = (" << real << ", " << imag << ")\n";
// 
//             coefficients.push_back({real, imag});
//         }
// 
//         return mqis::QuantumState {coefficients, mqis::QuantumStateEndian::BIG};
//     };
// 
//     const auto print_state = [](const mqis::QuantumState& qstate) {
//         std::cout << "--- STATE ---\n";
//         for (auto i : std::vector<std::size_t>{0, 4, 2, 6, 1, 5, 3, 7}) {
//             const auto coeff = qstate[i];
//             std::cout << "(real, imag) = (" << coeff.real << ", " << coeff.imag << ")\n";
//         }
//     };
// 
//     auto state = mqis::QuantumState {pair.input, mqis::QuantumStateEndian::BIG};
//     auto circuit = mqis::QuantumCircuit {3};
// 
//     mqis::apply_forward_fourier_transform(circuit, {0, 1, 2});
//     mqis::simulate(circuit, state);
// 
//     const auto expected = create_expected_state(pair);
// 
//     print_state(state);
//     print_state(expected);
// 
//     REQUIRE(mqis::almost_eq(state, expected));
// }

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mini-qiskit/state.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
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

    // SECTION("00 state")
    // {
    //     const auto input = std::string {"00"};
    //     const auto expected = mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, { norm,  0.0}, { norm,   0.0}} };

    //     auto state = mqis::QuantumState {input};
    //     auto circuit = mqis::QuantumCircuit {2};

    //     mqis::apply_forward_fourier_transform(circuit, {0, 1});
    //     mqis::simulate(circuit, state);

    //     REQUIRE(mqis::almost_eq(state, expected));
    // }

    // SECTION("10 state")
    // {
    //     const auto input = std::string {"10"};
    //     const auto expected = mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, {-norm,  0.0}, {-norm,   0.0}} };

    //     auto state = mqis::QuantumState {input};
    //     auto circuit = mqis::QuantumCircuit {2};

    //     mqis::apply_forward_fourier_transform(circuit, {0, 1});
    //     mqis::simulate(circuit, state);

    //     REQUIRE(mqis::almost_eq(state, expected));
    // }

    SECTION("01 state")
    {
        const auto input = std::string {"01"};
        const auto expected = mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0, norm}, {  0.0, -norm}} };

        auto state = mqis::QuantumState {input};
        auto circuit = mqis::QuantumCircuit {2};

        circuit.add_h_gate(0);
        circuit.add_crz_gate(M_PI, 1, 0);
        circuit.add_h_gate(1);

        circuit.add_cx_gate(1, 0);
        circuit.add_cx_gate(0, 1);
        circuit.add_cx_gate(1, 0);

        // mqis::apply_forward_fourier_transform(circuit, {0, 1});
        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, expected));
    }

//     SECTION("11 state")
//     {
//         const auto input = std::string {"11"};
//         const auto expected = mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0,-norm}, {  0.0,  norm}} };
// 
//         auto state = mqis::QuantumState {input};
//         auto circuit = mqis::QuantumCircuit {2};
// 
//         mqis::apply_forward_fourier_transform(circuit, {0, 1});
//         mqis::simulate(circuit, state);
// 
//         REQUIRE(mqis::almost_eq(state, expected));
//     }

//     // clang-format off
//     auto pair = GENERATE_COPY(
//         TestPair {"00", mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, { norm,  0.0}, { norm,   0.0}} }},
//         TestPair {"10", mqis::QuantumState { {{ norm,  0.0}, { norm,  0.0}, {-norm,  0.0}, {-norm,   0.0}} }},
//         TestPair {"01", mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0, norm}, {  0.0, -norm}} }},
//         TestPair {"11", mqis::QuantumState { {{ norm,  0.0}, {-norm,  0.0}, {  0.0,-norm}, {  0.0,  norm}} }}
//     );
//     // clang-format on

    // |10> -> (1/2) [|00> + |10> - |01> - |11>]
}

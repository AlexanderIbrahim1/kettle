#include <cmath>
#include <cstddef>
#include <random>
#include <tuple>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

/*
    Here are some commonly encountered trig function evaluations in the latter unit tests
*/
const auto SIN_PI_4 = std::sin(M_PI / 4.0);
const auto SIN_PI_8 = std::sin(M_PI / 8.0);
const auto SIN_PI_16 = std::sin(M_PI / 16.0);
const auto COS_PI_4 = std::cos(M_PI / 4.0);
const auto COS_PI_8 = std::cos(M_PI / 8.0);
const auto COS_PI_16 = std::cos(M_PI / 16.0);

auto generate_random_double(double left, double right) -> double
{
    auto device = std::random_device {};
    auto prng = std::mt19937 {device()};

    auto unif = std::uniform_real_distribution {left, right};

    return unif(prng);
}

auto simulate_single_qubit_with_ugate(
    const std::string& initial_state,
    const std::vector<std::tuple<mqis::Matrix2X2, std::size_t>>& matrices,
    std::size_t n_qubits
) -> mqis::QuantumState
{
    auto state = mqis::QuantumState {initial_state};

    auto circuit = mqis::QuantumCircuit {n_qubits};

    for (const auto& [matrix, target_index] : matrices) {
        circuit.add_u_gate(matrix, target_index);
    }

    mqis::simulate(circuit, state);

    return state;
}

auto simulate_single_qubit_with_builtin(
    const std::string& initial_state,
    const std::vector<std::tuple<std::string, double, std::size_t>>& gates_and_angles,
    std::size_t n_qubits
) -> mqis::QuantumState
{
    auto state = mqis::QuantumState {initial_state};

    auto circuit = mqis::QuantumCircuit {n_qubits};

    for (const auto& [gate_id, angle, target_index] : gates_and_angles) {
        if (gate_id == "H") {
            circuit.add_h_gate(target_index);
        }
        else if (gate_id == "X") {
            circuit.add_x_gate(target_index);
        }
        else if (gate_id == "RX") {
            circuit.add_rx_gate(angle, target_index);
        }
        else {
            throw std::runtime_error {"invalid gate entered into unit test"};
        }
    }

    mqis::simulate(circuit, state);

    return state;
}

auto simulate_double_qubit_with_ugate(
    const std::string& initial_state,
    const std::vector<std::tuple<mqis::Matrix2X2, std::size_t, std::size_t>>& matrices,
    std::size_t n_qubits
) -> mqis::QuantumState
{
    auto state = mqis::QuantumState {initial_state};

    auto circuit = mqis::QuantumCircuit {n_qubits};

    for (const auto& [matrix, control_index, target_index] : matrices) {
        circuit.add_cu_gate(matrix, control_index, target_index);
    }

    mqis::simulate(circuit, state);

    return state;
}

auto simulate_double_qubit_with_builtin(
    const std::string& initial_state,
    const std::vector<std::tuple<std::string, double, std::size_t, std::size_t>>& gates_and_angles,
    std::size_t n_qubits
) -> mqis::QuantumState
{
    auto state = mqis::QuantumState {initial_state};

    auto circuit = mqis::QuantumCircuit {n_qubits};

    for (const auto& [gate_id, angle, control_index, target_index] : gates_and_angles) {
        if (gate_id == "CX") {
            circuit.add_cx_gate(control_index, target_index);
        }
        else if (gate_id == "CRX") {
            circuit.add_crx_gate(angle, control_index, target_index);
        }
        else if (gate_id == "CP") {
            circuit.add_cp_gate(angle, control_index, target_index);
        }
        else {
            throw std::runtime_error {"invalid gate entered into unit test"};
        }
    }

    mqis::simulate(circuit, state);

    return state;
}

TEST_CASE("simulate X gate")
{
    SECTION("one qubit, X(0)")
    {
        // expectation
        //
        // |0> -> |1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)")
    {
        // expectation
        //
        // |00> -> |01>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)X(0)")
    {
        // expectation
        //
        // |00> -> |10> -> |11>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);
        circuit.add_x_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate H gate")
{
    SECTION("one qubit, H(0)")
    {
        // expectation
        //
        // |0> -> (1/sqrt2)|0> + (1/sqrt2)|1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2)|00> + (1/sqrt2)|10>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |01>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        //      -> (1/2) |00> + (1/2) |01> + (1/2) |10> + (1/2) |11>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate RX gate")
{
    SECTION("one qubit, RX(pi/4, 0)")
    {
        // expectation
        //
        // |0> -> cos(pi/8) |0> - i sin(pi/8)) |1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(M_PI / 4.0, 0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{COS_PI_8, 0.0}, {0.0, -SIN_PI_8}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, RX(pi/4, 0)")
    {
        // expectation
        //
        // |00> -> cos(pi/8) |00> - i sin(pi/8)) |10>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(M_PI / 4.0, 0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{COS_PI_8, 0.0}, {0.0, -SIN_PI_8}, {0.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("two qubits, RX(pi/8, 1)RX(pi/4, 0)")
    {
        // expectation
        //
        // |00> -> cos(pi/8) |00> - i sin(pi/8)) |10>
        //      -> cos(pi/8)        [cos(pi/16) |00> - i sin(pi/16) |01>]
        //       + (- i sin(pi/8))) [cos(pi/16) |10> - i sin(pi/16) |11>]
        //
        //      =   cos(pi/8) cos(pi/16) |00>
        //      - i sin(pi/8) cos(pi/16) |10>
        //      - i cos(pi/8) sin(pi/16) |01>
        //      -   sin(pi/8) sin(pi/16) |11>
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(M_PI / 4.0, 0);
        circuit.add_rx_gate(M_PI / 8.0, 1);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{COS_PI_8 * COS_PI_16, 0.0},
             {0.0, -SIN_PI_8 * COS_PI_16},
             {0.0, -COS_PI_8 * SIN_PI_16},
             {-SIN_PI_8 * SIN_PI_16, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate RY gate")
{
    SECTION("one qubit, RY(pi/4, 0)")
    {
        // expectation
        //
        // |0> -> cos(pi/8) |0> + sin(pi/8)) |1>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_ry_gate(M_PI / 4.0, 0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{COS_PI_8, 0.0}, {SIN_PI_8, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("one qubit, random angles")
    {
        // expectation
        //
        // |0> -> cos(theta/2) |0> + sin(theta/2)) |1>
        for (std::size_t i {0}; i < 5; ++i) {
            const auto n_qubits = 1;
            auto circuit = mqis::QuantumCircuit {n_qubits};

            const auto angle = generate_random_double(-2.0 * M_PI, 2.0 * M_PI);
            circuit.add_ry_gate(angle, 0);

            auto state = mqis::QuantumState {n_qubits};
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{std::cos(angle / 2.0), 0.0}, {std::sin(angle / 2.0), 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }
    }
}

TEST_CASE("simulate RZ gate")
{
    SECTION("one qubit, RZ(pi/4, 0)")
    {
        // expectation
        //
        // |0> -> [cos(pi/8) - i sin(pi/8)] |0>
        const auto n_qubits = 1;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_rz_gate(M_PI / 4.0, 0);

        auto state = mqis::QuantumState {n_qubits};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{COS_PI_8, -SIN_PI_8}, {0.0, 0.0}}
        };
        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("one qubit, random angles")
    {
        // expectation
        //
        // |0> -> [cos(angle/2) - i sin(angle/2)] |0>
        for (std::size_t i {0}; i < 5; ++i) {
            const auto n_qubits = 1;
            auto circuit = mqis::QuantumCircuit {n_qubits};

            const auto angle = generate_random_double(-2.0 * M_PI, 2.0 * M_PI);
            circuit.add_rz_gate(angle, 0);

            auto state = mqis::QuantumState {n_qubits};
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{std::cos(angle / 2.0), -std::sin(angle / 2.0)}, {0.0, 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }
    }

    SECTION("two qubits, random angles applied to qubit 0 or 1")
    {
        // expectation
        //
        // |00> -> [cos(angle/2) - i sin(angle/2)] |00>
        //
        // it shouldn't matter which qubit it is applied to: |00> is an eigenstate of RZ
        for (std::size_t i {0}; i < 5; ++i) {
            const auto n_qubits = 2;
            const auto angle = generate_random_double(-2.0 * M_PI, 2.0 * M_PI);

            auto circuit0 = mqis::QuantumCircuit {n_qubits};
            circuit0.add_rz_gate(angle, 0);

            auto state0 = mqis::QuantumState {n_qubits};
            mqis::simulate(circuit0, state0);

            auto circuit1 = mqis::QuantumCircuit {n_qubits};
            circuit1.add_rz_gate(angle, 1);

            auto state1 = mqis::QuantumState {n_qubits};
            mqis::simulate(circuit1, state1);

            const auto expected_state = mqis::QuantumState {
                {{std::cos(angle / 2.0), -std::sin(angle / 2.0)}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };

            REQUIRE(mqis::almost_eq(state0, expected_state));
            REQUIRE(mqis::almost_eq(state1, expected_state));
        }
    }
}

TEST_CASE("simulate CX gate")
{
    struct InputAndOutput
    {
        mqis::QuantumState state;
        mqis::QuantumState expected;
    };

    // expectation is that qubit 1 flips if qubit 0 is set
    SECTION("two qubits")
    {
        auto circuit = mqis::QuantumCircuit {2};

        SECTION("CX(source=0, target=1)")
        {
            circuit.add_cx_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {mqis::QuantumState {"00"}, mqis::QuantumState {"00"}},
                InputAndOutput {mqis::QuantumState {"01"}, mqis::QuantumState {"01"}},
                InputAndOutput {mqis::QuantumState {"10"}, mqis::QuantumState {"11"}},
                InputAndOutput {mqis::QuantumState {"11"}, mqis::QuantumState {"10"}}
            );

            mqis::simulate(circuit, pair.state);
            REQUIRE(mqis::almost_eq(pair.expected, pair.state));
        }

        SECTION("CX(source=1, target=0)")
        {
            circuit.add_cx_gate(1, 0);

            auto pair = GENERATE(
                InputAndOutput {mqis::QuantumState {"00"}, mqis::QuantumState {"00"}},
                InputAndOutput {mqis::QuantumState {"01"}, mqis::QuantumState {"11"}},
                InputAndOutput {mqis::QuantumState {"10"}, mqis::QuantumState {"10"}},
                InputAndOutput {mqis::QuantumState {"11"}, mqis::QuantumState {"01"}}
            );

            mqis::simulate(circuit, pair.state);
            REQUIRE(mqis::almost_eq(pair.expected, pair.state));
        }
    }

    SECTION("three qubits")
    {
        auto circuit = mqis::QuantumCircuit {3};

        SECTION("CX(source=0, target=1)")
        {
            circuit.add_cx_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {mqis::QuantumState {"000"}, mqis::QuantumState {"000"}},
                InputAndOutput {mqis::QuantumState {"100"}, mqis::QuantumState {"110"}},
                InputAndOutput {mqis::QuantumState {"010"}, mqis::QuantumState {"010"}},
                InputAndOutput {mqis::QuantumState {"110"}, mqis::QuantumState {"100"}},
                InputAndOutput {mqis::QuantumState {"001"}, mqis::QuantumState {"001"}},
                InputAndOutput {mqis::QuantumState {"101"}, mqis::QuantumState {"111"}},
                InputAndOutput {mqis::QuantumState {"011"}, mqis::QuantumState {"011"}},
                InputAndOutput {mqis::QuantumState {"111"}, mqis::QuantumState {"101"}}
            );

            mqis::simulate(circuit, pair.state);
            REQUIRE(mqis::almost_eq(pair.expected, pair.state));
        }

        SECTION("CX(source=0, target=2)")
        {
            circuit.add_cx_gate(0, 2);

            auto pair = GENERATE(
                InputAndOutput {mqis::QuantumState {"000"}, mqis::QuantumState {"000"}},
                InputAndOutput {mqis::QuantumState {"100"}, mqis::QuantumState {"101"}},
                InputAndOutput {mqis::QuantumState {"010"}, mqis::QuantumState {"010"}},
                InputAndOutput {mqis::QuantumState {"110"}, mqis::QuantumState {"111"}},
                InputAndOutput {mqis::QuantumState {"001"}, mqis::QuantumState {"001"}},
                InputAndOutput {mqis::QuantumState {"101"}, mqis::QuantumState {"100"}},
                InputAndOutput {mqis::QuantumState {"011"}, mqis::QuantumState {"011"}},
                InputAndOutput {mqis::QuantumState {"111"}, mqis::QuantumState {"110"}}
            );

            mqis::simulate(circuit, pair.state);
            REQUIRE(mqis::almost_eq(pair.expected, pair.state));
        }
    }
}

TEST_CASE("simulate H and CX gates")
{
    SECTION("2 qubits, CX(0, 1) H(0)")
    {
        // expectation
        // APPLY H(0)     : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY CX(0, 1) :      -> (1/sqrt2) |00> + (1/sqrt2) |11>
        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_cx_gate(0, 1);

        auto state = mqis::QuantumState {"00"};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}},
            mqis::QuantumStateEndian::LITTLE
        };

        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("2 qubits, CX(0, 1) CX(1, 0) CX(0, 1) X(0)")
    {
        // expectation
        // APPLY X(0)     : |00> -> |10>
        // APPLY CX(0, 1) :      -> |11>
        // APPLY CX(1, 0) :      -> |01>
        // APPLY CX(0, 1) :      -> |01>
        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_x_gate(0);
        circuit.add_cx_gate(0, 1);
        circuit.add_cx_gate(1, 0);
        circuit.add_cx_gate(0, 1);

        auto state = mqis::QuantumState {"00"};
        mqis::simulate(circuit, state);

        const auto expected_state = mqis::QuantumState {"01"};

        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate CRX gate")
{
    SECTION("computational basis states, 2 qubits")
    {
        struct TestPair
        {
            std::string input;
            mqis::QuantumState expected;
        };

        // clang-format off
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        auto pair = GENERATE_COPY(
            TestPair {"00", mqis::QuantumState { {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"10", mqis::QuantumState { {{0.0, 0.0}, {cost, 0.0}, {0.0, 0.0}, {0.0, -sint}} }},
            TestPair {"01", mqis::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"11", mqis::QuantumState { {{0.0, 0.0}, {0.0, -sint}, {0.0, 0.0}, {cost, 0.0}} }}
        );
        // clang-format on

        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_crx_gate(angle, 0, 1);

        auto state = mqis::QuantumState {pair.input, mqis::QuantumStateEndian::LITTLE};
        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, pair.expected));
    }

    SECTION("2 qubits, CRX(t, 0, 1) H(0)")
    {
        // expectation
        // APPLY H(0)         : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY CRX(t, 0, 1) :      -> (1/sqrt2) |00> + (1/sqrt2) cos(t/2) |10> - (i/sqrt2) sint(t/2) |11>
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_crx_gate(angle, 0, 1);

        auto state = mqis::QuantumState {"00"};
        mqis::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2 * cost, 0.0}, {0.0, 0.0}, {0.0, -M_SQRT1_2 * sint}},
            mqis::QuantumStateEndian::LITTLE
        };

        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate CRZ gate")
{
    SECTION("2 qubits, CRZ(t, 0, 1) H(0)")
    {
        // expectation
        // APPLY H(0)         : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY CRZ(t, 0, 1) :      -> (1/sqrt2) |00> + (1/sqrt2) exp(-i t/2) |10>
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_crz_gate(angle, 0, 1);

        auto state = mqis::QuantumState {"00"};
        mqis::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = mqis::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2 * cost, -M_SQRT1_2 * sint}, {0.0, 0.0}, {0.0, 0.0}},
            mqis::QuantumStateEndian::LITTLE
        };

        REQUIRE(mqis::almost_eq(state, expected_state));
    }

    SECTION("2 qubits, evenly spaced")
    {
        // expectation
        // APPLY H(0)         : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY H(1)         :      -> (1/2) [|00> + |10> + |01> + |11>]
        // APPLY CRZ(t, 0, 1) :      -> (1/2) [|00> + exp(-i t/2) |10> + |10> + exp(i t/2) |11>]
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);
        circuit.add_crz_gate(angle, 0, 1);

        auto state = mqis::QuantumState {"00"};
        mqis::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = mqis::QuantumState {
            {{0.5, 0.0}, {0.5 * cost, -0.5 * sint}, {0.5, 0.0}, {0.5 * cost, 0.5 * sint}},
            mqis::QuantumStateEndian::LITTLE
        };

        REQUIRE(mqis::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate CP gate")
{
    SECTION("computational basis")
    {
        struct TestPair
        {
            std::string input;
            mqis::QuantumState expected;
        };

        // clang-format off
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        const auto cost = std::cos(angle);
        const auto sint = std::sin(angle);

        auto pair = GENERATE_COPY(
            TestPair {"00", mqis::QuantumState { {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"10", mqis::QuantumState { {{0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"01", mqis::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"11", mqis::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {cost, sint}} }}
        );
        // clang-format on

        auto circuit = mqis::QuantumCircuit {2};
        circuit.add_cp_gate(angle, 0, 1);

        auto state = mqis::QuantumState {pair.input};
        mqis::simulate(circuit, state);

        REQUIRE(mqis::almost_eq(state, pair.expected));
    }
}

TEST_CASE("simulate U gate")
{
    // clang-format off
    const std::string initial_state = GENERATE(
        "0", "1",
        "00", "10", "01", "11",
        "000", "100", "010", "110", "001", "101", "011", "111"
    );

    const auto n_qubits = initial_state.size();

    const auto h_matrix = mqis::Matrix2X2 {
        {M_SQRT1_2,  0.0},
        {M_SQRT1_2,  0.0},
        {M_SQRT1_2,  0.0},
        {-M_SQRT1_2, 0.0}
    };

    const auto x_matrix = mqis::Matrix2X2 {
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 0.0},
        {0.0, 0.0}
    };

    const auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 0.75 * M_PI, M_PI, 1.25 * M_PI, 2.0 * M_PI);

    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    const auto rx_matrix = mqis::Matrix2X2 {
        {cost, 0.0  },
        {0.0,  -sint},
        {0.0,  -sint},
        {cost, 0.0  }
    };
    // clang-format on

    SECTION("H gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {h_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"H", 0.0, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("two H gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {h_matrix, 0},
                {h_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"H", 0.0, 0},
                {"H", 0.0, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("X gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {x_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"X", 0.0, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("two X gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {x_matrix, 0},
                {x_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"X", 0.0, 0},
                {"X", 0.0, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("RX gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {rx_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"RX", angle, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("two RX gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {rx_matrix, 0},
                {rx_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"RX", angle, 0},
                {"RX", angle, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("H and then X and then RX")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(
            initial_state,
            {
                {h_matrix,  0},
                {x_matrix,  0},
                {rx_matrix, 0}
        },
            n_qubits
        );
        const auto state_from_builtin = simulate_single_qubit_with_builtin(
            initial_state,
            {
                {"H",  0.0,   0},
                {"X",  0.0,   0},
                {"RX", angle, 0}
        },
            n_qubits
        );

        REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
    }
}

TEST_CASE("simulate CU gate")
{
    const auto x_matrix = mqis::Matrix2X2 {
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 0.0},
        {0.0, 0.0}
    };

    const auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 0.75 * M_PI, M_PI, 1.25 * M_PI, 2.0 * M_PI);

    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    const auto rx_matrix = mqis::Matrix2X2 {
        {cost, 0.0  },
        {0.0,  -sint},
        {0.0,  -sint},
        {cost, 0.0  }
    };

    const auto p_matrix = mqis::Matrix2X2 {
        {1.0,             0.0            },
        {0.0,             0.0            },
        {0.0,             0.0            },
        {std::cos(angle), std::sin(angle)},
    };

    SECTION("2-qubit circuits")
    {
        using CTPair = std::pair<std::size_t, std::size_t>;

        const auto [control_qubit, target_qubit] = GENERATE(CTPair {0, 1}, CTPair {1, 0});
        const std::string initial_state = GENERATE("00", "10", "01", "11");
        const auto n_qubits = initial_state.size();

        SECTION("CX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {x_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CX", 0.0, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CRX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {rx_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CRX", angle, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CP gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {p_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CP", angle, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }

        // clang-format off
        SECTION("CX then CRX")
        {
            const auto first_control_qubit = control_qubit;
            const auto first_target_qubit = target_qubit;
            const auto [second_control_qubit, second_target_qubit] = GENERATE_COPY(
                CTPair {control_qubit, target_qubit},
                CTPair {target_qubit, control_qubit}
            );

            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {x_matrix, first_control_qubit, first_target_qubit},
                    {rx_matrix, second_control_qubit, second_target_qubit}
                },
                n_qubits
            );

            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CX", 0.0, first_control_qubit, first_target_qubit},
                    {"CRX", angle, second_control_qubit, second_target_qubit}
                },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }
        // clang-format on
    }

    SECTION("3-qubit circuits")
    {
        using CTPair = std::pair<std::size_t, std::size_t>;

        const auto [control_qubit, target_qubit] =
            GENERATE(CTPair {0, 1}, CTPair {1, 0}, CTPair {0, 2}, CTPair {2, 0}, CTPair {1, 2}, CTPair {2, 1});
        const std::string initial_state = GENERATE("000", "100", "010", "110", "001", "101", "011", "111");
        const auto n_qubits = initial_state.size();

        SECTION("CX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {x_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CX", 0.0, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CRX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {rx_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CRX", angle, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CP gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(
                initial_state,
                {
                    {p_matrix, control_qubit, target_qubit}
            },
                n_qubits
            );
            const auto state_from_builtin = simulate_double_qubit_with_builtin(
                initial_state,
                {
                    {"CP", angle, control_qubit, target_qubit}
            },
                n_qubits
            );

            REQUIRE(mqis::almost_eq(state_from_matrix, state_from_builtin));
        }
    }
}

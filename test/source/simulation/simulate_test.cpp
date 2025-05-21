#include <cmath>
#include <cstddef>
#include <random>
#include <tuple>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/simulation/simulate.hpp"
#include "kettle/state/state.hpp"

/*
    Here are some commonly encountered trig function evaluations in the latter unit tests
*/
const auto SIN_PI_4 = std::sin(M_PI / 4.0);
const auto SIN_PI_8 = std::sin(M_PI / 8.0);
const auto SIN_PI_16 = std::sin(M_PI / 16.0);
const auto COS_PI_4 = std::cos(M_PI / 4.0);
const auto COS_PI_8 = std::cos(M_PI / 8.0);
const auto COS_PI_16 = std::cos(M_PI / 16.0);

static auto generate_random_double(double left, double right) -> double
{
    auto device = std::random_device {};
    auto prng = std::mt19937 {device()};

    auto unif = std::uniform_real_distribution {left, right};

    return unif(prng);
}

static auto simulate_single_qubit_with_ugate(
    const std::string& initial_state,
    const std::vector<std::tuple<ket::Matrix2X2, std::size_t>>& matrices,
    std::size_t n_qubits
) -> ket::QuantumState
{
    auto state = ket::QuantumState {initial_state};

    auto circuit = ket::QuantumCircuit {n_qubits};

    for (const auto& [matrix, target_index] : matrices) {
        circuit.add_u_gate(matrix, target_index);
    }

    ket::simulate(circuit, state);

    return state;
}

static auto simulate_single_qubit_with_builtin(
    const std::string& initial_state,
    const std::vector<std::tuple<std::string, double, std::size_t>>& gates_and_angles,
    std::size_t n_qubits
) -> ket::QuantumState
{
    auto state = ket::QuantumState {initial_state};

    auto circuit = ket::QuantumCircuit {n_qubits};

    for (const auto& [gate_id, angle, target_index] : gates_and_angles) {
        if (gate_id == "H") {
            circuit.add_h_gate(target_index);
        }
        else if (gate_id == "X") {
            circuit.add_x_gate(target_index);
        }
        else if (gate_id == "Y") {
            circuit.add_y_gate(target_index);
        }
        else if (gate_id == "Z") {
            circuit.add_z_gate(target_index);
        }
        else if (gate_id == "S") {
            circuit.add_s_gate(target_index);
        }
        else if (gate_id == "T") {
            circuit.add_t_gate(target_index);
        }
        else if (gate_id == "SX") {
            circuit.add_sx_gate(target_index);
        }
        else if (gate_id == "RX") {
            circuit.add_rx_gate(target_index, angle);
        }
        else {
            throw std::runtime_error {"invalid gate entered into unit test"};
        }
    }

    ket::simulate(circuit, state);

    return state;
}

static auto simulate_double_qubit_with_ugate(
    const std::string& initial_state,
    const std::vector<std::tuple<ket::Matrix2X2, std::size_t, std::size_t>>& matrices,
    std::size_t n_qubits
) -> ket::QuantumState
{
    auto state = ket::QuantumState {initial_state};

    auto circuit = ket::QuantumCircuit {n_qubits};

    for (const auto& [matrix, control_index, target_index] : matrices) {
        circuit.add_cu_gate(matrix, control_index, target_index);
    }

    ket::simulate(circuit, state);

    return state;
}

static auto simulate_double_qubit_with_builtin(
    const std::string& initial_state,
    const std::vector<std::tuple<std::string, double, std::size_t, std::size_t>>& gates_and_angles,
    std::size_t n_qubits
) -> ket::QuantumState
{
    auto state = ket::QuantumState {initial_state};

    auto circuit = ket::QuantumCircuit {n_qubits};

    for (const auto& [gate_id, angle, control_index, target_index] : gates_and_angles) {
        if (gate_id == "CH") {
            circuit.add_ch_gate(control_index, target_index);
        }
        else if (gate_id == "CX") {
            circuit.add_cx_gate(control_index, target_index);
        }
        else if (gate_id == "CY") {
            circuit.add_cy_gate(control_index, target_index);
        }
        else if (gate_id == "CZ") {
            circuit.add_cz_gate(control_index, target_index);
        }
        else if (gate_id == "CS") {
            circuit.add_cs_gate(control_index, target_index);
        }
        else if (gate_id == "CT") {
            circuit.add_ct_gate(control_index, target_index);
        }
        else if (gate_id == "CSX") {
            circuit.add_csx_gate(control_index, target_index);
        }
        else if (gate_id == "CRX") {
            circuit.add_crx_gate(control_index, target_index, angle);
        }
        else if (gate_id == "CP") {
            circuit.add_cp_gate(control_index, target_index, angle);
        }
        else {
            throw std::runtime_error {"invalid gate entered into unit test"};
        }
    }

    ket::simulate(circuit, state);

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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)")
    {
        // expectation
        //
        // |00> -> |01>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_x_gate(1);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, X(1)X(0)")
    {
        // expectation
        //
        // |00> -> |10> -> |11>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_x_gate(0);
        circuit.add_x_gate(1);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2)|00> + (1/sqrt2)|10>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |01>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_h_gate(1);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, H(1)H(0)")
    {
        // expectation
        //
        // |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        //      -> (1/2) |00> + (1/2) |01> + (1/2) |10> + (1/2) |11>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(0, M_PI / 4.0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{COS_PI_8, 0.0}, {0.0, -SIN_PI_8}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("two qubits, RX(pi/4, 0)")
    {
        // expectation
        //
        // |00> -> cos(pi/8) |00> - i sin(pi/8)) |10>
        const auto n_qubits = 2;

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(0, M_PI / 4.0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{COS_PI_8, 0.0}, {0.0, -SIN_PI_8}, {0.0, 0.0}, {0.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_rx_gate(0, M_PI / 4.0);
        circuit.add_rx_gate(1, M_PI / 8.0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{COS_PI_8 * COS_PI_16, 0.0},
             {0.0, -SIN_PI_8 * COS_PI_16},
             {0.0, -COS_PI_8 * SIN_PI_16},
             {-SIN_PI_8 * SIN_PI_16, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_ry_gate(0, M_PI / 4.0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{COS_PI_8, 0.0}, {SIN_PI_8, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("one qubit, random angles")
    {
        // expectation
        //
        // |0> -> cos(theta/2) |0> + sin(theta/2)) |1>
        for (std::size_t i {0}; i < 5; ++i) {
            const auto n_qubits = 1;
            auto circuit = ket::QuantumCircuit {n_qubits};

            const auto angle = generate_random_double(-2.0 * M_PI, 2.0 * M_PI);
            circuit.add_ry_gate(0, angle);

            auto state = ket::QuantumState {n_qubits};
            ket::simulate(circuit, state);

            const auto expected_state = ket::QuantumState {
                {{std::cos(angle / 2.0), 0.0}, {std::sin(angle / 2.0), 0.0}}
            };
            REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {n_qubits};
        circuit.add_rz_gate(0, M_PI / 4.0);

        auto state = ket::QuantumState {n_qubits};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{COS_PI_8, -SIN_PI_8}, {0.0, 0.0}}
        };
        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("one qubit, random angles")
    {
        // expectation
        //
        // |0> -> [cos(angle/2) - i sin(angle/2)] |0>
        for (std::size_t i {0}; i < 5; ++i) {
            const auto n_qubits = 1;
            auto circuit = ket::QuantumCircuit {n_qubits};

            const auto angle = generate_random_double(-2.0 * M_PI, 2.0 * M_PI);
            circuit.add_rz_gate(0, angle);

            auto state = ket::QuantumState {n_qubits};
            ket::simulate(circuit, state);

            const auto expected_state = ket::QuantumState {
                {{std::cos(angle / 2.0), -std::sin(angle / 2.0)}, {0.0, 0.0}}
            };
            REQUIRE(ket::almost_eq(state, expected_state));
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

            auto circuit0 = ket::QuantumCircuit {n_qubits};
            circuit0.add_rz_gate(0, angle);

            auto state0 = ket::QuantumState {n_qubits};
            ket::simulate(circuit0, state0);

            auto circuit1 = ket::QuantumCircuit {n_qubits};
            circuit1.add_rz_gate(1, angle);

            auto state1 = ket::QuantumState {n_qubits};
            ket::simulate(circuit1, state1);

            const auto expected_state = ket::QuantumState {
                {{std::cos(angle / 2.0), -std::sin(angle / 2.0)}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };

            REQUIRE(ket::almost_eq(state0, expected_state));
            REQUIRE(ket::almost_eq(state1, expected_state));
        }
    }
}

TEST_CASE("simulate CX gate")
{
    struct InputAndOutput
    {
        ket::QuantumState state;
        ket::QuantumState expected;
    };

    // expectation is that qubit 1 flips if qubit 0 is set
    SECTION("two qubits")
    {
        auto circuit = ket::QuantumCircuit {2};

        SECTION("CX(control=0, target=1)")
        {
            circuit.add_cx_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"00"}, ket::QuantumState {"00"}},
                InputAndOutput {ket::QuantumState {"01"}, ket::QuantumState {"01"}},
                InputAndOutput {ket::QuantumState {"10"}, ket::QuantumState {"11"}},
                InputAndOutput {ket::QuantumState {"11"}, ket::QuantumState {"10"}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }

        SECTION("CX(control=1, target=0)")
        {
            circuit.add_cx_gate(1, 0);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"00"}, ket::QuantumState {"00"}},
                InputAndOutput {ket::QuantumState {"01"}, ket::QuantumState {"11"}},
                InputAndOutput {ket::QuantumState {"10"}, ket::QuantumState {"10"}},
                InputAndOutput {ket::QuantumState {"11"}, ket::QuantumState {"01"}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }
    }

    SECTION("three qubits")
    {
        auto circuit = ket::QuantumCircuit {3};

        SECTION("CX(control=0, target=1)")
        {
            circuit.add_cx_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"000"}, ket::QuantumState {"000"}},
                InputAndOutput {ket::QuantumState {"100"}, ket::QuantumState {"110"}},
                InputAndOutput {ket::QuantumState {"010"}, ket::QuantumState {"010"}},
                InputAndOutput {ket::QuantumState {"110"}, ket::QuantumState {"100"}},
                InputAndOutput {ket::QuantumState {"001"}, ket::QuantumState {"001"}},
                InputAndOutput {ket::QuantumState {"101"}, ket::QuantumState {"111"}},
                InputAndOutput {ket::QuantumState {"011"}, ket::QuantumState {"011"}},
                InputAndOutput {ket::QuantumState {"111"}, ket::QuantumState {"101"}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }

        SECTION("CX(control=0, target=2)")
        {
            circuit.add_cx_gate(0, 2);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"000"}, ket::QuantumState {"000"}},
                InputAndOutput {ket::QuantumState {"100"}, ket::QuantumState {"101"}},
                InputAndOutput {ket::QuantumState {"010"}, ket::QuantumState {"010"}},
                InputAndOutput {ket::QuantumState {"110"}, ket::QuantumState {"111"}},
                InputAndOutput {ket::QuantumState {"001"}, ket::QuantumState {"001"}},
                InputAndOutput {ket::QuantumState {"101"}, ket::QuantumState {"100"}},
                InputAndOutput {ket::QuantumState {"011"}, ket::QuantumState {"011"}},
                InputAndOutput {ket::QuantumState {"111"}, ket::QuantumState {"110"}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }
    }
}

TEST_CASE("simulate CZ gate")
{
    struct InputAndOutput
    {
        ket::QuantumState state;
        ket::QuantumState expected;
    };

    // expectation is that qubit 1 flips if qubit 0 is set
    SECTION("two qubits")
    {
        auto circuit = ket::QuantumCircuit {2};

        SECTION("CZ(control=0, target=1)")
        {
            circuit.add_cz_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"00"}, ket::QuantumState {"00"}},
                InputAndOutput {ket::QuantumState {"01"}, ket::QuantumState {"01"}},
                InputAndOutput {ket::QuantumState {"10"}, ket::QuantumState {"10"}},
                InputAndOutput {ket::QuantumState {"11"}, ket::QuantumState {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {-1.0, 0.0}}}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }

        SECTION("CZ(control=1, target=0)")
        {
            circuit.add_cz_gate(1, 0);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"00"}, ket::QuantumState {"00"}},
                InputAndOutput {ket::QuantumState {"01"}, ket::QuantumState {"01"}},
                InputAndOutput {ket::QuantumState {"10"}, ket::QuantumState {"10"}},
                InputAndOutput {ket::QuantumState {"11"}, ket::QuantumState {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {-1.0, 0.0}}}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }
    }

    SECTION("three qubits")
    {
        auto circuit = ket::QuantumCircuit {3};

        SECTION("CZ(control=0, target=1)")
        {
            circuit.add_cz_gate(0, 1);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"000"}, ket::QuantumState {"000"}},
                InputAndOutput {ket::QuantumState {"100"}, ket::QuantumState {"100"}},
                InputAndOutput {ket::QuantumState {"010"}, ket::QuantumState {"010"}},
                InputAndOutput {ket::QuantumState {"110"}, ket::QuantumState {{{}, {}, {}, {-1.0, 0.0}, {}, {}, {}, {}}}},
                InputAndOutput {ket::QuantumState {"001"}, ket::QuantumState {"001"}},
                InputAndOutput {ket::QuantumState {"101"}, ket::QuantumState {"101"}},
                InputAndOutput {ket::QuantumState {"011"}, ket::QuantumState {"011"}},
                InputAndOutput {ket::QuantumState {"111"}, ket::QuantumState {{{}, {}, {}, {}, {}, {}, {}, {-1.0, 0.0}}}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
        }

        SECTION("CZ(control=0, target=2)")
        {
            circuit.add_cz_gate(0, 2);

            auto pair = GENERATE(
                InputAndOutput {ket::QuantumState {"000"}, ket::QuantumState {"000"}},
                InputAndOutput {ket::QuantumState {"100"}, ket::QuantumState {"100"}},
                InputAndOutput {ket::QuantumState {"010"}, ket::QuantumState {"010"}},
                InputAndOutput {ket::QuantumState {"110"}, ket::QuantumState {"110"}},
                InputAndOutput {ket::QuantumState {"001"}, ket::QuantumState {"001"}},
                InputAndOutput {ket::QuantumState {"101"}, ket::QuantumState {{{}, {}, {}, {}, {}, {-1.0, 0.0}, {}, {}}}},
                InputAndOutput {ket::QuantumState {"011"}, ket::QuantumState {"011"}},
                InputAndOutput {ket::QuantumState {"111"}, ket::QuantumState {{{}, {}, {}, {}, {}, {}, {}, {-1.0, 0.0}}}}
            );

            ket::simulate(circuit, pair.state);
            REQUIRE(ket::almost_eq(pair.expected, pair.state));
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
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_cx_gate(0, 1);

        auto state = ket::QuantumState {"00"};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {M_SQRT1_2, 0.0}},
            ket::QuantumStateEndian::LITTLE
        };

        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("2 qubits, CX(0, 1) CX(1, 0) CX(0, 1) X(0)")
    {
        // expectation
        // APPLY X(0)     : |00> -> |10>
        // APPLY CX(0, 1) :      -> |11>
        // APPLY CX(1, 0) :      -> |01>
        // APPLY CX(0, 1) :      -> |01>
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_x_gate(0);
        circuit.add_cx_gate(0, 1);
        circuit.add_cx_gate(1, 0);
        circuit.add_cx_gate(0, 1);

        auto state = ket::QuantumState {"00"};
        ket::simulate(circuit, state);

        const auto expected_state = ket::QuantumState {"01"};

        REQUIRE(ket::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate CRX gate")
{
    SECTION("computational basis states, 2 qubits")
    {
        struct TestPair
        {
            std::string input;
            ket::QuantumState expected;
        };

        // clang-format off
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        auto pair = GENERATE_COPY(
            TestPair {"00", ket::QuantumState { {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"10", ket::QuantumState { {{0.0, 0.0}, {cost, 0.0}, {0.0, 0.0}, {0.0, -sint}} }},
            TestPair {"01", ket::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"11", ket::QuantumState { {{0.0, 0.0}, {0.0, -sint}, {0.0, 0.0}, {cost, 0.0}} }}
        );
        // clang-format on

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_crx_gate(0, 1, angle);

        auto state = ket::QuantumState {pair.input, ket::QuantumStateEndian::LITTLE};
        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, pair.expected));
    }

    SECTION("2 qubits, CRX(t, 0, 1) H(0)")
    {
        // expectation
        // APPLY H(0)         : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY CRX(t, 0, 1) :      -> (1/sqrt2) |00> + (1/sqrt2) cos(t/2) |10> - (i/sqrt2) sint(t/2) |11>
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_crx_gate(0, 1, angle);

        auto state = ket::QuantumState {"00"};
        ket::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2 * cost, 0.0}, {0.0, 0.0}, {0.0, -M_SQRT1_2 * sint}},
            ket::QuantumStateEndian::LITTLE
        };

        REQUIRE(ket::almost_eq(state, expected_state));
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

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_crz_gate(0, 1, angle);

        auto state = ket::QuantumState {"00"};
        ket::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = ket::QuantumState {
            {{M_SQRT1_2, 0.0}, {M_SQRT1_2 * cost, -M_SQRT1_2 * sint}, {0.0, 0.0}, {0.0, 0.0}},
            ket::QuantumStateEndian::LITTLE
        };

        REQUIRE(ket::almost_eq(state, expected_state));
    }

    SECTION("2 qubits, evenly spaced")
    {
        // expectation
        // APPLY H(0)         : |00> -> (1/sqrt2) |00> + (1/sqrt2) |10>
        // APPLY H(1)         :      -> (1/2) [|00> + |10> + |01> + |11>]
        // APPLY CRZ(t, 0, 1) :      -> (1/2) [|00> + exp(-i t/2) |10> + |10> + exp(i t/2) |11>]
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);
        circuit.add_crz_gate(0, 1, angle);

        auto state = ket::QuantumState {"00"};
        ket::simulate(circuit, state);

        const auto cost = std::cos(angle / 2.0);
        const auto sint = std::sin(angle / 2.0);

        const auto expected_state = ket::QuantumState {
            {{0.5, 0.0}, {0.5 * cost, -0.5 * sint}, {0.5, 0.0}, {0.5 * cost, 0.5 * sint}},
            ket::QuantumStateEndian::LITTLE
        };

        REQUIRE(ket::almost_eq(state, expected_state));
    }
}

TEST_CASE("simulate CP gate")
{
    SECTION("computational basis")
    {
        struct TestPair
        {
            std::string input;
            ket::QuantumState expected;
        };

        // clang-format off
        auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI / 1.5, 0.99 * M_PI, M_PI);

        const auto cost = std::cos(angle);
        const auto sint = std::sin(angle);

        auto pair = GENERATE_COPY(
            TestPair {"00", ket::QuantumState { {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"10", ket::QuantumState { {{0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"01", ket::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}} }},
            TestPair {"11", ket::QuantumState { {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {cost, sint}} }}
        );
        // clang-format on

        auto circuit = ket::QuantumCircuit {2};
        circuit.add_cp_gate(0, 1, angle);

        auto state = ket::QuantumState {pair.input};
        ket::simulate(circuit, state);

        REQUIRE(ket::almost_eq(state, pair.expected));
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

    SECTION("H gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::h_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"H", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("two H gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::h_gate(), 0}, {ket::h_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"H", 0.0, 0}, {"H", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("X gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::x_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"X", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("two X gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::x_gate(), 0}, {ket::x_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"X", 0.0, 0}, {"X", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("Y gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::y_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"Y", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("Z gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::z_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"Z", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("S gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::s_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"S", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("T gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::t_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"T", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("SX gate mimic")
    {
        const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::sx_gate(), 0}}, n_qubits);
        const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"SX", 0.0, 0}}, n_qubits);

        REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
    }

    SECTION("simulate U gate with angles")
    {
        const auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 0.75 * M_PI, M_PI, 1.25 * M_PI, 2.0 * M_PI);

        SECTION("RX gate mimic")
        {
            const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::rx_gate(angle), 0}}, n_qubits);
            const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"RX", angle, 0}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("two RX gate mimic")
        {
            const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::rx_gate(angle), 0}, {ket::rx_gate(angle), 0}}, n_qubits);
            const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"RX", angle, 0}, {"RX", angle, 0}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("H and then X and then RX")
        {
            const auto state_from_matrix = simulate_single_qubit_with_ugate(initial_state, {{ket::h_gate(), 0}, {ket::x_gate(), 0}, {ket::rx_gate(angle), 0}}, n_qubits);
            const auto state_from_builtin = simulate_single_qubit_with_builtin(initial_state, {{"H", 0.0, 0}, {"X", 0.0, 0}, {"RX", angle, 0}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }
    }
    // clang-format on
}

TEST_CASE("simulate CU gate")
{
    // clang-format off

    SECTION("2-qubit circuits")
    {
        using CTPair = std::pair<std::size_t, std::size_t>;

        const auto [control_qubit, target_qubit] = GENERATE(CTPair {0, 1}, CTPair {1, 0});
        const std::string initial_state = GENERATE("00", "10", "01", "11");
        const auto n_qubits = initial_state.size();

        SECTION("CH gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::h_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CH", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::x_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CX", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CY gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::y_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CY", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CZ gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::z_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CZ", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CS gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::s_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CS", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CT gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::t_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CT", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("CSX gate mimic")
        {
            const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::sx_gate(), control_qubit, target_qubit}}, n_qubits);
            const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CSX", 0.0, control_qubit, target_qubit}}, n_qubits);

            REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
        }

        SECTION("simulate 2-qubit CU with angles")
        {
            const auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 0.75 * M_PI, M_PI, 1.25 * M_PI, 2.0 * M_PI);

            SECTION("CRX gate mimic")
            {
                const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::rx_gate(angle), control_qubit, target_qubit}}, n_qubits);
                const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CRX", angle, control_qubit, target_qubit}}, n_qubits);

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }

            SECTION("CP gate mimic")
            {
                const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::p_gate(angle), control_qubit, target_qubit}}, n_qubits);
                const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CP", angle, control_qubit, target_qubit}}, n_qubits);

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }

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
                        {ket::x_gate(), first_control_qubit, first_target_qubit},
                        {ket::rx_gate(angle), second_control_qubit, second_target_qubit}
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

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }
        }
    }

    SECTION("3-qubit circuits")
    {
        using CTPair = std::pair<std::size_t, std::size_t>;

        const auto [control_qubit, target_qubit] = GENERATE(
            CTPair {0, 1}, CTPair {1, 0}, CTPair {0, 2}, CTPair {2, 0}, CTPair {1, 2}, CTPair {2, 1}
        );
        const std::string initial_state = GENERATE("000", "100", "010", "110", "001", "101", "011", "111");
        const auto n_qubits = initial_state.size();

        SECTION("simulate 3-qubit CU with angles")
        {
            const auto angle = GENERATE(0.0, M_PI / 6.0, M_PI / 3.0, M_PI / 2.0, 0.75 * M_PI, M_PI, 1.25 * M_PI, 2.0 * M_PI);

            SECTION("CX gate mimic")
            {
                const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::x_gate(), control_qubit, target_qubit}}, n_qubits);
                const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CX", 0.0, control_qubit, target_qubit}}, n_qubits);

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }

            SECTION("CRX gate mimic")
            {
                const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::rx_gate(angle), control_qubit, target_qubit}}, n_qubits);
                const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CRX", angle, control_qubit, target_qubit}}, n_qubits);

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }

            SECTION("CP gate mimic")
            {
                const auto state_from_matrix = simulate_double_qubit_with_ugate(initial_state, {{ket::p_gate(angle), control_qubit, target_qubit}}, n_qubits);
                const auto state_from_builtin = simulate_double_qubit_with_builtin(initial_state, {{"CP", angle, control_qubit, target_qubit}}, n_qubits);

                REQUIRE(ket::almost_eq(state_from_matrix, state_from_builtin));
            }
        }
    }
    // clang-format off
}

TEST_CASE("Invalid simulation; circuit and state have different numbers of qubits")
{
    auto circuit = ket::QuantumCircuit {4};
    auto state = ket::QuantumState {"000"};

    REQUIRE_THROWS_AS(ket::simulate(circuit, state), std::runtime_error);
}

/*
    Begin in the |00> state, transform to the |10> state, and measure both qubits, guaranteeing
    that the 0th and 1st classical bits are 1 and 0, respectively.
*/
TEST_CASE("simulate and get classical register")
{
    SECTION("runs properly")
    {
        auto circuit = ket::QuantumCircuit {2};
        circuit.add_x_gate(0);
        circuit.add_m_gate({0, 1});

        auto statevector = ket::QuantumState {"00"};
        auto simulator = ket::StatevectorSimulator {};

        REQUIRE(!simulator.has_been_run());

        simulator.run(circuit, statevector);

        REQUIRE(simulator.has_been_run());

        const auto cregister = simulator.classical_register();
        REQUIRE(cregister.get(0) == 1);
        REQUIRE(cregister.get(1) == 0);
    }

    SECTION("throws if classical register is accessed before simulation is run")
    {
        auto simulator = ket::StatevectorSimulator {};
        REQUIRE_THROWS_AS(simulator.classical_register(), std::runtime_error);
    }
}

TEST_CASE("simulate and get statevector loggers")
{
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_statevector_circuit_logger();
    circuit.add_x_gate(0);
    circuit.add_statevector_circuit_logger();
    circuit.add_x_gate(0);
    circuit.add_statevector_circuit_logger();
    circuit.add_h_gate({0, 1});
    circuit.add_statevector_circuit_logger();

    auto statevector = ket::QuantumState {"00"};
    auto simulator = ket::StatevectorSimulator {};

    simulator.run(circuit, statevector);

    REQUIRE(simulator.has_been_run());

    const auto loggers = simulator.circuit_loggers();

    REQUIRE(loggers.size() == 4);

    const auto& logger0 = loggers[0].get_statevector_circuit_logger();
    const auto expected0 = ket::QuantumState {"00"};
    REQUIRE(ket::almost_eq(logger0.statevector(), expected0));

    const auto& logger1 = loggers[1].get_statevector_circuit_logger();
    const auto expected1 = ket::QuantumState {"10"};
    REQUIRE(ket::almost_eq(logger1.statevector(), expected1));

    const auto& logger2 = loggers[2].get_statevector_circuit_logger();
    const auto expected2 = ket::QuantumState {"00"};
    REQUIRE(ket::almost_eq(logger2.statevector(), expected2));

    const auto& logger3 = loggers[3].get_statevector_circuit_logger();
    const auto expected3 = ket::QuantumState {{ {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0}, {0.5, 0.0} }};
    REQUIRE(ket::almost_eq(logger3.statevector(), expected3));
}

#include <cmath>
#include <random>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/complex.hpp"
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
    SECTION("two qubits, CX(source=0, target=1)")
    {
        const auto n_qubits = 2;

        auto circuit = mqis::QuantumCircuit {n_qubits};
        circuit.add_cx_gate(0, 1);

        SECTION("applied to state |00>")
        {
            // expectation: |00> -> |00>
            auto state = mqis::QuantumState {
                {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }

        SECTION("applied to state |01>")
        {
            // expectation: |01> -> |01>
            auto state = mqis::QuantumState {
                {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}}
            };
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }

        SECTION("applied to state |10>")
        {
            // expectation: |10> -> |11>
            auto state = mqis::QuantumState {
                {{0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }

        SECTION("applied to state |11>")
        {
            // expectation: |11> -> |10>
            auto state = mqis::QuantumState {
                {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}
            };
            mqis::simulate(circuit, state);

            const auto expected_state = mqis::QuantumState {
                {{0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}
            };
            REQUIRE(mqis::almost_eq(state, expected_state));
        }
    }
}

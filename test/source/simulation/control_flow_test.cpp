#include <functional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/circuit/circuit.hpp>
#include <kettle/state/state.hpp>
#include <kettle/simulation/simulate.hpp>


TEST_CASE("add_if_statement()")
{
    struct TestCase
    {
        std::function<void(ket::QuantumCircuit&)> circuit_changer;
        ket::QuantumState expected;
    };

    auto testcase = GENERATE(
        TestCase {
            [](ket::QuantumCircuit& circuit) {
                circuit.add_x_gate(0);
            },
            ket::QuantumState {"11"}
        },
        TestCase {
            []([[maybe_unused]] ket::QuantumCircuit& circuit) {},
            ket::QuantumState {"00"}
        }
    );

    auto circuit = ket::QuantumCircuit {2};
    testcase.circuit_changer(circuit);
    circuit.add_m_gate(0);

    // this statement might flip the 1st qubit from '0' to '1'
    circuit.add_if_statement(0, [] {
        auto subcircuit = ket::QuantumCircuit {2};
        subcircuit.add_x_gate(1);
        return subcircuit;
    }());

    auto statevector = ket::QuantumState {"00"};
    ket::simulate(circuit, statevector);

    REQUIRE(ket::almost_eq(statevector, testcase.expected));
}

/*
    We create a statevector in the |00> state, and measure its 0th qubit, which is guaranteed
    to evaluate to 0; we then add an if-not statement that causes the 0th qubit to be flipped
    if the 0th bit register is set to 0.
*/
TEST_CASE("add_if_not_statement()")
{
    auto circuit = ket::QuantumCircuit {2};
    circuit.add_m_gate(0);
    circuit.add_if_not_statement(
        0,
        [] { auto circ = ket::QuantumCircuit {2}; circ.add_x_gate(0); return circ; }()
    );

    auto statevector = ket::QuantumState {"00"};
    ket::simulate(circuit, statevector);

    const auto expected = ket::QuantumState {"10"};
    REQUIRE(ket::almost_eq(statevector, expected));
}

TEST_CASE("add_if_else_statement()")
{
    auto if_circuit = []() {
        auto subcircuit = ket::QuantumCircuit {2};
        subcircuit.add_x_gate(1);
        return subcircuit;
    }();

    auto else_circuit = []() {
        auto subcircuit = ket::QuantumCircuit {2};
        subcircuit.add_x_gate(0);
        return subcircuit;
    }();

    SECTION("if branch runs")
    {
        auto circuit = ket::QuantumCircuit {2};  // state begins with |00>
        circuit.add_x_gate(0);                   // state becomes |10>
        circuit.add_m_gate(0);                   // 0th bit is guaranteed to be 1, state collapses to |10>

        // the if-branch is run, the state gets converted from |10> to |11>
        circuit.add_if_else_statement(0, std::move(if_circuit), std::move(else_circuit));

        auto statevector = ket::QuantumState {"00"};
        ket::simulate(circuit, statevector);

        const auto expected = ket::QuantumState {"11"};

        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("else branch runs")
    {
        auto circuit = ket::QuantumCircuit {2};  // state begins with |00>
        circuit.add_m_gate(0);                   // 0th bit is guaranteed to be 0, state collapses to |00>

        // the else-branch is run, the state gets converted from |00> to |10>
        circuit.add_if_else_statement(0, std::move(if_circuit), std::move(else_circuit));

        auto statevector = ket::QuantumState {"00"};
        ket::simulate(circuit, statevector);

        const auto expected = ket::QuantumState {"10"};

        REQUIRE(ket::almost_eq(statevector, expected));
    }
}

TEST_CASE("add_if_not_else_statement()")
{
    auto if_circuit = []() {
        auto subcircuit = ket::QuantumCircuit {2};
        subcircuit.add_x_gate(1);
        return subcircuit;
    }();

    auto else_circuit = []() {
        auto subcircuit = ket::QuantumCircuit {2};
        subcircuit.add_x_gate(0);
        return subcircuit;
    }();

    SECTION("if branch runs")
    {
        auto circuit = ket::QuantumCircuit {2};  // state begins with |00>
        circuit.add_m_gate(0);                   // 0th bit is guaranteed to be 0, state collapses to |00>

        // the if-branch is run, the state gets converted from |00> to |01>
        circuit.add_if_not_else_statement(0, std::move(if_circuit), std::move(else_circuit));

        auto statevector = ket::QuantumState {"00"};
        ket::simulate(circuit, statevector);

        const auto expected = ket::QuantumState {"01"};

        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("else branch runs")
    {
        auto circuit = ket::QuantumCircuit {2};  // state begins with |00>
        circuit.add_x_gate(0);                   // state becomes |10>
        circuit.add_m_gate(0);                   // 0th bit is guaranteed to be 1, state collapses to |10>

        // the else-branch is run, the state gets converted from |10> to |00>
        circuit.add_if_not_else_statement(0, std::move(if_circuit), std::move(else_circuit));

        auto statevector = ket::QuantumState {"00"};
        ket::simulate(circuit, statevector);

        const auto expected = ket::QuantumState {"00"};

        REQUIRE(ket::almost_eq(statevector, expected));
    }
}

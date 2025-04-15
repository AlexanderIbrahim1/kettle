#include <functional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <mini-qiskit/common/print.hpp>
#include <mini-qiskit/circuit/circuit.hpp>
#include <mini-qiskit/state.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/simulation/measure.hpp>


TEST_CASE("add_if_statement()")
{
    struct TestCase
    {
        std::function<void(mqis::QuantumCircuit&)> circuit_changer;
        mqis::QuantumState expected;
    };

    auto testcase = GENERATE(
        TestCase {
            [](mqis::QuantumCircuit& circuit) {
                circuit.add_x_gate(0);
            },
            mqis::QuantumState {"11"}
        },
        TestCase {
            []([[maybe_unused]] mqis::QuantumCircuit& circuit) {},
            mqis::QuantumState {"00"}
        }
    );

    auto circuit = mqis::QuantumCircuit {2};
    testcase.circuit_changer(circuit);
    circuit.add_m_gate(0);

    // this statement might flip the 1st qubit from '0' to '1'
    circuit.add_if_statement(0, [] {
        auto subcircuit = mqis::QuantumCircuit {2};
        subcircuit.add_x_gate(1);
        return subcircuit;
    }());

    auto statevector = mqis::QuantumState {"00"};
    mqis::simulate(circuit, statevector);

    REQUIRE(mqis::almost_eq_with_print(statevector, testcase.expected));
}

#include <complex>
#include <cstddef>
#include <functional>
#include <random>
#include <vector>

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
    // the 0th qubit is now guarenteed to be in state '1'
    auto circuit = mqis::QuantumCircuit {2};
    circuit.add_x_gate(0);
    circuit.add_m_gate(0);

    // this statement might flip the 1st qubit from '0' to '1'
    auto subcircuit = mqis::QuantumCircuit {2};
    subcircuit.add_x_gate(1);

    circuit.add_if_statement(0, subcircuit);

    auto statevector = mqis::QuantumState {"00"};

    mqis::simulate(circuit, statevector);

    auto expected = mqis::QuantumState {"11"};

    REQUIRE(mqis::almost_eq_with_print(statevector, expected));
}

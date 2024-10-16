#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/common/complex.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

TEST_CASE("simulate x gate")
{
    SECTION("one qubit, X(0)")
    {
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
}

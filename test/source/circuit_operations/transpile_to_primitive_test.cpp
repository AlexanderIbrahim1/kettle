#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/circuit_operations/transpile_to_primitive.hpp"
#include "mini-qiskit/gates/random_u_gates.hpp"
#include "mini-qiskit/simulation/simulate.hpp"
#include "mini-qiskit/state/random.hpp"


constexpr auto n_gates(const mqis::QuantumCircuit& circuit) -> std::ptrdiff_t
{
    return std::distance(circuit.begin(), circuit.end());
}


TEST_CASE("transpile_to_primitive()")
{
    SECTION("1 qubit circuit")
    {
        const auto unitary = mqis::generate_random_unitary2x2();

        auto original = mqis::QuantumCircuit {1};
        original.add_u_gate(unitary, 0);

        const auto transpiled = mqis::transpile_to_primitive(original);

        auto state0 = mqis::generate_random_state(1);
        auto state1 = state0;

        mqis::simulate(original, state0);
        mqis::simulate(transpiled, state1);

        REQUIRE(n_gates(original) == 1);
        REQUIRE(n_gates(transpiled) == 4);
        REQUIRE(mqis::almost_eq(state0, state1));
    }
}

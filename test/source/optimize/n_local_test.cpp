#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/circuit_operations/compare_circuits.hpp>
#include <kettle/gates/compound_gate.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/optimize/n_local.hpp>

using G = ket::Gate;
using CG = ket::CompoundGate;
using Entangle = ket::NLocalEntangelement;

TEST_CASE("n_local construction")
{
    SECTION("2-qubits, rotations=[X], entanglements=[CX]")
    {
        const auto n_repetitions = static_cast<std::size_t>(GENERATE(1, 2, 3, 4));

        const auto [n_local_circuit, parameters] = ket::n_local(2, {G::X}, {G::CX}, Entangle::LINEAR, n_repetitions);

        const auto expected = [n_repetitions]() {
            auto circuit = ket::QuantumCircuit {2};

            for (std::size_t i {0}; i < n_repetitions; ++i) {
                circuit.add_x_gate({0, 1});
                circuit.add_cx_gate(0, 1);
            }
            circuit.add_x_gate({0, 1});

            return circuit;
        }();

        REQUIRE(ket::almost_eq(n_local_circuit, expected));
        REQUIRE(parameters.size() == 0);
    }
}

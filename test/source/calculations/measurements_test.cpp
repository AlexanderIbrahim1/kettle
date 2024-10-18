#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/calculations/measurements.hpp"
#include "mini-qiskit/calculations/probabilities.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulate.hpp"
#include "mini-qiskit/state.hpp"

constexpr static auto FRACTION_TOLERANCE = double {0.02};

TEST_CASE("measurements")
{
    const auto n_shots = std::size_t {1'000'000};

    SECTION("two qubits, measurement after H(1) H(0)")
    {
        // NOTE: for the time being, the presence of the m-gates has nothing to do
        // with the circuit actually being measurable; this is not the case for an
        // actual quantum circuit, but it *is* the case for the limited functionality
        // of the MicroQiskit reference that I'm working with
        auto circuit = mqis::QuantumCircuit {2, 2};
        circuit.add_h_gate(0);
        circuit.add_h_gate(1);
        circuit.add_m_gate(0);
        circuit.add_m_gate(1);

        REQUIRE(mqis::is_circuit_measurable(circuit));

        auto state = mqis::QuantumState {"00"};

        mqis::simulate(circuit, state);

        const auto probabilities = mqis::calculate_probabilities(state);
        const auto measurements = mqis::perform_measurements(probabilities, n_shots);

        REQUIRE(measurements.size() == n_shots);

        const auto counts = mqis::measurements_to_fractions(measurements);

        REQUIRE_THAT(counts.at(0), Catch::Matchers::WithinAbs(0.25, FRACTION_TOLERANCE));
        REQUIRE_THAT(counts.at(1), Catch::Matchers::WithinAbs(0.25, FRACTION_TOLERANCE));
        REQUIRE_THAT(counts.at(2), Catch::Matchers::WithinAbs(0.25, FRACTION_TOLERANCE));
        REQUIRE_THAT(counts.at(3), Catch::Matchers::WithinAbs(0.25, FRACTION_TOLERANCE));
    }

    SECTION("one qubit, measurement after H(0)")
    {
        // NOTE: for the time being, the presence of the m-gates has nothing to do
        // with the circuit actually being measurable; this is not the case for an
        // actual quantum circuit, but it *is* the case for the limited functionality
        // of the MicroQiskit reference that I'm working with
        auto circuit = mqis::QuantumCircuit {1, 1};
        circuit.add_h_gate(0);
        circuit.add_m_gate(0);

        REQUIRE(mqis::is_circuit_measurable(circuit));

        auto state = mqis::QuantumState {"0"};

        mqis::simulate(circuit, state);

        const auto probabilities = mqis::calculate_probabilities(state);
        const auto measurements = mqis::perform_measurements(probabilities, n_shots);

        REQUIRE(measurements.size() == n_shots);

        const auto counts = mqis::measurements_to_fractions(measurements);

        REQUIRE_THAT(counts.at(0), Catch::Matchers::WithinAbs(0.5, FRACTION_TOLERANCE));
        REQUIRE_THAT(counts.at(1), Catch::Matchers::WithinAbs(0.5, FRACTION_TOLERANCE));
    }
}

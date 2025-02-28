#include <algorithm>

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

        const auto probabilities_raw = mqis::calculate_probabilities_raw(state);
        const auto measurements = mqis::perform_measurements_as_memory(probabilities_raw, n_shots);

        REQUIRE(measurements.size() == n_shots);

        const auto counts = mqis::memory_to_fractions(measurements);

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

        const auto probabilities_raw = mqis::calculate_probabilities_raw(state);
        const auto measurements = mqis::perform_measurements_as_memory(probabilities_raw, n_shots);

        REQUIRE(measurements.size() == n_shots);

        const auto counts = mqis::memory_to_fractions(measurements);

        REQUIRE_THAT(counts.at(0), Catch::Matchers::WithinAbs(0.5, FRACTION_TOLERANCE));
        REQUIRE_THAT(counts.at(1), Catch::Matchers::WithinAbs(0.5, FRACTION_TOLERANCE));
    }
}

TEST_CASE("measurement gate")
{
    using BitMask = std::vector<std::uint8_t>;

    const auto calculate_n_measurements = [](const BitMask& bitmask)
    { return std::count_if(bitmask.begin(), bitmask.end(), [](std::uint8_t bit) { return bit == 1; }); };

    auto circuit = mqis::QuantumCircuit {2};

    SECTION("no measurements")
    {
        REQUIRE(calculate_n_measurements(circuit.measure_bitmask()) == 0);
    }

    SECTION("qubit 0 set")
    {
        circuit.add_m_gate(0);
        REQUIRE(circuit.measure_bitmask() == BitMask {1, 0});
        REQUIRE(calculate_n_measurements(circuit.measure_bitmask()) == 1);
    }

    SECTION("qubit 1 set")
    {
        circuit.add_m_gate(1);
        REQUIRE(circuit.measure_bitmask() == BitMask {0, 1});
        REQUIRE(calculate_n_measurements(circuit.measure_bitmask()) == 1);
    }

    SECTION("qubits 0 and 1 set")
    {
        circuit.add_m_gate(0);
        circuit.add_m_gate(1);
        REQUIRE(circuit.measure_bitmask() == BitMask {1, 1});
        REQUIRE(calculate_n_measurements(circuit.measure_bitmask()) == 2);
    }
}

TEST_CASE("throws after multiple measurement gates")
{
    auto circuit = mqis::QuantumCircuit {2};

    SECTION("add m gate to qubit 0 twice")
    {
        circuit.add_m_gate(0);
        REQUIRE_THROWS_AS(circuit.add_m_gate(0), std::runtime_error);
    }

    SECTION("add m gate to qubit 1 twice")
    {
        circuit.add_m_gate(1);
        REQUIRE_THROWS_AS(circuit.add_m_gate(1), std::runtime_error);
    }

    SECTION("add m gate to qubits 0 and 1, and then again to 0")
    {
        circuit.add_m_gate(0);
        circuit.add_m_gate(1);
        REQUIRE_THROWS_AS(circuit.add_m_gate(0), std::runtime_error);
    }
}

TEST_CASE("throws after adding non-measurement gate after measurement gate")
{
    auto circuit = mqis::QuantumCircuit {2};
    circuit.add_h_gate(0);
    circuit.add_m_gate(0);

    REQUIRE_THROWS_AS(circuit.add_h_gate(0), std::runtime_error);
}

TEST_CASE("throws when trying to get marginal counts with no measurement gates")
{
    auto circuit = mqis::QuantumCircuit {2};
    auto state = mqis::QuantumState {"00"};
    circuit.add_h_gate(0);

    REQUIRE_THROWS_AS(mqis::perform_measurements_as_counts_marginal(circuit, state, 1024), std::runtime_error);
}

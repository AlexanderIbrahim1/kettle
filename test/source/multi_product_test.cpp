#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <mini-qiskit/circuit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/decomposed/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit_operations/make_controlled_circuit.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>

#include "test_utils/powers_of_diagonal_unitary.hpp"


auto apply_n_times(std::istream& gate_stream, std::size_t n_powers) -> mqis::QuantumCircuit
{
    const auto gates = mqis::read_decomposed_gate_info(gate_stream);
    const auto n_qubits = mqis::decomposed_circuit_size(gates);

    auto output = mqis::QuantumCircuit {n_qubits};

    for (std::size_t i {0}; i < n_powers; ++i) {
        auto circuit = mqis::make_circuit_from_decomposed_gates(gates);
        mqis::extend_circuit(output, circuit);
    }

    return output;
}

auto apply_n_times_controlled(std::istream& gate_stream, std::size_t n_powers) -> mqis::QuantumCircuit
{
    const auto gates = mqis::read_decomposed_gate_info(gate_stream);
    const auto n_subcircuit_qubits = mqis::decomposed_circuit_size(gates);
    const auto n_qubits = 1 + n_subcircuit_qubits;

    auto mapped_qubits = std::vector<std::size_t> {};
    for (std::size_t i {1}; i < n_qubits; ++i) {
        mapped_qubits.push_back(i);
    }

    auto output = mqis::QuantumCircuit {n_qubits};

    for (std::size_t i {0}; i < n_powers; ++i) {
        const auto circuit = mqis::make_circuit_from_decomposed_gates(gates);
        const auto controlled_circuit = mqis::make_controlled_circuit(circuit, n_qubits, 0, mapped_qubits);
        mqis::extend_circuit(output, controlled_circuit);
    }

    return output;
}

TEST_CASE("powers of unitary operator")
{
    auto gate_pow_1_stream = get_gate_pow_1_stream();
    auto gate_pow_2_stream = get_gate_pow_2_stream();
    auto gate_pow_4_stream = get_gate_pow_4_stream();
    auto gate_pow_8_stream = get_gate_pow_8_stream();
    auto gate_pow_16_stream = get_gate_pow_16_stream();
    auto gate_pow_32_stream = get_gate_pow_32_stream();

    auto init_bitstring = std::string {GENERATE("00", "10", "01", "11")};
    
    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    SECTION("applying gate_pow_1 x2 VS applying gate_pow_2 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_1_stream, 2);
        const auto circuit1 = apply_n_times(gate_pow_2_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_2 x2 VS applying gate_pow_4 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_2_stream, 2);
        const auto circuit1 = apply_n_times(gate_pow_4_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_1 x4 VS applying gate_pow_4 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_1_stream, 4);
        const auto circuit1 = apply_n_times(gate_pow_4_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_1 x8 VS applying gate_pow_8 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_1_stream, 8);
        const auto circuit1 = apply_n_times(gate_pow_8_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_2 x8 VS applying gate_pow_16 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_2_stream, 8);
        const auto circuit1 = apply_n_times(gate_pow_16_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_1 x16 VS applying gate_pow_16 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_1_stream, 16);
        const auto circuit1 = apply_n_times(gate_pow_16_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_1 x32 VS applying gate_pow_32 x1")
    {
        const auto circuit0 = apply_n_times(gate_pow_1_stream, 32);
        const auto circuit1 = apply_n_times(gate_pow_32_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }
}

TEST_CASE("powers of unitary operator controlled")
{
    auto gate_pow_1_stream = get_gate_pow_1_stream();
    auto gate_pow_2_stream = get_gate_pow_2_stream();
    auto gate_pow_8_stream = get_gate_pow_8_stream();

    auto init_bitstring = std::string {GENERATE("000", "100", "010", "110", "001", "101", "011", "111")};
    
    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    SECTION("applying gate_pow_1 x2 VS applying gate_pow_2 x1")
    {
        const auto circuit0 = apply_n_times_controlled(gate_pow_1_stream, 2);
        const auto circuit1 = apply_n_times_controlled(gate_pow_2_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }

    SECTION("applying gate_pow_1 x8 VS applying gate_pow_8 x1")
    {
        const auto circuit0 = apply_n_times_controlled(gate_pow_1_stream, 8);
        const auto circuit1 = apply_n_times_controlled(gate_pow_8_stream, 1);

        mqis::simulate(circuit0, state0);
        mqis::simulate(circuit1, state1);

        REQUIRE(mqis::almost_eq(state0, state1));
    }
}

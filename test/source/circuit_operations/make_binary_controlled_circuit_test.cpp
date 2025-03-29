#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/simulation/simulate.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/circuit_operations/build_decomposed_circuit.hpp"
#include "mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp"
#include "mini-qiskit/decomposed/read_decomposition_file.hpp"

#include "../test_utils/powers_of_diagonal_unitary.hpp"

TEST_CASE("make_binary_controlled_circuit()")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    const auto angle = M_PI_4;
    const auto t_gate = mqis::p_gate(angle);

    // create the circuit manually, by repeating the gates
    auto manual_circuit = mqis::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{0, 3, angle}});
    manual_circuit.add_cp_gate({{1, 3, angle}, {1, 3, angle}});
    manual_circuit.add_cp_gate({{2, 3, angle}, {2, 3, angle}, {2, 3, angle}, {2, 3, angle}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuit = mqis::QuantumCircuit {1};
    subcircuit.add_u_gate(t_gate, 0);
    auto binary_made_circuit = mqis::make_binary_controlled_circuit_naive(subcircuit, 4, {0, 1, 2}, {3});

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(manual_circuit, state0);
    mqis::simulate(binary_made_circuit, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

TEST_CASE("make_binary_controlled_circuit_from_binary_powers() for single qubit gate")
{
    const auto init_bitstring = std::string {
        GENERATE(
            "0000", "1000", "0100", "1100", "0010", "1010", "0110", "1110",
            "0001", "1001", "0101", "1101", "0011", "1011", "0111", "1111"
        )
    };

    const auto angle = 1.2345;

    // create the circuit manually, by repeating the gates
    auto manual_circuit = mqis::QuantumCircuit {4};
    manual_circuit.add_cp_gate({{0, 3, angle}});
    manual_circuit.add_cp_gate({{1, 3, angle}, {1, 3, angle}});
    manual_circuit.add_cp_gate({{2, 3, angle}, {2, 3, angle}, {2, 3, angle}, {2, 3, angle}});

    // create the circuit using the `make_binary_controlled_circuit()` function
    auto subcircuits = std::vector<mqis::QuantumCircuit> {};
    for (std::size_t i {0}; i < 3; ++i) {
        const auto power_angle = static_cast<double>(1ul << i) * angle;

        auto subcircuit = mqis::QuantumCircuit {1};
        subcircuit.add_u_gate(mqis::p_gate(power_angle), 0);
        subcircuits.emplace_back(std::move(subcircuit));
    }

    auto binary_made_circuit = mqis::make_binary_controlled_circuit_from_binary_powers(subcircuits, 4, {0, 1, 2}, {3});

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(manual_circuit, state0);
    mqis::simulate(binary_made_circuit, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

TEST_CASE("make_binary_controlled_circuit_from_binary_powers() for double qubit gate")
{
    auto all_streams = std::vector<std::stringstream> {};
    all_streams.emplace_back(get_gate_pow_1_stream());
    all_streams.emplace_back(get_gate_pow_2_stream());
    all_streams.emplace_back(get_gate_pow_4_stream());
    all_streams.emplace_back(get_gate_pow_8_stream());
    all_streams.emplace_back(get_gate_pow_16_stream());
    all_streams.emplace_back(get_gate_pow_32_stream());

    const auto circuits_pow_maker = [](std::vector<std::stringstream>& streams, std::size_t n_control_qubits) {
        auto output = std::vector<mqis::QuantumCircuit> {};

        for (std::size_t i {0}; i < n_control_qubits; ++i) {
            auto& stream = streams[i];
            const auto gates = mqis::read_decomposed_gate_info(stream);
            output.emplace_back(mqis::make_circuit_from_decomposed_gates(gates));
        }

        return output;
    };

    const auto control_and_mapped_qubits = [](std::size_t n_total_qubits) -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>> {
        if (n_total_qubits < 3) {
            throw std::runtime_error {"Can't perform this test with fewer than 3 qubits."};
        }

        auto output_control_qubits = std::vector<std::size_t> {};
        for (std::size_t i {0}; i < n_total_qubits - 2ul; ++i) {
            output_control_qubits.push_back(i);
        }

        auto output_mapped_qubits = std::vector<std::size_t> {n_total_qubits - 2ul, n_total_qubits - 1ul};

        return {output_control_qubits, output_mapped_qubits};
    };

    const auto n_control_qubits = GENERATE(Catch::Generators::range(1ul, 6ul));
    const auto n_total_qubits = n_control_qubits + 2ul;
    const auto i_state = GENERATE_COPY(Catch::Generators::range(0ul, 1ul << n_total_qubits));
    const auto init_bitstring = mqis::state_as_bitstring_little_endian(i_state, n_total_qubits);

    const auto [control_qubits, mapped_qubits] = control_and_mapped_qubits(n_total_qubits);

    const auto circuits_pow = circuits_pow_maker(all_streams, n_control_qubits);
    const auto binary_made_circuit_naive = mqis::make_binary_controlled_circuit_naive(circuits_pow[0], n_total_qubits, control_qubits, mapped_qubits);
    const auto binary_made_circuit_power = mqis::make_binary_controlled_circuit_from_binary_powers(circuits_pow, n_total_qubits, control_qubits, mapped_qubits);

    auto state0 = mqis::QuantumState {init_bitstring};
    auto state1 = mqis::QuantumState {init_bitstring};

    mqis::simulate(binary_made_circuit_naive, state0);
    mqis::simulate(binary_made_circuit_power, state1);

    REQUIRE(mqis::almost_eq(state0, state1));
}

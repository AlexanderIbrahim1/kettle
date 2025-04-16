#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/state/qubit_state_conversion.hpp>
#include <mini-qiskit/gates/multiplicity_controlled_u_gate.hpp>


enum class QueryCase
{
    CONSTANT_0 = 0,
    CONSTANT_1 = 1,
    BALANCED = 2,
};


auto sampled_indices_of_half_of_all_states(std::size_t n_data_qubits) -> std::vector<std::size_t>
{
    auto prng = []() {
        auto device = std::random_device {};
        const auto seed = device();
        return std::mt19937 {seed};
    }();

    auto all_states = [&]() {
        auto states = std::vector<std::size_t> (1ul << n_data_qubits);
        std::iota(states.begin(), states.end(), 0);
        return states;
    }();

    auto sampled_states = [&]() {
        const auto n_samp_states = all_states.size() / 2;

        auto samp_states = std::vector<std::size_t> {};
        samp_states.reserve(n_samp_states);

        std::sample(all_states.begin(), all_states.end(), std::back_inserter(samp_states), n_samp_states, prng);

        return samp_states;
    }();

    return sampled_states;
}

void add_x_gates_on_set_bits(mqis::QuantumCircuit& circuit, const std::vector<std::uint8_t>& bitset) {
    for (std::size_t i_qubit {0}; i_qubit < bitset.size(); ++i_qubit) {
        if (bitset[i_qubit]) {
            circuit.add_x_gate(i_qubit);
        }
    }
};

void add_deutsch_jozsa_function(mqis::QuantumCircuit& circuit, QueryCase query)
{
    const auto i_ancilla = circuit.n_qubits() - 1;

    if (query == QueryCase::CONSTANT_0) {
        return;
    }

    if (query == QueryCase::CONSTANT_1) {
        circuit.add_x_gate(i_ancilla);
        return;
    }

    const auto data_qubits = [&]() {
        auto qubits = std::vector<std::size_t> (circuit.n_qubits() - 1);
        std::iota(qubits.begin(), qubits.end(), 0);
        return qubits;
    }();

    const auto sampled_states = sampled_indices_of_half_of_all_states(data_qubits.size());

    for (auto i_state : sampled_states) {
        const auto bitset = mqis::state_index_to_dynamic_bitset(i_state, data_qubits.size(), mqis::QuantumStateEndian::LITTLE);

        add_x_gates_on_set_bits(circuit, bitset);
        mqis::apply_multiplicity_controlled_u_gate(circuit, mqis::x_gate(), i_ancilla, data_qubits);
        add_x_gates_on_set_bits(circuit, bitset);
    }
}

auto main() -> int
{
    const auto query = QueryCase::BALANCED;

    auto statevector = mqis::QuantumState {"00001"};

    auto circuit = mqis::QuantumCircuit {5};
    circuit.add_h_gate({0, 1, 2, 3, 4});
    add_deutsch_jozsa_function(circuit, query);
    circuit.add_h_gate({0, 1, 2, 3});

    mqis::simulate(circuit, statevector);

    const auto counts = mqis::perform_measurements_as_counts_marginal(statevector, 10000, {4});

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }
    // using `QueryCase::CONSTANT_0`
    // ```
    // (state, count) = (0000x, 10000)
    // ```
    //
    // using `QueryCase::CONSTANT_1`
    // ```
    // (state, count) = (0000x, 10000)
    // ```
    //
    // using `QueryCase::BALANCED`
    // ```
    // (state, count) = (0011x, 590)
    // (state, count) = (0010x, 658)
    // (state, count) = (1111x, 627)
    // (state, count) = (1011x, 653)
    // (state, count) = (0001x, 2572)
    // (state, count) = (0110x, 613)
    // (state, count) = (1100x, 2472)
    // (state, count) = (0111x, 589)
    // (state, count) = (1010x, 612)
    // (state, count) = (1110x, 614)
    // ```

    return 0;
}

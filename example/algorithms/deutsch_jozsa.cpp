#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include <kettle/kettle.hpp>

/*
    This file contains a demonstration of how to use the library to simulate
    the Deutsch-Jozsa algorithm. To pick which case the query function satisfies,
    select the appropriate QueryCase instance in the first non-comment line
    of the main() function below.
*/


enum class QueryCase
{
    CONSTANT_0 = 0,  // all inputs map to 0
    CONSTANT_1 = 1,  // all inputs map to 1
    BALANCED = 2,    // half of all inputs map to 0, the other half map to 1
};


/*
    Randomly select which of the 2^(n_data_qubits - 1) will be mapped to 1
*/
auto sampled_indices_of_half_of_all_states(std::size_t n_data_qubits) -> std::vector<std::size_t>
{
    auto prng = std::mt19937 {std::random_device {}()};
    const auto state_indices = ket::arange(1UL << n_data_qubits);

    const auto n_samples = state_indices.size() / 2;
    auto sampled_indices = std::vector<std::size_t> {};
    sampled_indices.reserve(n_samples);
    std::sample(state_indices.begin(), state_indices.end(), std::back_inserter(sampled_indices), n_samples, prng);

    return sampled_indices;
}

/*
    Add the Deutsch-Jozsa query to the circuit; which query is applied, is determined
    by the choice of `query`
*/
void apply_deutsch_jozsa_function(ket::QuantumCircuit& circuit, QueryCase query)
{
    const auto i_ancilla = circuit.n_qubits() - 1;

    // the case where the DJ function maps all inputs to 0; because the XOR of a bit with 0
    // returns that bit, this is equivalent to not doing anything
    if (query == QueryCase::CONSTANT_0) {
        return;
    }

    // the case where the DJ function maps all inputs to 1; because the XOR of a bit with 1
    // flips that bit, this is equivalent to adding an X gate at the ancilla
    if (query == QueryCase::CONSTANT_1) {
        circuit.add_x_gate(i_ancilla);
        return;
    }

    // helper function need to map specific inputs in the query function to 1
    const auto add_x_gates_on_set_bits = [&](const std::vector<std::uint8_t>& bitset_) {
        for (std::size_t i_qubit {0}; i_qubit < bitset_.size(); ++i_qubit) {
            if (bitset_[i_qubit]) {
                circuit.add_x_gate(i_qubit);
            }
        }
    };

    const auto data_qubits = ket::arange(circuit.n_qubits() - 1);
    const auto sampled_states = sampled_indices_of_half_of_all_states(data_qubits.size());

    for (auto i_state : sampled_states) {
        const auto bitset = ket::state_index_to_dynamic_bitset(i_state, data_qubits.size(), ket::Endian::LITTLE);

        add_x_gates_on_set_bits(bitset);
        ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), i_ancilla, data_qubits);
        add_x_gates_on_set_bits(bitset);
    }
}

auto main() -> int
{
    // choose the query function; this is done by selecting one of the enum variants
    const auto query = QueryCase::BALANCED;

    // construct the initial state; we have 4 "data qubits" and 1 "ancilla qubit" (the last)
    auto statevector = ket::Statevector {"00001"};

    // create the circuit with the gates needed for the Deutsch-Jozsa algorithm
    auto circuit = ket::QuantumCircuit {5};
    circuit.add_h_gate({0, 1, 2, 3, 4});
    apply_deutsch_jozsa_function(circuit, query);
    circuit.add_h_gate({0, 1, 2, 3});

    // propagate the state through the circuit
    ket::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts; the ancilla qubit (at index `4`) is being marginalized
    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 10000, {4});

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

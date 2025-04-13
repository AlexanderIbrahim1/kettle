#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <mini-qiskit/mini-qiskit.hpp>

/*
Uses the simon oracle given in the qiskit textbook:
    https://github.com/qiskit-community/qiskit-textbook/blob/master/qiskit-textbook-src/qiskit_textbook/tools/__init__.py
*/


void apply_simon_function(mqis::QuantumCircuit& circuit, const std::vector<std::uint8_t>& hidden_bitset)
{
    // the oracle for Simon'd problem cannot use the zero bitstring; it invalidates the Simon property
    const auto it = std::find(hidden_bitset.begin(), hidden_bitset.end(), std::uint8_t {1});
    if (it == hidden_bitset.end()) {
        throw std::runtime_error {"The hidden bitstring cannot be the zero bitstring"};
    }

    const auto i_first_set = static_cast<std::size_t>(std::distance(hidden_bitset.begin(), it));

    const auto n_data_qubits = hidden_bitset.size();

    if (circuit.n_qubits() != 2 * n_data_qubits) {
        throw std::runtime_error {"The bitstring has an invalid number of qubits for this circuit"};
    }

    // step 1: copy |x>|0> -> |x>|x>
    for (std::size_t i {0}; i < n_data_qubits; ++i) {
        circuit.add_cx_gate(i, n_data_qubits + i);
    }

    // step 2: map |x>|x> -> |x>|x XOR b>
    for (std::size_t i {0}; i < n_data_qubits; ++i) {
        if (hidden_bitset[i]) {
            circuit.add_cx_gate(i_first_set, n_data_qubits + i);
        }
    }
}

void check_bitstring(const std::string& bitstring, const std::vector<std::uint8_t>& hidden_bitset) {
    // check if the measured portion of the bitstring is "orthogonal" (mod 2) to the hidden bitset
    const auto bitset = [&]() {
        const auto n_bits = hidden_bitset.size();

        auto output = std::vector<std::uint8_t> {};
        output.reserve(n_bits);

        for (const auto bitchar : bitstring) {
            if (bitchar == '1') {
                output.push_back(1);
            }
            else if (bitchar == '0') {
                output.push_back(0);
            }
        }

        return output;
    }();

    const auto product = std::inner_product(bitset.begin(), bitset.end(), hidden_bitset.begin(), std::uint8_t {0});
    if (product % 2 == 0) {
        std::cout << "VALID:   " << bitstring << " is 'orthogonal' to the hidden bitstring!\n";
    } else {
        std::cout << "INVALID: " << bitstring << " is NOT 'orthogonal' to the hidden bitstring!\n";
    }
}

auto main() -> int
{
    const auto hidden_bitset = std::vector<std::uint8_t> {1, 0, 1};

    auto state = mqis::QuantumState {"000000"};

    auto circuit = mqis::QuantumCircuit {6};
    circuit.add_h_gate({0, 1, 2});
    apply_simon_function(circuit, hidden_bitset);
    circuit.add_h_gate({0, 1, 2});

    mqis::simulate(circuit, state);

    const auto counts = mqis::perform_measurements_as_counts_marginal(state, 1024, {3, 4, 5});

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    for (const auto& kv_pair : counts) {
        const auto& bitstring = kv_pair.first;
        check_bitstring(bitstring, hidden_bitset);
    }
    // Example output:
    // ```
    // (state, count) = (000xxx, 247)
    // (state, count) = (111xxx, 268)
    // (state, count) = (010xxx, 246)
    // (state, count) = (101xxx, 263)
    // VALID:   000xxx is 'orthogonal' to the hidden bitstring!
    // VALID:   111xxx is 'orthogonal' to the hidden bitstring!
    // VALID:   010xxx is 'orthogonal' to the hidden bitstring!
    // VALID:   101xxx is 'orthogonal' to the hidden bitstring!
    // ```

    return 0;
}

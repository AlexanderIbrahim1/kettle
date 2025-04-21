#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <mini-qiskit/mini-qiskit.hpp>

/*
    Uses the Simon oracle given in the qiskit textbook:
        https://github.com/qiskit-community/qiskit-textbook/blob/master/qiskit-textbook-src/qiskit_textbook/tools/__init__.py

    In Simon's problem, we have a function `f` that maps `{0, 1}^n -> {0, 1}^m`, with `m >= n`
    The function `f` has the following properties:
      1. it is a "two-to-one" function:
        - every possible output in `{0, 1}^m` is mapped to by exactly `0` or `2` possible inputs in `{0, 1}^n`
      2. two inputs `a` and `b` map to the same value IFF `a != b`
        - i.e. f(a) == f(b)  <->  a != b
      3. any two inputs that satisfy this property are called a matching pair
        - and there are 2^(n - 1) matching pairs
      4. every matching pair `(a, b)` is related by the property `a = b XOR s`
        - where `s` is some hidden non-zero bitstring
        - and it is the same `s` for all matching pairs

    The goal of Simon's algorithm is to find this hidden bitstring `s`
*/

/*
    Apply the Simon's oracle as implemented in the link above.
*/
void apply_simon_function(mqis::QuantumCircuit& circuit, const std::vector<std::uint8_t>& hidden_bitset)
{
    // make sure we aren't using the zero bitstring; it invalidates the Simon property
    const auto it = std::find(hidden_bitset.begin(), hidden_bitset.end(), std::uint8_t {1});
    if (it == hidden_bitset.end()) {
        throw std::runtime_error {"The hidden bitstring cannot be the zero bitstring"};
    }

    // make sure the number of bits in the hidden bitstring is correct
    const auto n_data_qubits = hidden_bitset.size();
    if (circuit.n_qubits() != 2 * n_data_qubits) {
        throw std::runtime_error {"The bitstring has an invalid number of qubits for this circuit"};
    }

    // step 1: copy |x>|0> -> |x>|x>
    for (std::size_t i {0}; i < n_data_qubits; ++i) {
        circuit.add_cx_gate(i, n_data_qubits + i);
    }

    // step 2: map |x>|x> -> |x>|x XOR b>
    const auto i_first_set = static_cast<std::size_t>(std::distance(hidden_bitset.begin(), it));
    for (std::size_t i {0}; i < n_data_qubits; ++i) {
        if (hidden_bitset[i]) {
            circuit.add_cx_gate(i_first_set, n_data_qubits + i);
        }
    }
}

/*
    Check if the measured portion of the bitstring is "orthogonal" (mod 2) to the hidden bitset
*/
void check_bitstring(const std::string& bitstring, const std::string& hidden_bitstring) {
    const auto bitset = mqis::bitstring_to_dynamic_bitset(bitstring);
    const auto hidden_bitset = mqis::bitstring_to_dynamic_bitset(hidden_bitstring);
    const auto product = std::inner_product(bitset.begin(), bitset.end(), hidden_bitset.begin(), std::uint8_t {0});

    if (product % 2 == 0) {
        std::cout << "VALID:   " << bitstring;
        std::cout << " is 'orthogonal' to the hidden bitstring " << hidden_bitstring << '\n';
    } else {
        std::cout << "INVALID: " << bitstring;
        std::cout << " is NOT 'orthogonal' to the hidden bitstring" << hidden_bitstring << '\n';
    }
}

auto main() -> int
{
    // choose the hidden bitstring
    const auto hidden_bitstring = "101";
    const auto hidden_bitset = mqis::bitstring_to_dynamic_bitset(hidden_bitstring);

    // create the circuit needed for Simon's algorithm
    auto circuit = mqis::QuantumCircuit {6};
    circuit.add_h_gate({0, 1, 2});
    apply_simon_function(circuit, hidden_bitset);
    circuit.add_h_gate({0, 1, 2});

    // create the statevector, and propagate it through the circuit
    auto state = mqis::QuantumState {"000000"};
    mqis::simulate(circuit, state);

    // get a map of the bitstrings to the counts; in Simon's algorithm, we are concerned
    // with the leftmost half of the qubits, which given the bitstrings that are "orthogonal"
    // to the hidden bitstring; so we marginalize out the right half of the qubits (3, 4, 5)
    const auto counts = mqis::perform_measurements_as_counts_marginal(state, 1024, {3, 4, 5});

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    // check if the output is correct;
    // i.e. check if the resulting bitstrings are orthogonal to the hidden bistring
    for (const auto& kv_pair : counts) {
        const auto bitstring = mqis::rstrip_marginal_bits(kv_pair.first);
        check_bitstring(bitstring, hidden_bitstring);
    }
    // Example output:
    // ```
    // (state, count) = (101xxx, 245)
    // (state, count) = (000xxx, 239)
    // (state, count) = (111xxx, 276)
    // (state, count) = (010xxx, 264)
    // VALID:   101 is 'orthogonal' to the hidden bitstring 101
    // VALID:   000 is 'orthogonal' to the hidden bitstring 101
    // VALID:   111 is 'orthogonal' to the hidden bitstring 101
    // VALID:   010 is 'orthogonal' to the hidden bitstring 101
    // ```

    return 0;
}

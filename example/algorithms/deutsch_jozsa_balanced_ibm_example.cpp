#include <iostream>

#include <kettle/kettle.hpp>

/*
    This file contains a demonstration of how to use the library to simulate
    the Deutsch-Jozsa algorithm.

    This example uses the balanced f-query given in the guide in:
        https://learning.quantum.ibm.com/course/fundamentals-of-quantum-algorithms/quantum-query-algorithms

    Specifically, the one with 3 data qubits and 1 ancilla qubit
*/

/*
    This function applies the gates for the Deutsch-Jozsa query function provided in
    the aforementioned IBM page's example.
*/
void apply_balanced_deutsch_jozsa_from_ibm_example(ket::QuantumCircuit& circuit)
{
    // |001> maps to 1
    circuit.add_x_gate({2});
    ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({2});

    // |111> maps to 1
    circuit.add_x_gate({0, 1, 2});
    ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({0, 1, 2});

    // |101> maps to 1
    circuit.add_x_gate({0, 2});
    ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({0, 2});

    // |010> maps to 1
    circuit.add_x_gate({1});
    ket::apply_multiplicity_controlled_u_gate(circuit, ket::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({1});
}

auto main() -> int
{
    // construct the initial state; we have 3 "data qubits" and 1 "ancilla qubit" (the last)
    auto statevector = ket::QuantumState {"0001"};

    // create the circuit with the gates needed for the Deutsch-Jozsa algorithm
    auto circuit = ket::QuantumCircuit {4};
    circuit.add_h_gate({0, 1, 2, 3});
    apply_balanced_deutsch_jozsa_from_ibm_example(circuit);
    circuit.add_h_gate({0, 1, 2});

    // propagate the state through the circuit
    ket::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts; the ancilla qubit (at index `3`) is being marginalized
    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1000, {3});

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }
    // Example output:
    // ```
    // (state, count) = (001x, 256)
    // (state, count) = (111x, 240)
    // (state, count) = (011x, 259)
    // (state, count) = (101x, 245)
    // ```

    return 0;
}

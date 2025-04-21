#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>

/*
    This file contains an example of Grover's algorithm with n = 2 qubits.
    This code is based on the example found in:
        https://github.com/Qiskit/textbook/blob/main/notebooks/ch-algorithms/grover.ipynb

    The target state we are interested in finding is `|11>`.
*/


auto main() -> int
{
    // construct the circuit with the gates needed for the Grover algorithm
    auto circuit = mqis::QuantumCircuit {2};

    // initialize to uniform superposition of all states
    circuit.add_h_gate({0, 1});

    // add the oracle
    // the target state is `|11>`, which we want to mark as negative; we can do this
    // with a single CZ gate
    circuit.add_cz_gate(0, 1);

    // add the Grover diffuser operator
    // - this leaves |00> positive, but makes all other computational basis states negative
    // - the two Z gates followed by the CZ gate accomplish this
    //
    // NOTICE: only a single rotation is required for this example
    circuit.add_h_gate({0, 1});
    circuit.add_z_gate({0, 1});
    circuit.add_cz_gate(0, 1);
    circuit.add_h_gate({0, 1});

    // create the initial statevector, and propagate it through the circuit
    auto statevector = mqis::QuantumState {"00"};
    mqis::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts
    const auto counts = mqis::perform_measurements_as_counts(statevector, 1024);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(bitstring, count) = (" << bitstring << ", " << count << ")\n";
    }
    // Expected output:
    // ```
    // (bitstring, count) = (11, 1024)
    // ```

    return 0;
}

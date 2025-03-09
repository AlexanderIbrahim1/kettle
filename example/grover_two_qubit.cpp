#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>

/*
This is an example of Grover's algorithm with n = 2 qubits.
The target state we are interested in finding is `|11>`

Useful resource: https://github.com/Qiskit/textbook/blob/main/notebooks/ch-algorithms/grover.ipynb
*/


auto main() -> int
{
    auto circuit = mqis::QuantumCircuit {2};

    // initialize to uniform superposition of all states
    circuit.add_h_gate({0, 1});

    // add the oracle, which marks the `|11>` state as negative
    circuit.add_cz_gate(0, 1);

    // add the Grover diffuser operator
    // - this leaves |00> positive, but leaves all other computational basis states negative
    // - the two Z-gates followed by the CZ-gate accomplish this
    //
    // NOTICE: only a single rotation is required!
    circuit.add_h_gate({0, 1});
    circuit.add_z_gate({0, 1});
    circuit.add_cz_gate(0, 1);
    circuit.add_h_gate({0, 1});

    // add the measurements
    circuit.add_m_gate({0, 1});

    auto state = mqis::QuantumState {"00"};
    mqis::simulate(circuit, state);

    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, state, 1024);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(bitstring, count) = (" << bitstring << ", " << count << ")\n";
    }
    // Expected output:
    // ```
    // (bitstring, count) = (11, 1024)
    // ```

    return 0;
}

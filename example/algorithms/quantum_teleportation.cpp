#include <complex>
#include <iostream>
#include <random>

#include <mini-qiskit/mini-qiskit.hpp>

/*
    This file contains a demonstration of how to use the library to simulate the
    prototypical example of quantum teleportation of a one-qubit state from Alice to Bob.
*/

auto main() -> int
{
    // Alice initally holds the state
    const auto alice_qubit = mqis::generate_random_state(1);

    // now we create the two resource qubits
    const auto resource_qubits = mqis::QuantumState {"00"};

    // the input to the circuit is the tensor product of these states
    auto input = mqis::tensor_product(alice_qubit, resource_qubits);

    // Now we create the circuit
    auto circuit = mqis::QuantumCircuit {3};

    // we begin with the gates that turn the resource qubits into the Bell state
    circuit.add_h_gate(1);
    circuit.add_cx_gate(1, 2);

    // and then the gates that entangle the Bell state with Alice's qubit
    circuit.add_cx_gate(0, 1);
    circuit.add_h_gate(0);

    // now perform the measurements, and modify the behaviour of qubit 2 based on these measurements
    circuit.add_m_gate({0, 1});

    // if qubit 1 is measured as set, apply the X gate to qubit 2
    circuit.add_if_statement(1, [] {
        auto subcircuit = mqis::QuantumCircuit {3};
        subcircuit.add_x_gate(2);
        return subcircuit;
    }());

    // if qubit 0 is measured as set, apply the Z gate to qubit 2
    circuit.add_if_statement(0, [] {
        auto subcircuit_ = mqis::QuantumCircuit {3};
        subcircuit_.add_z_gate(2);
        return subcircuit_;
    }());

    // what is the probability distribution of the original qubit that Alice had?
    const auto alice_counts = mqis::perform_measurements_as_counts_marginal(alice_qubit, 1 << 12);

    // what is the probability distribution of the qubit that Bob receives?
    // - because the measurement causes the final statevector to be different with each simulation,
    //   to collect the counts, we simulate the statevector and sample a single time for each shot
    const auto bob_counts = mqis::perform_measurements_as_counts_marginal(circuit, input, 1 << 12, {0, 1});

    std::cout << "ALICE: (state, count) = (0, " << alice_counts.at("0") << ")\n";
    std::cout << "ALICE: (state, count) = (1, " << alice_counts.at("1") << ")\n";
    std::cout << "BOB  : (state, count) = (0, " << bob_counts.at("xx0") << ")\n";
    std::cout << "BOB  : (state, count) = (1, " << bob_counts.at("xx1") << ")\n";
    // Expected output:
    // ```
    // ALICE: (state, count) = (0, 1750)
    // ALICE: (state, count) = (1, 2346)
    // BOB  : (state, count) = (0, 1713)
    // BOB  : (state, count) = (1, 2383)
    // ```
    // The counts for Alice's original state, and Bob's delivered state, should be similar

    return 0;
}

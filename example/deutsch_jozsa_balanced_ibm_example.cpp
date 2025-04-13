#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/gates/multiplicity_controlled_u_gate.hpp>

/*
Uses the balanced f-query given in the guide in:
    https://learning.quantum.ibm.com/course/fundamentals-of-quantum-algorithms/quantum-query-algorithms

Specifically, the one with 3 data qubits and 1 ancilla qubit
*/

void add_balanced_deutsch_jozsa_from_ibm_example(mqis::QuantumCircuit& circuit)
{
    // this is a circuit with 3 data qubits and 1 ancilla qubit
    circuit.add_x_gate({2});
    mqis::apply_multiplicity_controlled_u_gate(circuit, mqis::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({2});

    circuit.add_x_gate({0, 1, 2});
    mqis::apply_multiplicity_controlled_u_gate(circuit, mqis::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({0, 1, 2});

    circuit.add_x_gate({0, 2});
    mqis::apply_multiplicity_controlled_u_gate(circuit, mqis::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({0, 2});

    circuit.add_x_gate({1});
    mqis::apply_multiplicity_controlled_u_gate(circuit, mqis::x_gate(), 3, {0, 1, 2});
    circuit.add_x_gate({1});
}

auto main() -> int
{
    auto statevector = mqis::QuantumState {"0001"};

    auto circuit = mqis::QuantumCircuit {4};
    circuit.add_h_gate({0, 1, 2, 3});
    add_balanced_deutsch_jozsa_from_ibm_example(circuit);
    circuit.add_h_gate({0, 1, 2});

    mqis::simulate(circuit, statevector);

    const auto counts = mqis::perform_measurements_as_counts_marginal(statevector, 1000, {3});

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

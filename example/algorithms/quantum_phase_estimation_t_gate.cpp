#include <algorithm>
#include <cmath>
#include <iostream>

#include <kettle/kettle.hpp>

/*
    This file contains a demonstration of how to use the library to perform
    Quantum Phase Estimation (QPE).

    For this explanation:
      - the qubits used to represent the binary expansion of the exponent in the
        eigenvalue, are referred to as the "register qubits".
      - the qubits used to represent the unitary operator whose eigenvalue is being
        calculated, are referred to as the "unitary qubits"

    We choose T gate, represented by the unitary matrix:
        [ 1    0                 ]
        [ 0    exp(2 pi i (1/8)) ]
    
    This means the |1> state has an eigenvalue of exp(2 pi i (1/8)), and using QPE,
    we can express the eigenvalue exactly using only 3 register qubits.
*/

/*
    This function applies the unitary operator of interest (the T gate) to the circuit
    in the manner required for QPE.

    More specifically, it applies the T gate in a multiplicity-binary controlled manner,
    where the gate is applied `2^n` times for the `n`th register qubit.
*/
void apply_multiplicity_controlled_t_gate_manually(ket::QuantumCircuit& circuit)
{
    const auto angle = M_PI_4;

    // apply the T gate:
    // - 1 time  for the 0th register qubit
    // - 2 times for the 1st register qubit
    // - 3 times for the 2nd register qubit
    circuit.add_cp_gate({{0, 3, angle}});
    circuit.add_cp_gate({{1, 3, angle}, {1, 3, angle}});
    circuit.add_cp_gate({{2, 3, angle}, {2, 3, angle}, {2, 3, angle}, {2, 3, angle}});
}


auto main() -> int
{
    // specify the number of register and unitary qubits for the problem
    const auto n_register_qubits = 3;
    const auto n_unitary_qubits = 1;

    // create the circuit with the gates needed to perform QPE
    auto circuit = ket::QuantumCircuit {n_register_qubits + n_unitary_qubits};
    circuit.add_h_gate({0, 1, 2});
    circuit.add_x_gate(3);
    apply_multiplicity_controlled_t_gate_manually(circuit);
    circuit.add_iqft_gate({2, 1, 0});

    // construct the statevector in the 0-state, and propagate it through the circuit
    auto statevector = ket::Statevector {"0000"};
    ket::simulate(circuit, statevector);

    // get a map of the bitstrings to the counts; in QPE, we are concerned with the output
    // of the register qubits, and thus we marginalize the unitary qubit (`3`) here
    const auto counts = ket::perform_measurements_as_counts_marginal(statevector, 1024, {3});

    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";

        auto rstripped_bitstring = ket::rstrip_marginal_bits(bitstring);

        // the manner in which we apply the controlled unitary gates for QPE affects the output;
        // - in this example, the 0th qubit was applied once, the 1st qubit was applied twice, etc.
        // - this means the largest contributor is on the right of the bitstring
        //   - and we need to reverse the bitstring being calculating the binary fraction expansion
        std::ranges::reverse(rstripped_bitstring);
        const auto binary_fraction = ket::binary_fraction_expansion(rstripped_bitstring);

        const auto estimated_phase = 2.0 * M_PI * binary_fraction;

        std::cout << "estimated phase: " << estimated_phase << '\n';
    }
    // Expected output:
    // ```
    // (state, count) = (100x, 1024)
    // estimated phase: 0.785398
    // ```

    return 0;
}

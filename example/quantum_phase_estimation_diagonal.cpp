#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/decomposed/decomposed_gate.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/decomposed/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state.hpp>

/*
    We first create a 4x4 diagonal unitary matrix, where the diagonal elements are
        [ exp(2 PI i B0), exp(2 PI i B1), exp(2 PI i B2), exp(2 PI i B3) ]

    Where {B0, B1, B2, B3} are each a floating point number in [0, 1), given by a binary expansion.

    The binary expansions are (following little endian order):
      - 010110 (for input "00")
      - 100101 (for input "10")
      - 101010 (for input "01")
      - 011101 (for input "11")
    
    The phases for each of these binary expansions (calculated directly from the unitary matrix):
        {2.1598449493429825, 3.6324665057131984, 4.123340357836604, 2.84706834231575}
    
    We then decompose this 4x4 unitary matrix into a quantum cicuit of 1-qubit and 2-qubit gates.

    The code below performs quantum phase estimation to recover the phases.

    ----------------------------------------------------------------------------

    In the code below, change the input to the `unitary_eigenstatvector` to any of:
      {"00", "10", "01", "11"}
*/

auto main() -> int
{
    // the information needed to create the circuit representing the 4x4 unitary matrix
    auto stream = std::stringstream {
        "NUMBER_OF_COMMANDS : 5                          \n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        " -5.5557023301960196e-01  8.3146961230254535e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        " -5.5557023301960196e-01 -8.3146961230254546e-01\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        "  8.8192126434835483e-01 -4.7139673682599792e-01\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  8.8192126434835494e-01  4.7139673682599792e-01\n"
        "ALLCONTROL : 0                                  \n"
        " -5.5557023301960218e-01 -8.3146961230254524e-01\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -7.0710678118654724e-01  7.0710678118654779e-01\n"
    };

    // create the circuit representing the 4x4 unitary matrix
    const auto gates = mqis::read_decomposed_gate_info(stream);
    const auto unitary_op_circuit = mqis::make_circuit_from_decomposed_gates(gates);

    // make the unitary operator circuit a controlled circuit
    // - it will be controlled by 6 other qubits, in a binary controlled manner
    auto subcircuit = mqis::make_binary_controlled_circuit_naive(unitary_op_circuit, 8, {0, 1, 2, 3, 4, 5}, {6, 7});

    // create the circuit needed to perform quantum phase estimation
    auto circuit = mqis::QuantumCircuit {8};
    circuit.add_h_gate({0, 1, 2, 3, 4, 5});
    mqis::extend_circuit(circuit, subcircuit);
    mqis::apply_inverse_fourier_transform(circuit, {5, 4, 3, 2, 1, 0});
    // circuit.add_m_gate({0, 1, 2, 3, 4, 5});

    // create the input statevector
    // - we set the eigenstates for the unitary operator directly, rather than through x-gates
    auto counting_statevector = mqis::QuantumState {"000000"};
    auto unitary_eigenstatevector = mqis::QuantumState {"11"};
    auto statevector = mqis::tensor_product(counting_statevector, unitary_eigenstatevector);

    // perform the simulation
    mqis::simulate(circuit, statevector);

    // perform the measurements
    const auto counts = mqis::perform_measurements_as_counts_marginal(statevector, 1024, {6, 7});

    // output the results
    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";

        const auto rstripped_bitstring = mqis::rstrip_marginal_bits(bitstring);
        const auto state_index = mqis::bitstring_to_state_index(rstripped_bitstring);
        const auto n_states = 1ul << 6;
        const auto binary_fraction = static_cast<double>(state_index) / static_cast<double>(n_states);

        const auto estimated_phase = 2.0 * M_PI * binary_fraction;

        // the internal layout of the mini-qiskit simulator is little-endian, so we need
        // to reverse the bitstring to get the binary expansion used to calculate the phase
        auto binary_expansion = rstripped_bitstring;
        std::reverse(binary_expansion.begin(), binary_expansion.end());

        std::cout << "binary expansion: " << binary_expansion << '\n';
        std::cout << "estimated phase:  " << estimated_phase << '\n';
    }
    // Expected output (for "00"):
    // ```
    // (state, count) = (011010xx, 1024)
    // binary expansion: 010110
    // estimated phase:  2.15984
    // ```
    //
    // Expected output (for "10"):
    // ```
    // (state, count) = (101001xx, 1024)
    // binary expansion: 100101
    // estimated phase:  3.63247
    // ```
    //
    // Expected output (for "01"):
    // ```
    // (state, count) = (010101xx, 1024)
    // binary expansion: 101010
    // estimated phase:  4.12334
    // ```
    //
    // Expected output (for "11"):
    // ```
    // (state, count) = (101110xx, 1024)
    // binary expansion: 011101
    // estimated phase:  2.84707
    // ```

    return 0;
}

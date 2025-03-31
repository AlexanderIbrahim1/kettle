#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/decomposed/read_tangelo_file.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/circuit_operations/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state.hpp>
#include <mini-qiskit/io/statevector.hpp>

/*
    An attempt at performing QPE for the 9-qubit gate for the rotor paper.

    The output from the program upon creating the quantum circuit is "Killed"
      - this indicates that the program is running out of memory even constructing the circuit
      - even creating the first circuit results in ~26.8 million gates, just before the process is killed
        - so my naive method of creating the gates is too inefficient
*/

/*
    trotter20_init_circ.dat
    trotter20_qft_circuit.dat
    trotter20_0.dat
    trotter20_1.dat
    trotter20_2.dat
    trotter20_3.dat
    trotter20_iqft_circuit.dat
*/

static const auto SHAEER_FILEPATH = []() {
    const auto home = std::getenv("HOME");

    return std::filesystem::path(home) / "research" / "quantum-decompose" / "playground" / "shaeer_code";
}();

void extend(mqis::QuantumCircuit& circuit, const std::filesystem::path& filepath)
{
    auto right_circuit = mqis::read_tangelo_circuit(13, filepath, 2);
    mqis::extend_circuit(circuit, right_circuit);
}

auto main() -> int
{
    auto circuit = mqis::QuantumCircuit {13};
    extend(circuit, SHAEER_FILEPATH / "trotter20_init_circ.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_qft_circuit.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_0.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_1.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_2.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_3.dat");
    extend(circuit, SHAEER_FILEPATH / "trotter20_iqft_circuit.dat");
    circuit.add_m_gate({9, 10, 11, 12});

    // // make the unitary operator circuit a controlled circuit
    // // - it will be controlled by 4 other qubits, in a binary controlled manner
    // auto subcircuit = mqis::make_binary_controlled_circuit_from_binary_powers(unitary_op_circuits, 13, {0, 1, 2, 3}, {4, 5, 6, 7, 8, 9, 10, 11, 12});

    // // create the circuit needed to perform quantum phase estimation
    // auto circuit = mqis::QuantumCircuit {13};
    // circuit.add_h_gate({0, 1, 2, 3});
    // mqis::extend_circuit(circuit, subcircuit);
    // mqis::apply_inverse_fourier_transform(circuit, {3, 2, 1, 0});
    // circuit.add_m_gate({0, 1, 2, 3});

    // create the input statevector
    // - we set the eigenstates for the unitary operator directly, rather than through x-gates
    auto statevector = mqis::QuantumState {13};

    // auto unitary_eigenstatevector = []() {
    //     const auto filepath = SHAEER_FILEPATH / "trotter_ansatz.dat";
    //     return mqis::read_numpy_statevector(filepath, mqis::QuantumStateEndian::BIG);
    // }();

    // auto statevector = mqis::tensor_product(counting_statevector, unitary_eigenstatevector);

    // perform the simulation
    mqis::simulate_multithreaded(circuit, statevector, 4);

    // mqis::save_statevector("trotter20_example_statevector.dat", statevector);
    // auto statevector = mqis::load_statevector("trotter20_example_statevector.dat");

    // std::cout << "F\n";

    // perform the measurements
    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, statevector, 1ul << 12);

    // output the results
    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";

        // const auto rstripped_bitstring = mqis::rstrip_marginal_bits(bitstring);
        // const auto state_index = mqis::bitstring_to_state_index(rstripped_bitstring);
        // const auto n_states = 1ul << 6;
        // const auto binary_fraction = static_cast<double>(state_index) / static_cast<double>(n_states);

        // const auto estimated_phase = 2.0 * M_PI * binary_fraction;

        // // the internal layout of the mini-qiskit simulator is little-endian, so we need
        // // to reverse the bitstring to get the binary expansion used to calculate the phase
        // auto binary_expansion = rstripped_bitstring;
        // std::reverse(binary_expansion.begin(), binary_expansion.end());

        // std::cout << "binary expansion: " << binary_expansion << '\n';
        // std::cout << "estimated phase:  " << estimated_phase << '\n';
    }

    return 0;
}


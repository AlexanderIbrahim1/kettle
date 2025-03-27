#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/decomposed/decomposed_gate.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>
#include <mini-qiskit/circuit_operations/append_circuits.hpp>
#include <mini-qiskit/circuit_operations/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit_operations/make_binary_controlled_circuit.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulate.hpp>
#include <mini-qiskit/state.hpp>

/*
    An attempt at performing QPE for the 9-qubit gate for the rotor paper.

    The output from the program upon creating the quantum circuit is "Killed"
      - this indicates that the program is running out of memory even constructing the circuit
*/

static const auto SHAEER_FILEPATH = []() {
    const auto home = std::getenv("HOME");

    return std::filesystem::path(home) / "research" / "quantum-decompose" / "playground" / "shaeer_code";
}();

auto main() -> int
{
    auto unitary_op_circuits = []() {
        auto output = std::vector<mqis::QuantumCircuit> {};

        for (auto power : {1ul, 2ul, 4ul, 8ul}) {
            const auto filepath = SHAEER_FILEPATH / std::format("trotter_pow_{}.dat", power);
            std::cout << "reading power = " << power << '\n';
            auto gates = mqis::read_decomposed_gate_info(filepath);
            std::cout << "creating unitary for power = " << power << '\n';
            auto unitary_op_circuit = mqis::make_circuit_from_decomposed_gates(std::move(gates));
            std::cout << "created unitary for power = " << power << '\n';

            output.emplace_back(std::move(unitary_op_circuit));
        }

        return output;
    }();

    std::cout << "A\n";

    // make the unitary operator circuit a controlled circuit
    // - it will be controlled by 4 other qubits, in a binary controlled manner
    auto subcircuit = mqis::make_binary_controlled_circuit_from_binary_powers(unitary_op_circuits, 13, {0, 1, 2, 3}, {4, 5, 6, 7, 8, 9, 10, 11, 12});

    std::cout << "B\n";

    // create the circuit needed to perform quantum phase estimation
    auto circuit = mqis::QuantumCircuit {13};
    circuit.add_h_gate({0, 1, 2, 3});
    mqis::extend_circuit(circuit, subcircuit);
    mqis::apply_inverse_fourier_transform(circuit, {3, 2, 1, 0});
    circuit.add_m_gate({0, 1, 2, 3});

    std::cout << "C\n";

    // create the input statevector
    // - we set the eigenstates for the unitary operator directly, rather than through x-gates
    auto counting_statevector = mqis::QuantumState {"0000"};

    auto unitary_eigenstatevector = []() {
        const auto filepath = SHAEER_FILEPATH / "trotter_ansatz.dat";
        return mqis::read_statevector(filepath, mqis::QuantumStateEndian::BIG);
    }();

    std::cout << "D\n";

    auto statevector = mqis::tensor_product(counting_statevector, unitary_eigenstatevector);

    std::cout << "E\n";

    // perform the simulation
    mqis::simulate(circuit, statevector);

    std::cout << "F\n";

    // perform the measurements
    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, statevector, 1ul << 12);

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

    return 0;
}


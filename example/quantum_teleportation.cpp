#include <complex>
#include <iostream>
#include <random>

#include <mini-qiskit/mini-qiskit.hpp>

auto create_random_one_qubit_state() -> mqis::QuantumState
{
    auto device = std::random_device {};
    auto prng = std::mt19937 {device()};
    auto uniform = std::uniform_real_distribution<double> {-1.0, 1.0};

    // this fixed '0.5' prevents the (admittedly incredibly rare) edge case where all
    // the generated amplitudes evaluate to near 0.0, and we get an un-normalizable state
    auto amplitude0 = std::complex<double> {0.5, uniform(prng)};
    auto amplitude1 = std::complex<double> {uniform(prng), uniform(prng)};

    const auto total_norm = std::sqrt(std::norm(amplitude0) + std::norm(amplitude1));

    amplitude0 /= total_norm;
    amplitude1 /= total_norm;

    return mqis::QuantumState {{amplitude0, amplitude1}};
}

auto main() -> int
{
    // Alice initally holds the state
    const auto alice_qubit = create_random_one_qubit_state();

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
    const auto bob_counts = mqis::perform_measurements_as_counts_marginal(circuit, input, 1 << 12, {0, 1});

    std::cout << "ALICE: (state, count) = (0, " << alice_counts.at("0") << ")\n";
    std::cout << "ALICE: (state, count) = (1, " << alice_counts.at("1") << ")\n";
    std::cout << "BOB  : (state, count) = (0, " << bob_counts.at("xx0") << ")\n";
    std::cout << "BOB  : (state, count) = (1, " << bob_counts.at("xx1") << ")\n";
    // Expected output: the counts for Alice's original state, and Bob's delivered state,
    // should be similar

    return 0;
}

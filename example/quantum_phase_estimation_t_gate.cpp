#include <cmath>
#include <iostream>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/gates/multiplicity_controlled_u_gate.hpp>


void apply_multiplicity_controlled_t_gate_manually(mqis::QuantumCircuit& circuit)
{
    const auto angle = M_PI_4;
    circuit.add_cp_gate({{angle, 0, 3}});
    circuit.add_cp_gate({{angle, 1, 3}, {angle, 1, 3}});
    circuit.add_cp_gate({{angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}, {angle, 2, 3}});
}


auto main() -> int
{
    auto state = mqis::QuantumState {"0000"};

    const auto n_counting_qubits = 3;
    const auto n_ancilla_qubits = 1;

    auto circuit = mqis::QuantumCircuit {n_counting_qubits + n_ancilla_qubits};
    circuit.add_h_gate({0, 1, 2});
    circuit.add_x_gate(3);
    apply_multiplicity_controlled_t_gate_manually(circuit);
    mqis::apply_inverse_fourier_transform(circuit, {2, 1, 0});
    circuit.add_m_gate({0, 1, 2});

    mqis::simulate(circuit, state);

    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, state, 1024);

    for (const auto& [bitstring, count]: counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";

        const auto rstripped_bitstring = mqis::rstrip_marginal_bits(bitstring);
        const auto state_index = mqis::bitstring_to_state_index(rstripped_bitstring);
        const auto n_states = 1ul << n_counting_qubits;
        const auto binary_fraction = static_cast<double>(state_index) / static_cast<double>(n_states);

        const auto estimated_phase = 2.0 * M_PI * binary_fraction;

        std::cout << "estimated phase: " << estimated_phase << '\n';
    }
    // Expected output:
    // ```
    // (state, count) = (100x, 1024)
    // ```

    return 0;
}

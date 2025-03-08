#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include <mini-qiskit/mini-qiskit.hpp>
#include <mini-qiskit/gates/control_swap.hpp>
#include <mini-qiskit/gates/fourier.hpp>
#include <mini-qiskit/common/print.hpp>

/*
A basic implementation of Shor's algorithm, inspired by the code from:
    https://github.com/Qiskit/textbook/blob/main/notebooks/ch-algorithms/shor.ipynb

NOTE: this is a WIP
*/

auto VALID_BASES = std::unordered_set<int> {2, 4, 7, 8, 11, 13};


void control_multiplication_mod15(
    mqis::QuantumCircuit& circuit,
    int base,
    std::size_t control_qubit,
    std::size_t n_counting_qubits,
    std::size_t n_iterations
)
{
    if (!VALID_BASES.contains(base)) {
        throw std::runtime_error {"An invalid base has been entered"};
    }

    const auto i0 = n_counting_qubits;
    const auto i1 = 1 + n_counting_qubits;
    const auto i2 = 2 + n_counting_qubits;
    const auto i3 = 3 + n_counting_qubits;

    for (std::size_t i {0}; i < n_iterations; ++i) {
        if (base == 2 || base == 13) {
            mqis::apply_control_swap(circuit, control_qubit, i2, i3);
            mqis::apply_control_swap(circuit, control_qubit, i1, i2);
            mqis::apply_control_swap(circuit, control_qubit, i0, i1);
        }

        if (base == 7 || base == 8) {
            mqis::apply_control_swap(circuit, control_qubit, i0, i1);
            mqis::apply_control_swap(circuit, control_qubit, i1, i2);
            mqis::apply_control_swap(circuit, control_qubit, i2, i3);
        }

        if (base == 4 || base == 11) {
            mqis::apply_control_swap(circuit, control_qubit, i1, i3);
            mqis::apply_control_swap(circuit, control_qubit, i0, i2);
        }

        if (base == 7 || base == 11 || base == 13) {
            for (auto iq : {i0, i1, i2, i3}) {
                circuit.add_x_gate(iq);
            }
        }
    }
}

auto main() -> int
{
    const auto base = int {7};

    const auto n_counting_qubits = 8;
    const auto n_ancilla_qubits = 4;

    auto counting_qubits = std::vector<std::size_t> (n_counting_qubits);
    std::iota(counting_qubits.begin(), counting_qubits.end(), 0);

    auto circuit = mqis::QuantumCircuit {n_counting_qubits + n_ancilla_qubits};
    circuit.add_h_gate(counting_qubits);
    circuit.add_x_gate(8);

    for (std::size_t i {0}; i < n_counting_qubits; ++i) {
        const auto n_iterations = std::size_t {1 << i};
        control_multiplication_mod15(circuit, base, i, n_counting_qubits, n_iterations);
    }

    mqis::apply_inverse_fourier_transform(circuit, counting_qubits);
    circuit.add_m_gate(counting_qubits);

    auto state = mqis::QuantumState {n_counting_qubits + n_ancilla_qubits};

    mqis::simulate(circuit, state);

    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, state, 1 << 12);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}

#include <iostream>
#include <ranges>
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
            circuit.add_cx_gate(control_qubit, i0);
            circuit.add_cx_gate(control_qubit, i1);
            circuit.add_cx_gate(control_qubit, i2);
            circuit.add_cx_gate(control_qubit, i3);
        }
    }
}

auto main() -> int
{
    const auto base = int {7};

    const auto n_counting_qubits = 8ul;
    const auto n_ancilla_qubits = 4ul;

    const auto counting_qubits = std::vector<std::size_t> {0, 1, 2, 3, 4, 5, 6, 7};

    auto circuit = mqis::QuantumCircuit {n_counting_qubits + n_ancilla_qubits};
    circuit.add_h_gate(counting_qubits);
    circuit.add_x_gate(n_counting_qubits);

    for (auto i : std::views::iota(0ul, n_counting_qubits) | std::views::reverse) {
        const auto n_iterations = 1ul << i;
        control_multiplication_mod15(circuit, base, i, n_counting_qubits, n_iterations);
    }

    mqis::apply_inverse_fourier_transform(circuit, counting_qubits);
    circuit.add_m_gate(counting_qubits);

    auto state = mqis::QuantumState {n_counting_qubits + n_ancilla_qubits};

    mqis::simulate(circuit, state);

    // mqis::print_state(state);

    const auto counts = mqis::perform_measurements_as_counts_marginal(circuit, state, 1 << 10);

    for (const auto& [bitstring, count] : counts) {
        std::cout << "(state, count) = (" << bitstring << ", " << count << ")\n";
    }

    return 0;
}

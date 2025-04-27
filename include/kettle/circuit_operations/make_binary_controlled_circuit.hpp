#pragma once

#include <stdexcept>
#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/circuit_operations/append_circuits.hpp"
#include "kettle/circuit_operations/make_controlled_circuit.hpp"


namespace impl_ket
{

template <QubitIndices Container = impl_ket::QubitIndicesIList>
inline void check_subcircuit_powers_(
    const std::vector<ket::QuantumCircuit>& subcircuit_powers,
    const Container& control_qubits
)
{
    if (subcircuit_powers.size() != get_container_size(control_qubits)) {
        throw std::runtime_error {"The number of circuits passed does not match the number of control qubits."};
    }
}

}  // namespace impl_ket

namespace ket
{

/*
    This function creates a binary-controlled circuit by repeating the subcircuit
    the required number of times. This is slower than finding a way to combine the
    circuits and creating a new gate with each iteration.
*/
template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
inline auto make_binary_controlled_circuit_naive(
    const QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    auto new_circuit = QuantumCircuit {n_new_qubits};

    const auto size = impl_ket::get_container_size(control_qubits);

    for (std::size_t i {0}; i < size; ++i) {
        const auto control = impl_ket::get_container_index(control_qubits, i);
        const auto n_iterations = 1UL << i;

        for (std::size_t i_iter {0}; i_iter < n_iterations; ++i_iter) {
            const auto controlled_subcircuit = make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
            extend_circuit(new_circuit, controlled_subcircuit);
        }
    }

    return new_circuit;
}

/*
    This function creates a binary-controlled circuit by accepting a container of
    increasing binary powers of the subcircuit in question.
    
    It assumes that the circuits are the correct binary powers. This results in
    fewer gates than the naive approach.
*/
template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
inline auto make_binary_controlled_circuit_from_binary_powers(
    const std::vector<QuantumCircuit>& subcircuit_powers,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    impl_ket::check_subcircuit_powers_(subcircuit_powers, control_qubits);

    auto new_circuit = QuantumCircuit {n_new_qubits};
    const auto size = impl_ket::get_container_size(control_qubits);

    for (std::size_t i {0}; i < size; ++i) {
        const auto control = impl_ket::get_container_index(control_qubits, i);

        const auto& subcircuit = subcircuit_powers[i];
        const auto controlled_subcircuit = make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
        extend_circuit(new_circuit, controlled_subcircuit);
    }

    return new_circuit;
}

}  // namespace ket

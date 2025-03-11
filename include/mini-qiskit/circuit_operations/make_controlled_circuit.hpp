#pragma once

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/gates/multiplicity_controlled_u_gate.hpp"
#include "mini-qiskit/primitive_gate.hpp"


namespace impl_mqis
{

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void check_all_indices_are_unique_(const Container& container)
{
    auto seen = std::unordered_set<std::size_t> {};
    for (auto index : container) {
        if (seen.contains(index)) {
            throw std::runtime_error {"The new qubit indices for the controlled circuit must be unique."};
        }

        seen.insert(index);
    }
}

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void check_valid_number_of_mapped_indices_(const Container& container, const mqis::QuantumCircuit& circuit)
{
    const auto size = impl_mqis::get_container_size(container);

    if (size != circuit.n_qubits()) {
        throw std::runtime_error {
            "The number of proposed new qubit indices does not match the number of qubits in the subcircuit."
        };
    }
}

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void check_control_qubit_is_not_a_mapped_qubit_(const Container& container, std::size_t control_qubit)
{
    const auto is_control_qubit = [&](std::size_t i) { return i == control_qubit; };
    const auto it = std::find_if(container.begin(), container.end(), is_control_qubit);

    if (it != container.end()) {
        throw std::runtime_error {"The control qubit cannot be one of the mapped qubit indices"};
    }
}

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void check_new_indices_fit_onto_new_circuit_(const Container& container, std::size_t control_qubit, std::size_t n_qubits_on_new_circuit)
{
    // the additional '1' comes from the control qubit
    const auto n_minimum_new_indices = impl_mqis::get_container_size(container) + 1;
    if (n_minimum_new_indices > n_qubits_on_new_circuit) {
        throw std::runtime_error {"The mapped qubits will not fit onto the new circuit"};
    }

    const auto is_outside_new_qubits = [&](std::size_t i) { return i >= n_qubits_on_new_circuit; };

    if (is_outside_new_qubits(control_qubit)) {
        throw std::runtime_error {"The control qubit is outside the range of qubits on the new circuit."};
    }

    if (std::any_of(container.begin(), container.end(), is_outside_new_qubits)) {
        throw std::runtime_error {"A mapped qubit was found to be outside the range of qubits on the new circuit."};
    }
}

}  // namespace impl_mqis

namespace mqis
{

/*
PLAN:

Take an existing QuantumCircuit instance, and make the execution of the entire circuit
dependent on a control qubit outside of that circuit.

The existing QuantumCircuit gets turned into a new QuantumCircuit instance.

The function should take as arguments:
  - the QuantumCircuit instance to be made controlled
  - the control qubit
  - the new qubit indices that the qubits of the current QuantumCircuit instance get mapped to

Later I should make a multiplicity-controlled version of this function
  - I would have to turn all the general gates into U-gates
*/
template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
inline auto make_controlled_circuit(
    const mqis::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const Container& container
) -> mqis::QuantumCircuit
{
    impl_mqis::check_valid_number_of_mapped_indices_(container, subcircuit);
    impl_mqis::check_all_indices_are_unique_(container);
    impl_mqis::check_control_qubit_is_not_a_mapped_qubit_(container, control);
    impl_mqis::check_new_indices_fit_onto_new_circuit_(container, control, n_new_qubits);

    auto new_circuit = mqis::QuantumCircuit {n_new_qubits};

    for (const auto& gate_info : subcircuit) {
        switch (gate_info.gate)
        {
            case Gate::X : {
                const auto original_target = impl_mqis::unpack_x_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                new_circuit.add_cu_gate(x_gate(), control, new_target);
                break;
            }
            case Gate::RX : {
                const auto [angle, original_target] = impl_mqis::unpack_rx_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                new_circuit.add_cu_gate(rx_gate(angle), control, new_target);
                break;
            }
            case Gate::H : {
                const auto original_target = impl_mqis::unpack_h_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                new_circuit.add_cu_gate(h_gate(), control, new_target);
                break;
            }
            case Gate::CX : {
                const auto [original_control, original_target] = impl_mqis::unpack_cx_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(container, original_control);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, x_gate(), new_target, {control, new_control});
                break;
            }
            case Gate::CRX : {
                const auto [original_control, original_target, angle] = impl_mqis::unpack_crx_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(container, original_control);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, rx_gate(angle), new_target, {control, new_control});
                break;
            }
            case Gate::CP : {
                const auto [original_control, original_target, angle] = impl_mqis::unpack_cp_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(container, original_control);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, p_gate(angle), new_target, {control, new_control});
                break;
            }
            case Gate::U : {
                const auto [original_target, original_gate_index] = impl_mqis::unpack_u_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                const auto& matrix = subcircuit.unitary_gate(original_gate_index);
                new_circuit.add_cu_gate(matrix, control, new_target);
                break;
            }
            case Gate::CU : {
                const auto [original_control, original_target, original_gate_index] = impl_mqis::unpack_cu_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(container, original_control);
                const auto new_target = impl_mqis::get_container_index(container, original_target);
                const auto& matrix = subcircuit.unitary_gate(original_gate_index);
                apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, {control, new_control});
                break;
            }
            case Gate::M : {
                throw std::runtime_error {"Cannot make a measurement gate controlled."};
            }
        }
    }

    return new_circuit;
}

}  // namespace mqis

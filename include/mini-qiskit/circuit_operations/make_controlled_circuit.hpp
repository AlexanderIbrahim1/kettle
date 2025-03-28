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
void check_no_overlap_between_control_qubits_and_mapped_qubits_(const Container& mapped_qubits, const Container& control_qubits)
{
    auto control_qubit_set = std::unordered_set<std::size_t> {control_qubits.begin(), control_qubits.end()};

    for (auto mapped_qubit : mapped_qubits) {
        if (control_qubit_set.contains(mapped_qubit)) {
            throw std::runtime_error {"The control qubit indices cannot overlap with the mapped qubit indices"};
        }
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

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
void check_new_indices_fit_onto_new_circuit_(const Container& mapped_qubits, const Container& control_qubits, std::size_t n_qubits_on_new_circuit)
{
    const auto n_mapped_indices = impl_mqis::get_container_size(mapped_qubits);
    const auto n_control_indices = impl_mqis::get_container_size(control_qubits);
    const auto n_minimum_new_indices = n_mapped_indices + n_control_indices;

    if (n_minimum_new_indices > n_qubits_on_new_circuit) {
        throw std::runtime_error {"The mapped qubits will not fit onto the new circuit"};
    }

    const auto is_outside_new_qubits = [&](std::size_t i) { return i >= n_qubits_on_new_circuit; };

    if (std::any_of(control_qubits.begin(), control_qubits.end(), is_outside_new_qubits)) {
        throw std::runtime_error {"The control qubit is outside the range of qubits on the new circuit."};
    }

    if (std::any_of(mapped_qubits.begin(), mapped_qubits.end(), is_outside_new_qubits)) {
        throw std::runtime_error {"A mapped qubit was found to be outside the range of qubits on the new circuit."};
    }
}

}  // namespace impl_mqis

namespace mqis
{

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
inline auto make_multiplicity_controlled_circuit(
    const mqis::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> mqis::QuantumCircuit
{
    impl_mqis::check_valid_number_of_mapped_indices_(mapped_qubits, subcircuit);
    impl_mqis::check_all_indices_are_unique_(mapped_qubits);
    impl_mqis::check_all_indices_are_unique_(control_qubits);
    impl_mqis::check_no_overlap_between_control_qubits_and_mapped_qubits_(mapped_qubits, control_qubits);
    impl_mqis::check_new_indices_fit_onto_new_circuit_(mapped_qubits, control_qubits, n_new_qubits);

    auto new_circuit = mqis::QuantumCircuit {n_new_qubits};

    for (const auto& gate_info : subcircuit) {
        switch (gate_info.gate)
        {
            case Gate::X : {
                const auto original_target = impl_mqis::unpack_x_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, x_gate(), new_target, control_qubits);
                break;
            }
            case Gate::RX : {
                const auto [angle, original_target] = impl_mqis::unpack_rx_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, rx_gate(angle), new_target, control_qubits);
                break;
            }
            case Gate::RZ : {
                const auto [angle, original_target] = impl_mqis::unpack_rz_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, rz_gate(angle), new_target, control_qubits);
                break;
            }
            case Gate::H : {
                const auto original_target = impl_mqis::unpack_h_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                apply_multiplicity_controlled_u_gate(new_circuit, h_gate(), new_target, control_qubits);
                break;
            }
            case Gate::CX : {
                const auto [original_control, original_target] = impl_mqis::unpack_cx_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(mapped_qubits, original_control);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                const auto new_controls = impl_mqis::extend_container_to_vector(control_qubits, {new_control});
                apply_multiplicity_controlled_u_gate(new_circuit, x_gate(), new_target, new_controls);
                break;
            }
            case Gate::CRX : {
                const auto [original_control, original_target, angle] = impl_mqis::unpack_crx_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(mapped_qubits, original_control);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                const auto new_controls = impl_mqis::extend_container_to_vector(control_qubits, {new_control});
                apply_multiplicity_controlled_u_gate(new_circuit, rx_gate(angle), new_target, new_controls);
                break;
            }
            case Gate::CP : {
                const auto [original_control, original_target, angle] = impl_mqis::unpack_cp_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(mapped_qubits, original_control);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                const auto new_controls = impl_mqis::extend_container_to_vector(control_qubits, {new_control});
                apply_multiplicity_controlled_u_gate(new_circuit, p_gate(angle), new_target, new_controls);
                break;
            }
            case Gate::U : {
                const auto [original_target, original_gate_index] = impl_mqis::unpack_u_gate(gate_info);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                const auto& matrix = subcircuit.unitary_gate(original_gate_index);
                apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, control_qubits);
                break;
            }
            case Gate::CU : {
                const auto [original_control, original_target, original_gate_index] = impl_mqis::unpack_cu_gate(gate_info);
                const auto new_control = impl_mqis::get_container_index(mapped_qubits, original_control);
                const auto new_target = impl_mqis::get_container_index(mapped_qubits, original_target);
                const auto new_controls = impl_mqis::extend_container_to_vector(control_qubits, {new_control});
                const auto& matrix = subcircuit.unitary_gate(original_gate_index);
                apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, new_controls);
                break;
            }
            case Gate::M : {
                throw std::runtime_error {"Cannot make a measurement gate controlled."};
            }
        }
    }

    return new_circuit;
}

template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
inline auto make_controlled_circuit(
    const mqis::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const Container& mapped_qubits
) -> mqis::QuantumCircuit
{
    return make_multiplicity_controlled_circuit(subcircuit, n_new_qubits, {control}, mapped_qubits);
}

}  // namespace mqis

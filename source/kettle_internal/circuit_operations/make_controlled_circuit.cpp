#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/make_controlled_circuit.hpp"
#include "kettle_internal/common/utils_internal.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/multiplicity_controlled_u_gate.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/toffoli.hpp"

#include "kettle_internal/gates/primitive_gate/gate_id.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/gates/primitive_gate_map.hpp"


namespace
{

template <ket::QubitIndices Container = ket::QubitIndicesIList>
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


template <ket::QubitIndices Container = ket::QubitIndicesIList>
void check_valid_number_of_mapped_indices_(const Container& container, const ket::QuantumCircuit& circuit)
{
    const auto size = ket::internal::get_container_size(container);

    if (size != circuit.n_qubits()) {
        throw std::runtime_error {
            "The number of proposed new qubit indices does not match the number of qubits in the subcircuit."
        };
    }
}


template <ket::QubitIndices Container = ket::QubitIndicesIList>
void check_control_qubit_is_not_a_mapped_qubit_(const Container& container, std::size_t control_qubit)
{
    const auto is_control_qubit = [&](std::size_t i) { return i == control_qubit; };
    const auto it = std::find_if(container.begin(), container.end(), is_control_qubit);

    if (it != container.end()) {
        throw std::runtime_error {"The control qubit cannot be one of the mapped qubit indices"};
    }
}


template <ket::QubitIndices Container = ket::QubitIndicesIList>
void check_no_overlap_(const Container& mapped_qubits, const Container& control_qubits)
{
    auto control_qubit_set = std::unordered_set<std::size_t> {control_qubits.begin(), control_qubits.end()};

    for (auto mapped_qubit : mapped_qubits) {
        if (control_qubit_set.contains(mapped_qubit)) {
            throw std::runtime_error {"The control qubit indices cannot overlap with the mapped qubit indices"};
        }
    }
}


template <ket::QubitIndices Container = ket::QubitIndicesIList>
void check_new_indices_fit_onto_new_circuit_(const Container& mapped_qubits, const Container& control_qubits, std::size_t n_qubits_on_new_circuit)
{
    const auto n_mapped_indices = ket::internal::get_container_size(mapped_qubits);
    const auto n_control_indices = ket::internal::get_container_size(control_qubits);
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


void make_one_target_gate_controlled(
    ket::QuantumCircuit& circuit,
    ket::Gate gate,
    std::size_t control,
    std::size_t target
)
{
    const auto controlled_gate = ket::internal::UNCONTROLLED_TO_CONTROLLED_GATE.at(gate);
    const auto controlled_gate_operation = ket::internal::GATE_TO_FUNCTION_1C1T.at(controlled_gate);
    (circuit.*controlled_gate_operation)(control, target);
}


void make_one_target_one_angle_gate_controlled(
    ket::QuantumCircuit& circuit,
    ket::Gate gate,
    std::size_t control,
    std::size_t target,
    double angle
)
{
    const auto controlled_gate = ket::internal::UNCONTROLLED_TO_CONTROLLED_GATE.at(gate);
    const auto controlled_gate_operation = ket::internal::GATE_TO_FUNCTION_1C1T1A.at(controlled_gate);
    (circuit.*controlled_gate_operation)(control, target, angle);
}

}  // namespace


namespace ket
{

template <QubitIndices Container>
auto make_controlled_circuit(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    namespace gid = ket::internal::gate_id;
    namespace cre = ket::internal::create;

    check_valid_number_of_mapped_indices_(mapped_qubits, subcircuit);
    check_all_indices_are_unique_(mapped_qubits);
    check_all_indices_are_unique_({control});
    check_no_overlap_(mapped_qubits, {control});
    check_new_indices_fit_onto_new_circuit_(mapped_qubits, {control}, n_new_qubits);

    auto new_circuit = ket::QuantumCircuit {n_new_qubits};

    for (const auto& circuit_element : subcircuit) {
        if (circuit_element.is_control_flow()) {
            throw std::runtime_error {"ERROR: classical control flow statement cannot be made controlled.\n"};
        }

        if (circuit_element.is_circuit_logger()) {
            new_circuit.add_circuit_logger(circuit_element.get_circuit_logger());
            continue;
        }

        const auto& gate_info = circuit_element.get_gate();

        if (gid::is_one_target_transform_gate(gate_info.gate)) {
            const auto original_target = cre::unpack_one_target_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            make_one_target_gate_controlled(new_circuit, gate_info.gate, control, new_target);
        }
        else if (gid::is_one_target_one_angle_transform_gate(gate_info.gate))
        {
            const auto [original_target, angle] = cre::unpack_one_target_one_angle_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            make_one_target_one_angle_gate_controlled(new_circuit, gate_info.gate, control, new_target, angle);
        }
        else if (gid::is_one_control_one_target_transform_gate(gate_info.gate)) {
            const auto [original_control, original_target] = cre::unpack_one_control_one_target_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto matrix = non_angle_gate(gate_info.gate);
            apply_doubly_controlled_gate(new_circuit, matrix, {control, new_control}, new_target);
        }
        else if (gid::is_one_control_one_target_one_angle_transform_gate(gate_info.gate)) {
            const auto [original_control, original_target, angle] = cre::unpack_one_control_one_target_one_angle_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto matrix = angle_gate(gate_info.gate, angle);
            apply_doubly_controlled_gate(new_circuit, matrix, {control, new_control}, new_target);
        }
        else if (gate_info.gate == Gate::U) {
            const auto [original_target, unitary_ptr] = cre::unpack_u_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            new_circuit.add_cu_gate(*unitary_ptr, control, new_target);
        }
        else if (gate_info.gate == Gate::CU) {
            const auto [original_control, original_target, unitary_ptr] = cre::unpack_cu_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            apply_doubly_controlled_gate(new_circuit, *unitary_ptr, {control, new_control}, new_target);
        }
        else if (gate_info.gate == Gate::M) {
            throw std::runtime_error {"Cannot make a measurement gate controlled.\n"};
        }
        else {
            throw std::runtime_error {"UNREACHABLE: dev error, invalid gate found when making controlled circuit.\n"};
        }
    }

    return new_circuit;
}
template
auto make_controlled_circuit<ket::QubitIndicesVector>(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const ket::QubitIndicesVector& mapped_qubits
) -> ket::QuantumCircuit;
template
auto make_controlled_circuit<ket::QubitIndicesIList>(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    std::size_t control,
    const ket::QubitIndicesIList& mapped_qubits
) -> ket::QuantumCircuit;


template <QubitIndices Container>
auto make_multiplicity_controlled_circuit(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const Container& control_qubits,
    const Container& mapped_qubits
) -> ket::QuantumCircuit
{
    if (ket::internal::get_container_size(control_qubits) == 1) {
        const auto control = ket::internal::get_container_index(control_qubits, 0);
        make_controlled_circuit(subcircuit, n_new_qubits, control, mapped_qubits);
    }

    namespace gid = ket::internal::gate_id;
    namespace cre = ket::internal::create;

    check_valid_number_of_mapped_indices_(mapped_qubits, subcircuit);
    check_all_indices_are_unique_(mapped_qubits);
    check_all_indices_are_unique_(control_qubits);
    check_no_overlap_(mapped_qubits, control_qubits);
    check_new_indices_fit_onto_new_circuit_(mapped_qubits, control_qubits, n_new_qubits);

    auto new_circuit = QuantumCircuit {n_new_qubits};

    for (const auto& circuit_element : subcircuit) {
        if (circuit_element.is_control_flow()) {
            throw std::runtime_error {"ERROR: classical control flow statement cannot be made controlled.\n"};
        }

        if (circuit_element.is_circuit_logger()) {
            new_circuit.add_circuit_logger(circuit_element.get_circuit_logger());
            continue;
        }

        const auto& gate_info = circuit_element.get_gate();

        if (gid::is_one_target_transform_gate(gate_info.gate)) {
            const auto original_target = cre::unpack_one_target_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto matrix = non_angle_gate(gate_info.gate);
            apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, control_qubits);
        }
        else if (gid::is_one_target_one_angle_transform_gate(gate_info.gate))
        {
            const auto [original_target, angle] = cre::unpack_one_target_one_angle_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto matrix = angle_gate(gate_info.gate, angle);
            apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, control_qubits);
        }
        else if (gid::is_one_control_one_target_transform_gate(gate_info.gate)) {
            const auto [original_control, original_target] = cre::unpack_one_control_one_target_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto new_controls = ket::internal::extend_container_to_vector(control_qubits, {new_control});
            const auto matrix = non_angle_gate(gate_info.gate);
            apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, new_controls);
        }
        else if (gid::is_one_control_one_target_one_angle_transform_gate(gate_info.gate)) {
            const auto [original_control, original_target, angle] = cre::unpack_one_control_one_target_one_angle_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto new_controls = ket::internal::extend_container_to_vector(control_qubits, {new_control});
            const auto matrix = angle_gate(gate_info.gate, angle);
            apply_multiplicity_controlled_u_gate(new_circuit, matrix, new_target, new_controls);
        }
        else if (gate_info.gate == Gate::U) {
            const auto [original_target, unitary_ptr] = cre::unpack_u_gate(gate_info);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            apply_multiplicity_controlled_u_gate(new_circuit, *unitary_ptr, new_target, control_qubits);
        }
        else if (gate_info.gate == Gate::CU) {
            const auto [original_control, original_target, unitary_ptr] = cre::unpack_cu_gate(gate_info);
            const auto new_control = ket::internal::get_container_index(mapped_qubits, original_control);
            const auto new_target = ket::internal::get_container_index(mapped_qubits, original_target);
            const auto new_controls = ket::internal::extend_container_to_vector(control_qubits, {new_control});
            apply_multiplicity_controlled_u_gate(new_circuit, *unitary_ptr, new_target, new_controls);
        }
        else if (gate_info.gate == Gate::M) {
            throw std::runtime_error {"Cannot make a measurement gate controlled.\n"};
        }
        else {
            throw std::runtime_error {"UNREACHABLE: dev error, invalid gate found when making controlled circuit.\n"};
        }
    }

    return new_circuit;
}
template
auto make_multiplicity_controlled_circuit<ket::QubitIndicesVector>(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const ket::QubitIndicesVector& control_qubits,
    const ket::QubitIndicesVector& mapped_qubits
) -> ket::QuantumCircuit;
template
auto make_multiplicity_controlled_circuit<ket::QubitIndicesIList>(
    const ket::QuantumCircuit& subcircuit,
    std::size_t n_new_qubits,
    const ket::QubitIndicesIList& control_qubits,
    const ket::QubitIndicesIList& mapped_qubits
) -> ket::QuantumCircuit;

}  // namespace ket

#pragma once

#include <cstddef>
#include <stdexcept>

#include "kettle/circuit/circuit.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/matrix2x2_gate_decomposition.hpp"
#include "kettle/gates/primitive_gate.hpp"

/*
    This header file contains the `transpile_to_primitive()` function, which takes an
    existing `QuantumCircuit` instance that may contain matrices that use unitary 2x2
    matrices as gates, and creates a new `QuantumCircuit` instance composed of only
    primitive gates.
*/

namespace impl_ket
{

constexpr auto is_primitive_gate(ket::Gate gate) -> bool
{
    return gate_id::is_non_angle_transform_gate(gate) || gate_id::is_angle_transform_gate(gate);
}

}  // namespace impl_ket

namespace ket
{

inline auto transpile_to_primitive(
    const QuantumCircuit& circuit,
    double tolerance_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> QuantumCircuit
{
    auto new_circuit = QuantumCircuit {circuit.n_qubits(), circuit.n_bits()};

    const auto decomp_1t = impl_ket::decomp_to_one_target_primitive_gates_;
    const auto decomp_1c_1t = impl_ket::decomp_to_one_control_one_target_primitive_gates_;

    for (const auto& circuit_element : circuit.elements_) {
        if (circuit_element.is_control_flow()) {
            const auto& control_flow = circuit_element.get_control_flow();

            if (control_flow.is_if_statement()) {
                // TODO: implement when I get this thing to compile
            }
            else {
                // TODO: implement when I get this thing to compile
            }
            // const auto cfi_kind = impl_ket::control::unpack_control_flow_kind(ginfo);
            // const auto current_cfi_index = impl_ket::control::unpack_control_flow_index(ginfo);
            // auto instruction = circuit.control_flow_instructions_[current_cfi_index];

            // if (cfi_kind == impl_ket::control::IF_STMT) {
            //     const auto subcircuit = instruction.primary_circuit();
            //     auto transpiled_subcircuit = transpile_to_primitive(subcircuit, tolerance_sq);

            //     auto cfi = impl_ket::ControlFlowInstruction {
            //         std::move(instruction),
            //         std::make_unique<QuantumCircuit>(std::move(transpiled_subcircuit))
            //     };

            //     new_circuit.control_flow_instructions_.emplace_back(std::move(cfi));

            //     const auto new_cfi_index = new_circuit.control_flow_instructions_.size() - 1;
            //     new_circuit.elements_.emplace_back(impl_ket::control::create_control_flow_gate(new_cfi_index, cfi_kind));
            // }
        }

        const auto& gate_info = circuit_element.get_gate();

        if (impl_ket::is_primitive_gate(gate_info.gate) || gate_info.gate == Gate::M) {
            new_circuit.elements_.push_back(gate_info);
        }
        else if (gate_info.gate == Gate::U) {
            const auto [target, i_matrix] = impl_ket::unpack_u_gate(gate_info);
            const auto& unitary = circuit.unitaries_[i_matrix];
            const auto decomp_gates = decomp_1t(target, unitary, tolerance_sq);
            for (const auto& decomp_gate : decomp_gates) {
                new_circuit.elements_.push_back(decomp_gate);
            }
        }
        else if (gate_info.gate == Gate::CU) {
            const auto [control, target, i_matrix] = impl_ket::unpack_cu_gate(gate_info);
            const auto& unitary = circuit.unitaries_[i_matrix];
            const auto decomp_gates = decomp_1c_1t(control, target, unitary, tolerance_sq);
            for (const auto& decomp_gate : decomp_gates) {
                new_circuit.elements_.push_back(decomp_gate);
            }
        }
    }

    return new_circuit;
}

}  // namespace ket

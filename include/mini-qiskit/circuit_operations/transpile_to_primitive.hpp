#pragma once

#include <cstddef>
#include <stdexcept>

#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/matrix2x2_gate_decomposition.hpp"
#include "mini-qiskit/gates/primitive_gate.hpp"

/*
    This header file contains the `transpile_to_primitive()` function, which takes an
    existing `QuantumCircuit` instance that may contain matrices that use unitary 2x2
    matrices as gates, and creates a new `QuantumCircuit` instance composed of only
    primitive gates.
*/

namespace impl_mqis
{

constexpr auto is_primitive_gate(mqis::Gate gate) -> bool
{
    return gate_id::is_non_angle_transform_gate(gate) || gate_id::is_angle_transform_gate(gate);
}

}  // namespace impl_mqis

namespace mqis
{

inline auto transpile_to_primitive(
    const QuantumCircuit& circuit,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> QuantumCircuit
{
    auto new_circuit = QuantumCircuit {circuit.n_qubits(), circuit.n_bits()};

    const auto decomp_1t = impl_mqis::decomp_to_one_target_primitive_gates_;
    const auto decomp_1c_1t = impl_mqis::decomp_to_one_control_one_target_primitive_gates_;

    for (const auto& ginfo : circuit.gates_) {
        if (impl_mqis::is_primitive_gate(ginfo.gate) || ginfo.gate == Gate::M) {
            new_circuit.gates_.push_back(ginfo);
        }
        else if (ginfo.gate == Gate::U) {
            const auto [target, i_matrix] = impl_mqis::unpack_u_gate(ginfo);
            const auto& unitary = circuit.unitary_gates_[i_matrix];
            const auto decomp_gates = decomp_1t(target, unitary, tolerance_sq);
            for (const auto& decomp_gate : decomp_gates) {
                new_circuit.gates_.push_back(decomp_gate);
            }
        }
        else if (ginfo.gate == Gate::CU) {
            const auto [control, target, i_matrix] = impl_mqis::unpack_cu_gate(ginfo);
            const auto& unitary = circuit.unitary_gates_[i_matrix];
            const auto decomp_gates = decomp_1c_1t(control, target, unitary, tolerance_sq);
            for (const auto& decomp_gate : decomp_gates) {
                new_circuit.gates_.push_back(decomp_gate);
            }
        }
        else if (ginfo.gate == Gate::CONTROL) {
            const auto cfi_kind = impl_mqis::control::unpack_control_flow_kind(ginfo);
            const auto current_cfi_index = impl_mqis::control::unpack_control_flow_index(ginfo);
            auto instruction = circuit.control_flow_instructions_[current_cfi_index];

            if (cfi_kind == impl_mqis::control::IF_STMT) {
                const auto subcircuit = instruction.primary_circuit();
                auto transpiled_subcircuit = transpile_to_primitive(subcircuit, tolerance_sq);

                auto cfi = impl_mqis::ControlFlowInstruction {
                    std::move(instruction),
                    std::make_unique<QuantumCircuit>(std::move(transpiled_subcircuit))
                };

                new_circuit.control_flow_instructions_.emplace_back(std::move(cfi));

                const auto new_cfi_index = new_circuit.control_flow_instructions_.size() - 1;
                new_circuit.gates_.emplace_back(impl_mqis::control::create_control_flow_gate(new_cfi_index, cfi_kind));
            }
        }
        else {
            throw std::runtime_error {"DEV ERROR: found a gate that couldn't be transpiled.\n"};
        }
    }

    return new_circuit;
}

}  // namespace mqis

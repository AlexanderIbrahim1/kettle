#pragma once

#include <stdexcept>

#include "kettle/circuit/circuit.hpp"
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

// NOLINTNEXTLINE(misc-no-recursion)
inline auto transpile_to_primitive(
    const QuantumCircuit& circuit,
    double tolerance_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> QuantumCircuit
{
    auto new_circuit = QuantumCircuit {circuit.n_qubits(), circuit.n_bits()};

    const auto decomp_1t = impl_ket::decomp_to_one_target_primitive_gates_;
    const auto decomp_1c_1t = impl_ket::decomp_to_one_control_one_target_primitive_gates_;

    // TODO: implement circuit logger functionality

    for (const auto& circuit_element : circuit.elements_) {
        if (circuit_element.is_control_flow()) {
            const auto& control_flow = circuit_element.get_control_flow();

            if (control_flow.is_if_statement()) {
                const auto& if_stmt = control_flow.get_if_statement();
                const auto& current_subcircuit = *if_stmt.circuit();
                auto transpiled_subcircuit = transpile_to_primitive(current_subcircuit, tolerance_sq);

                auto cfi = impl_ket::ClassicalIfStatement {
                    if_stmt.predicate(),
                    std::make_unique<QuantumCircuit>(std::move(transpiled_subcircuit))
                };

                new_circuit.elements_.emplace_back(std::move(cfi));
            }
            else if (control_flow.is_if_else_statement()) {
                const auto& if_else_stmt = control_flow.get_if_else_statement();
                const auto& if_subcircuit = *if_else_stmt.if_circuit();
                const auto& else_subcircuit = *if_else_stmt.else_circuit();
                auto transpiled_if_subcircuit = transpile_to_primitive(if_subcircuit, tolerance_sq);
                auto transpiled_else_subcircuit = transpile_to_primitive(else_subcircuit, tolerance_sq);

                auto cfi = impl_ket::ClassicalIfElseStatement {
                    if_else_stmt.predicate(),
                    std::make_unique<QuantumCircuit>(std::move(transpiled_if_subcircuit)),
                    std::make_unique<QuantumCircuit>(std::move(transpiled_else_subcircuit))
                };

                new_circuit.elements_.emplace_back(std::move(cfi));
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid control flow element found\n"};
            }
        }
        else {
            const auto& gate_info = circuit_element.get_gate();

            if (impl_ket::is_primitive_gate(gate_info.gate) || gate_info.gate == Gate::M) {
                new_circuit.elements_.emplace_back(gate_info);
            }
            else if (gate_info.gate == Gate::U) {
                const auto [target, unitary_ptr] = impl_ket::unpack_u_gate(gate_info);
                const auto decomp_gates = decomp_1t(target, *unitary_ptr, tolerance_sq);
                for (const auto& decomp_gate : decomp_gates) {
                    new_circuit.elements_.emplace_back(decomp_gate);
                }
            }
            else if (gate_info.gate == Gate::CU) {
                const auto [control, target, unitary_ptr] = impl_ket::unpack_cu_gate(gate_info);
                const auto decomp_gates = decomp_1c_1t(control, target, *unitary_ptr, tolerance_sq);
                for (const auto& decomp_gate : decomp_gates) {
                    new_circuit.elements_.emplace_back(decomp_gate);
                }
            }
        }
    }

    return new_circuit;
}

}  // namespace ket

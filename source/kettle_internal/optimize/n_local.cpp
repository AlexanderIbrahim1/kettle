#include <vector>

#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/primitive_gate.hpp>
#include <kettle/optimize/n_local.hpp>

#include <kettle_internal/gates/primitive_gate/gate_id.hpp>
#include <kettle_internal/gates/primitive_gate_map.hpp>

namespace 
{

void verify_valid_gates(const std::vector<ket::Gate>& gates)
{
    for (auto gate : gates) {
        if (gate == ket::Gate::U || gate == ket::Gate::CU || gate == ket::Gate::M) {
            throw std::runtime_error {"ERROR: cannot create n-local circuit with U, CU, or M gates.\n"};
        }
    }
}

void apply_rotation_gates(ket::QuantumCircuit& circuit, const std::vector<ket::Gate>& rotation_blocks)
{
    for (auto gate : rotation_blocks) {
        if (ket::internal::gate_id::is_non_angle_transform_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1T.at(gate);
            for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
                (circuit.*func)(i_target);
            }
        }
        else if (ket::internal::gate_id::is_angle_transform_gate(gate)) {
            const auto func = ket::internal::GATE_TO_FUNCTION_1T1A.at(gate);
            for (std::size_t i_target {0}; i_target < circuit.n_qubits(); ++i_target) {
                (circuit.*func)(i_target);
            }
        }
    }
}

}  // namespace


namespace ket
{


}  // namespace ket

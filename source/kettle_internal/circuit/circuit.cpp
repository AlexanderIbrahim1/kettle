#include <cmath>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/circuit_loggers/classical_register_circuit_logger.hpp"
#include "kettle/circuit_loggers/statevector_circuit_logger.hpp"
#include "kettle_internal/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/control_flow.hpp"

#include "kettle/circuit/circuit.hpp"


namespace ket
{

void QuantumCircuit::pop_back()
{
    if (elements_.size() == 0) {
        throw std::runtime_error {"ERROR: called `pop_back()` on a `QuantumCircuit` instance with no circuit elements.\n"};
    }

    elements_.pop_back();
}

void QuantumCircuit::add_h_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "H");
    elements_.emplace_back(impl_ket::create_one_target_gate(Gate::H, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_h_gate(const Container& indices)
{
    for (auto index : indices) {
        add_h_gate(index);
    }
}
template void QuantumCircuit::add_h_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_h_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_x_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "X");
    elements_.emplace_back(impl_ket::create_one_target_gate(Gate::X, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_x_gate(const Container& indices)
{
    for (auto index : indices) {
        add_x_gate(index);
    }
}
template void QuantumCircuit::add_x_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_x_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_y_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "Y");
    elements_.emplace_back(impl_ket::create_one_target_gate(Gate::Y, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_y_gate(const Container& indices)
{
    for (auto index : indices) {
        add_y_gate(index);
    }
}
template void QuantumCircuit::add_y_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_y_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_z_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "Z");
    elements_.emplace_back(impl_ket::create_one_target_gate(Gate::Z, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_z_gate(const Container& indices)
{
    for (auto index : indices) {
        add_z_gate(index);
    }
}
template void QuantumCircuit::add_z_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_z_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_sx_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "SX");
    elements_.emplace_back(impl_ket::create_one_target_gate(Gate::SX, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_sx_gate(const Container& indices)
{
    for (auto index : indices) {
        add_sx_gate(index);
    }
}
template void QuantumCircuit::add_sx_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_sx_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_rx_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RX");
    elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RX, target_index, angle));
}

template <impl_ket::QubitIndicesAndAngles Container>
void QuantumCircuit::add_rx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_rx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_rx_gate<impl_ket::QubitIndicesAndAnglesVector>(const impl_ket::QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_rx_gate<impl_ket::QubitIndicesAndAnglesIList>(const impl_ket::QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_ry_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RY");
    elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RY, target_index, angle));
}

template <impl_ket::QubitIndicesAndAngles Container>
void QuantumCircuit::add_ry_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_ry_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_ry_gate<impl_ket::QubitIndicesAndAnglesVector>(const impl_ket::QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_ry_gate<impl_ket::QubitIndicesAndAnglesIList>(const impl_ket::QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_rz_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RZ");
    elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RZ, target_index, angle));
}

template <impl_ket::QubitIndicesAndAngles Container>
void QuantumCircuit::add_rz_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_rz_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_rz_gate<impl_ket::QubitIndicesAndAnglesVector>(const impl_ket::QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_rz_gate<impl_ket::QubitIndicesAndAnglesIList>(const impl_ket::QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_p_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "P");
    elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::P, target_index, angle));
}

template <impl_ket::QubitIndicesAndAngles Container>
void QuantumCircuit::add_p_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_p_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_p_gate<impl_ket::QubitIndicesAndAnglesVector>(const impl_ket::QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_p_gate<impl_ket::QubitIndicesAndAnglesIList>(const impl_ket::QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_ch_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CH");
    check_qubit_range_(target_index, "target qubit", "CH");
    elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CH, control_index, target_index));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_ch_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_ch_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_ch_gate<impl_ket::ControlAndTargetIndicesVector>(const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_ch_gate<impl_ket::ControlAndTargetIndicesIList>(const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cx_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CX");
    check_qubit_range_(target_index, "target qubit", "CX");
    elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CX, control_index, target_index));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_cx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cx_gate<impl_ket::ControlAndTargetIndicesVector>(const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cx_gate<impl_ket::ControlAndTargetIndicesIList>(const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cy_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CY");
    check_qubit_range_(target_index, "target qubit", "CY");
    elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CY, control_index, target_index));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_cy_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cy_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cy_gate<impl_ket::ControlAndTargetIndicesVector>(const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cy_gate<impl_ket::ControlAndTargetIndicesIList>(const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cz_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CZ");
    check_qubit_range_(target_index, "target qubit", "CZ");
    elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CZ, control_index, target_index));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_cz_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cz_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cz_gate<impl_ket::ControlAndTargetIndicesVector>(const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cz_gate<impl_ket::ControlAndTargetIndicesIList>(const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_csx_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CSX");
    check_qubit_range_(target_index, "target qubit", "CSX");
    elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CSX, control_index, target_index));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_csx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_csx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_csx_gate<impl_ket::ControlAndTargetIndicesVector>(const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_csx_gate<impl_ket::ControlAndTargetIndicesIList>(const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_crx_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRX");
    check_qubit_range_(target_index, "target qubit", "CRX");
    elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRX, control_index, target_index, angle));
}

template <impl_ket::ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_crx_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_crx_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_crx_gate<impl_ket::ControlAndTargetIndicesAndAnglesVector>(const impl_ket::ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_crx_gate<impl_ket::ControlAndTargetIndicesAndAnglesIList>(const impl_ket::ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_cry_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRY");
    check_qubit_range_(target_index, "target qubit", "CRY");
    elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRY, control_index, target_index, angle));
}

template <impl_ket::ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_cry_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_cry_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_cry_gate<impl_ket::ControlAndTargetIndicesAndAnglesVector>(const impl_ket::ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_cry_gate<impl_ket::ControlAndTargetIndicesAndAnglesIList>(const impl_ket::ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_crz_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRZ");
    check_qubit_range_(target_index, "target qubit", "CRZ");
    elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRZ, control_index, target_index, angle));
}

template <impl_ket::ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_crz_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_crz_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_crz_gate<impl_ket::ControlAndTargetIndicesAndAnglesVector>(const impl_ket::ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_crz_gate<impl_ket::ControlAndTargetIndicesAndAnglesIList>(const impl_ket::ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_cp_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CP");
    check_qubit_range_(target_index, "target qubit", "CP");
    elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CP, control_index, target_index, angle));
}

template <impl_ket::ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_cp_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_cp_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_cp_gate<impl_ket::ControlAndTargetIndicesAndAnglesVector>(const impl_ket::ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_cp_gate<impl_ket::ControlAndTargetIndicesAndAnglesIList>(const impl_ket::ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_u_gate(const Matrix2X2& gate, std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "U");
    elements_.emplace_back(impl_ket::create_u_gate(target_index, ket::internal::ClonePtr<Matrix2X2> {gate}));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_u_gate(const Matrix2X2& gate, const Container& indices)
{
    for (auto target_index : indices) {
        add_u_gate(gate, target_index);
    }
}

template void QuantumCircuit::add_u_gate<impl_ket::QubitIndicesVector>(const Matrix2X2& gate, const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_u_gate<impl_ket::QubitIndicesIList>(const Matrix2X2& gate, const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CU");
    check_qubit_range_(target_index, "target qubit", "CU");

    elements_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, ket::internal::ClonePtr<Matrix2X2> {gate}));
}

template <impl_ket::ControlAndTargetIndices Container>
void QuantumCircuit::add_cu_gate(const Matrix2X2& gate, const Container& pairs)
{
    for (auto pair : pairs) {
        add_cu_gate(gate, pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cu_gate<impl_ket::ControlAndTargetIndicesVector>(const Matrix2X2& gate, const impl_ket::ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cu_gate<impl_ket::ControlAndTargetIndicesIList>(const Matrix2X2& gate, const impl_ket::ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_m_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "M");
    check_bit_range_(target_index);
    elements_.emplace_back(impl_ket::create_m_gate(target_index, target_index));
}

template <impl_ket::QubitIndices Container>
void QuantumCircuit::add_m_gate(const Container& indices)
{
    for (auto index : indices) {
        add_m_gate(index);
    }
}
template void QuantumCircuit::add_m_gate<impl_ket::QubitIndicesVector>(const impl_ket::QubitIndicesVector& indices);
template void QuantumCircuit::add_m_gate<impl_ket::QubitIndicesIList>(const impl_ket::QubitIndicesIList& indices);

void QuantumCircuit::add_m_gate(std::size_t target_index, std::size_t bit_index)
{
    check_qubit_range_(target_index, "qubit", "M");
    check_bit_range_(bit_index);
    elements_.emplace_back(impl_ket::create_m_gate(target_index, bit_index));
}

template <impl_ket::QubitAndBitIndices Container>
void QuantumCircuit::add_m_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_m_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_m_gate<impl_ket::QubitAndBitIndicesVector>(const impl_ket::QubitAndBitIndicesVector& indices);
template void QuantumCircuit::add_m_gate<impl_ket::QubitAndBitIndicesIList>(const impl_ket::QubitAndBitIndicesIList& indices);

void QuantumCircuit::add_if_statement(ControlFlowPredicate predicate, QuantumCircuit circuit)
{
    for (auto bit_index : predicate.bit_indices_to_check()) {
        check_bit_range_(bit_index);
    }

    auto cfi = ClassicalIfStatement {
        std::move(predicate),
        std::make_unique<QuantumCircuit>(std::move(circuit))
    };

    elements_.emplace_back(std::move(cfi));
}

void QuantumCircuit::add_if_statement(
    std::size_t bit_index,
    QuantumCircuit subcircuit
)
{
    auto predicate = ControlFlowPredicate {{bit_index}, {1}, ControlFlowBooleanKind::IF};
    add_if_statement(std::move(predicate), std::move(subcircuit));
}

void QuantumCircuit::add_if_not_statement(
    std::size_t bit_index,
    QuantumCircuit subcircuit
)
{
    auto predicate = ControlFlowPredicate {{bit_index}, {0}, ControlFlowBooleanKind::IF};
    add_if_statement(std::move(predicate), std::move(subcircuit));
}

void QuantumCircuit::add_if_else_statement(
    ControlFlowPredicate predicate,
    QuantumCircuit if_subcircuit,
    QuantumCircuit else_subcircuit
)
{
    for (auto bit_index : predicate.bit_indices_to_check()) {
        check_bit_range_(bit_index);
    }

    auto cfi = ClassicalIfElseStatement {
        std::move(predicate),
        std::make_unique<QuantumCircuit>(std::move(if_subcircuit)),
        std::make_unique<QuantumCircuit>(std::move(else_subcircuit))
    };

    elements_.emplace_back(std::move(cfi));
}

void QuantumCircuit::add_if_else_statement(
    std::size_t bit_index,
    QuantumCircuit if_subcircuit,
    QuantumCircuit else_subcircuit
)
{
    auto predicate = ControlFlowPredicate {{bit_index}, {1}, ControlFlowBooleanKind::IF};
    add_if_else_statement(std::move(predicate), std::move(if_subcircuit), std::move(else_subcircuit));
}

void QuantumCircuit::add_if_not_else_statement(
    std::size_t bit_index,
    QuantumCircuit if_subcircuit,
    QuantumCircuit else_subcircuit
)
{
    auto predicate = ControlFlowPredicate {{bit_index}, {0}, ControlFlowBooleanKind::IF};
    add_if_else_statement(std::move(predicate), std::move(if_subcircuit), std::move(else_subcircuit));
}

void QuantumCircuit::add_classical_register_circuit_logger()
{
    elements_.emplace_back(ClassicalRegisterCircuitLogger {});
}

void QuantumCircuit::add_statevector_circuit_logger()
{
    elements_.emplace_back(StatevectorCircuitLogger {});
}

void QuantumCircuit::add_circuit_logger(CircuitLogger circuit_logger)
{
    elements_.emplace_back(std::move(circuit_logger));
}

void QuantumCircuit::check_qubit_range_(std::size_t target_index, std::string_view qubit_name, std::string_view gate_name) const
{
    if (target_index >= n_qubits_) {
        auto err_msg = std::stringstream {};

        err_msg << "The index for the " << qubit_name;
        err_msg << " at which the '" << gate_name << "' gate is applied, is out of bounds.\n";
        err_msg << "n_qubits             = " << n_qubits_ << '\n';
        err_msg << "provided qubit index = " << target_index << '\n';

        throw std::runtime_error {err_msg.str()};
    }
}

void QuantumCircuit::check_bit_range_(std::size_t bit_index) const
{
    if (bit_index >= n_bits_) {
        auto err_msg = std::stringstream {};

        err_msg << "The bit index at which the 'M' gate is applied, is out of bounds.\n";
        err_msg << "n_bits             = " << n_bits_ << '\n';
        err_msg << "provided bit index = " << bit_index << '\n';

        throw std::runtime_error {err_msg.str()};
    }
}

}  // namespace ket

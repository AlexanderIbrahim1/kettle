#include <cmath>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "kettle/circuit/control_flow.hpp"
#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/circuit_loggers/classical_register_circuit_logger.hpp"
#include "kettle/circuit_loggers/statevector_circuit_logger.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/parameter/parameter.hpp"
#include "kettle/parameter/parameter_expression.hpp"

#include "kettle/circuit/circuit.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"


namespace
{

auto default_parameter_name_(std::size_t param_number) -> std::string
{
    auto output = std::stringstream {};
    output << "theta" << param_number;

    return output.str();
}

}  // namespace


namespace ket
{

namespace create = ket::internal::create;

void QuantumCircuit::set_parameter_value(const ket::param::ParameterID& id, double angle)
{
    if (!parameter_values_.contains(id)) {
        throw std::out_of_range {"ERROR: no parameter found with the provided id.\n"};
    }

    parameter_values_[id] = angle;
}

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
    elements_.emplace_back(create::create_one_target_gate(Gate::H, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_h_gate(const Container& indices)
{
    for (auto index : indices) {
        add_h_gate(index);
    }
}
template void QuantumCircuit::add_h_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_h_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_x_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "X");
    elements_.emplace_back(create::create_one_target_gate(Gate::X, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_x_gate(const Container& indices)
{
    for (auto index : indices) {
        add_x_gate(index);
    }
}
template void QuantumCircuit::add_x_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_x_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_y_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "Y");
    elements_.emplace_back(create::create_one_target_gate(Gate::Y, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_y_gate(const Container& indices)
{
    for (auto index : indices) {
        add_y_gate(index);
    }
}
template void QuantumCircuit::add_y_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_y_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_z_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "Z");
    elements_.emplace_back(create::create_one_target_gate(Gate::Z, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_z_gate(const Container& indices)
{
    for (auto index : indices) {
        add_z_gate(index);
    }
}
template void QuantumCircuit::add_z_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_z_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_sx_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "SX");
    elements_.emplace_back(create::create_one_target_gate(Gate::SX, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_sx_gate(const Container& indices)
{
    for (auto index : indices) {
        add_sx_gate(index);
    }
}
template void QuantumCircuit::add_sx_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_sx_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_rx_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RX");
    elements_.emplace_back(create::create_one_target_one_angle_gate(Gate::RX, target_index, angle));
}

auto QuantumCircuit::add_rx_gate(
    std::size_t target_index,
    double initial_angle,
    [[maybe_unused]] ket::param::parameterized key
) -> ket::param::ParameterID
{
    check_qubit_range_(target_index, "qubit", "RX");

    auto parameter = ket::param::Parameter {default_parameter_name_(parameter_count_)};
    auto id = parameter.id();

    parameter_values_[parameter.id()] = initial_angle;
    ++parameter_count_;

    auto expression = ket::param::ParameterExpression {std::move(parameter)};

    elements_.emplace_back(create::create_one_target_one_parameter_gate(Gate::RX, target_index, expression));

    return id;
}

template <QubitIndicesAndAngles Container>
void QuantumCircuit::add_rx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_rx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_rx_gate<QubitIndicesAndAnglesVector>(const QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_rx_gate<QubitIndicesAndAnglesIList>(const QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_ry_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RY");
    elements_.emplace_back(create::create_one_target_one_angle_gate(Gate::RY, target_index, angle));
}

template <QubitIndicesAndAngles Container>
void QuantumCircuit::add_ry_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_ry_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_ry_gate<QubitIndicesAndAnglesVector>(const QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_ry_gate<QubitIndicesAndAnglesIList>(const QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_rz_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "RZ");
    elements_.emplace_back(create::create_one_target_one_angle_gate(Gate::RZ, target_index, angle));
}

template <QubitIndicesAndAngles Container>
void QuantumCircuit::add_rz_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_rz_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_rz_gate<QubitIndicesAndAnglesVector>(const QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_rz_gate<QubitIndicesAndAnglesIList>(const QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_p_gate(std::size_t target_index, double angle)
{
    check_qubit_range_(target_index, "qubit", "P");
    elements_.emplace_back(create::create_one_target_one_angle_gate(Gate::P, target_index, angle));
}

template <QubitIndicesAndAngles Container>
void QuantumCircuit::add_p_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_p_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_p_gate<QubitIndicesAndAnglesVector>(const QubitIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_p_gate<QubitIndicesAndAnglesIList>(const QubitIndicesAndAnglesIList& indices);

void QuantumCircuit::add_ch_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CH");
    check_qubit_range_(target_index, "target qubit", "CH");
    elements_.emplace_back(create::create_one_control_one_target_gate(Gate::CH, control_index, target_index));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_ch_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_ch_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_ch_gate<ControlAndTargetIndicesVector>(const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_ch_gate<ControlAndTargetIndicesIList>(const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cx_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CX");
    check_qubit_range_(target_index, "target qubit", "CX");
    elements_.emplace_back(create::create_one_control_one_target_gate(Gate::CX, control_index, target_index));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_cx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cx_gate<ControlAndTargetIndicesVector>(const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cx_gate<ControlAndTargetIndicesIList>(const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cy_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CY");
    check_qubit_range_(target_index, "target qubit", "CY");
    elements_.emplace_back(create::create_one_control_one_target_gate(Gate::CY, control_index, target_index));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_cy_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cy_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cy_gate<ControlAndTargetIndicesVector>(const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cy_gate<ControlAndTargetIndicesIList>(const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_cz_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CZ");
    check_qubit_range_(target_index, "target qubit", "CZ");
    elements_.emplace_back(create::create_one_control_one_target_gate(Gate::CZ, control_index, target_index));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_cz_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_cz_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cz_gate<ControlAndTargetIndicesVector>(const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cz_gate<ControlAndTargetIndicesIList>(const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_csx_gate(std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CSX");
    check_qubit_range_(target_index, "target qubit", "CSX");
    elements_.emplace_back(create::create_one_control_one_target_gate(Gate::CSX, control_index, target_index));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_csx_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_csx_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_csx_gate<ControlAndTargetIndicesVector>(const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_csx_gate<ControlAndTargetIndicesIList>(const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_crx_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRX");
    check_qubit_range_(target_index, "target qubit", "CRX");
    elements_.emplace_back(create::create_one_control_one_target_one_angle_gate(Gate::CRX, control_index, target_index, angle));
}

template <ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_crx_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_crx_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_crx_gate<ControlAndTargetIndicesAndAnglesVector>(const ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_crx_gate<ControlAndTargetIndicesAndAnglesIList>(const ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_cry_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRY");
    check_qubit_range_(target_index, "target qubit", "CRY");
    elements_.emplace_back(create::create_one_control_one_target_one_angle_gate(Gate::CRY, control_index, target_index, angle));
}

template <ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_cry_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_cry_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_cry_gate<ControlAndTargetIndicesAndAnglesVector>(const ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_cry_gate<ControlAndTargetIndicesAndAnglesIList>(const ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_crz_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CRZ");
    check_qubit_range_(target_index, "target qubit", "CRZ");
    elements_.emplace_back(create::create_one_control_one_target_one_angle_gate(Gate::CRZ, control_index, target_index, angle));
}

template <ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_crz_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_crz_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_crz_gate<ControlAndTargetIndicesAndAnglesVector>(const ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_crz_gate<ControlAndTargetIndicesAndAnglesIList>(const ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_cp_gate(std::size_t control_index, std::size_t target_index, double angle)
{
    check_qubit_range_(control_index, "control qubit", "CP");
    check_qubit_range_(target_index, "target qubit", "CP");
    elements_.emplace_back(create::create_one_control_one_target_one_angle_gate(Gate::CP, control_index, target_index, angle));
}

template <ControlAndTargetIndicesAndAngles Container>
void QuantumCircuit::add_cp_gate(const Container& tuples)
{
    for (auto tuple : tuples) {
        add_cp_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}
template void QuantumCircuit::add_cp_gate<ControlAndTargetIndicesAndAnglesVector>(const ControlAndTargetIndicesAndAnglesVector& indices);
template void QuantumCircuit::add_cp_gate<ControlAndTargetIndicesAndAnglesIList>(const ControlAndTargetIndicesAndAnglesIList& indices);

void QuantumCircuit::add_u_gate(const Matrix2X2& gate, std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "U");
    elements_.emplace_back(create::create_u_gate(target_index, ket::ClonePtr<Matrix2X2> {gate}));
}

template <QubitIndices Container>
void QuantumCircuit::add_u_gate(const Matrix2X2& gate, const Container& indices)
{
    for (auto target_index : indices) {
        add_u_gate(gate, target_index);
    }
}

template void QuantumCircuit::add_u_gate<QubitIndicesVector>(const Matrix2X2& gate, const QubitIndicesVector& indices);
template void QuantumCircuit::add_u_gate<QubitIndicesIList>(const Matrix2X2& gate, const QubitIndicesIList& indices);

void QuantumCircuit::add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index)
{
    check_qubit_range_(control_index, "control qubit", "CU");
    check_qubit_range_(target_index, "target qubit", "CU");

    elements_.emplace_back(create::create_cu_gate(control_index, target_index, ket::ClonePtr<Matrix2X2> {gate}));
}

template <ControlAndTargetIndices Container>
void QuantumCircuit::add_cu_gate(const Matrix2X2& gate, const Container& pairs)
{
    for (auto pair : pairs) {
        add_cu_gate(gate, pair.first, pair.second);
    }
}
template void QuantumCircuit::add_cu_gate<ControlAndTargetIndicesVector>(const Matrix2X2& gate, const ControlAndTargetIndicesVector& indices);
template void QuantumCircuit::add_cu_gate<ControlAndTargetIndicesIList>(const Matrix2X2& gate, const ControlAndTargetIndicesIList& indices);

void QuantumCircuit::add_m_gate(std::size_t target_index)
{
    check_qubit_range_(target_index, "qubit", "M");
    check_bit_range_(target_index);
    elements_.emplace_back(create::create_m_gate(target_index, target_index));
}

template <QubitIndices Container>
void QuantumCircuit::add_m_gate(const Container& indices)
{
    for (auto index : indices) {
        add_m_gate(index);
    }
}
template void QuantumCircuit::add_m_gate<QubitIndicesVector>(const QubitIndicesVector& indices);
template void QuantumCircuit::add_m_gate<QubitIndicesIList>(const QubitIndicesIList& indices);

void QuantumCircuit::add_m_gate(std::size_t target_index, std::size_t bit_index)
{
    check_qubit_range_(target_index, "qubit", "M");
    check_bit_range_(bit_index);
    elements_.emplace_back(create::create_m_gate(target_index, bit_index));
}

template <QubitAndBitIndices Container>
void QuantumCircuit::add_m_gate(const Container& pairs)
{
    for (auto pair : pairs) {
        add_m_gate(pair.first, pair.second);
    }
}
template void QuantumCircuit::add_m_gate<QubitAndBitIndicesVector>(const QubitAndBitIndicesVector& indices);
template void QuantumCircuit::add_m_gate<QubitAndBitIndicesIList>(const QubitAndBitIndicesIList& indices);

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

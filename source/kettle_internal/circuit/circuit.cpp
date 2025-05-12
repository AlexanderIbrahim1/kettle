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
#include "kettle_internal/gates/primitive_gate_map.hpp"

// NOTE:
// - there is the potential for a lot of code reduction here
//   - for example, all the `add_h_gate()` and `add_x_gate()` member functions only
//     differ by the function name and the `Gate::???` value used in the function
// - I've tried to find a nicer modern C++ way to reduce the amount of code
// - but it looks like macros would reduce the amount of code the most
//
// - however, I'm very hesitant to use macros
//   - I'll tolerate the code duplication for now

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
    add_one_target_gate_(target_index, Gate::H);
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
    add_one_target_gate_(target_index, Gate::X);
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
    add_one_target_gate_(target_index, Gate::Y);
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
    add_one_target_gate_(target_index, Gate::Z);
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
    add_one_target_gate_(target_index, Gate::SX);
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
    add_one_target_one_angle_gate_(target_index, angle, Gate::RX);
}

auto QuantumCircuit::add_rx_gate(
    std::size_t target_index,
    double initial_angle,
    [[maybe_unused]] ket::param::parameterized key
) -> ket::param::ParameterID
{
    return add_one_target_one_parameter_gate_new_(target_index, initial_angle, Gate::RX, key);
}

// TODO: add unit test for this
void QuantumCircuit::add_rx_gate(std::size_t target_index, const ket::param::ParameterID& id)
{
    add_one_target_one_parameter_gate_existing_(target_index, Gate::RX, id);
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
    add_one_target_one_angle_gate_(target_index, angle, Gate::RY);
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
    add_one_target_one_angle_gate_(target_index, angle, Gate::RZ);
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
    add_one_target_one_angle_gate_(target_index, angle, Gate::P);
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
    add_one_control_one_target_gate_(control_index, target_index, Gate::CH);
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
    add_one_control_one_target_gate_(control_index, target_index, Gate::CX);
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
    add_one_control_one_target_gate_(control_index, target_index, Gate::CY);
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
    add_one_control_one_target_gate_(control_index, target_index, Gate::CZ);
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
    add_one_control_one_target_gate_(control_index, target_index, Gate::CSX);
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
    add_one_control_one_target_one_angle_gate_(control_index, target_index, angle, Gate::CRX);
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
    add_one_control_one_target_one_angle_gate_(control_index, target_index, angle, Gate::CRY);
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
    add_one_control_one_target_one_angle_gate_(control_index, target_index, angle, Gate::CRZ);
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
    add_one_control_one_target_one_angle_gate_(control_index, target_index, angle, Gate::CP);
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

void QuantumCircuit::add_if_statement(
    ControlFlowPredicate predicate,
    QuantumCircuit circuit,
    double tolerance
)
{
    for (auto bit_index : predicate.bit_indices_to_check()) {
        check_bit_range_(bit_index);
    }

    merge_subcircuit_parameters_(circuit, tolerance);

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

void QuantumCircuit::add_one_target_gate_(
    std::size_t target_index,
    ket::Gate gate
)
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(target_index, "qubit", gate_name);
    elements_.emplace_back(create::create_one_target_gate(gate, target_index));
}

void QuantumCircuit::add_one_target_one_angle_gate_(
    std::size_t target_index,
    double angle,
    ket::Gate gate
)
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(target_index, "qubit", gate_name);
    elements_.emplace_back(create::create_one_target_one_angle_gate(gate, target_index, angle));
}

void QuantumCircuit::add_one_control_one_target_gate_(
    std::size_t control_index,
    std::size_t target_index,
    ket::Gate gate
)
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(control_index, "control qubit", gate_name);
    check_qubit_range_(target_index, "target qubit", gate_name);
    elements_.emplace_back(create::create_one_control_one_target_gate(gate, control_index, target_index));
}

void QuantumCircuit::add_one_control_one_target_one_angle_gate_(
    std::size_t control_index,
    std::size_t target_index,
    double angle,
    ket::Gate gate
)
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(control_index, "control qubit", gate_name);
    check_qubit_range_(target_index, "target qubit", gate_name);
    elements_.emplace_back(create::create_one_control_one_target_one_angle_gate(gate, control_index, target_index, angle));
}

auto QuantumCircuit::add_one_target_one_parameter_gate_new_(
    std::size_t target_index,
    double initial_angle,
    Gate gate,
    [[maybe_unused]] ket::param::parameterized key
) -> ket::param::ParameterID
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(target_index, "qubit", gate_name);

    auto parameter = ket::param::Parameter {default_parameter_name_(parameter_count_)};
    auto id = parameter.id();

    ++parameter_count_;
    parameter_values_[id] = initial_angle;
    parameter_data_[id] = ParameterData {.count=1, .name=parameter.name()};

    auto expression = ket::param::ParameterExpression {std::move(parameter)};

    elements_.emplace_back(create::create_one_target_one_parameter_gate(gate, target_index, expression));

    return id;
}

void QuantumCircuit::add_one_target_one_parameter_gate_existing_(
    std::size_t target_index,
    Gate gate,
    const ket::param::ParameterID& id
)
{
    const auto gate_name = ket::internal::PRIMITIVE_GATES_TO_STRING.at(gate);
    check_qubit_range_(target_index, "qubit", gate_name);

    if (!parameter_values_.contains(id)) {
        throw std::out_of_range {"ERROR: no parameter found with the provided id.\n"};
    }

    auto& data = parameter_data_.at(id);

    auto parameter = ket::param::Parameter {data.name, id};
    auto expression = ket::param::ParameterExpression {std::move(parameter)};
    ++data.count;

    elements_.emplace_back(create::create_one_target_one_parameter_gate(gate, target_index, expression));
}

void QuantumCircuit::merge_subcircuit_parameters_(
    const QuantumCircuit& subcircuit,
    double tolerance
)
{
    // TODO: currently if the names of the parameters do not match, then no exception is thrown,
    // and the name of the parent circuit takes precedence;
    // 
    // maybe this behaviour should change in the future?
    for (const auto& sub_id_angle_pair : subcircuit.parameter_values_)
    {
        [[maybe_unused]]
        auto [insert_it, is_inserted] = parameter_values_.insert(sub_id_angle_pair);

        if (!is_inserted) {
            if (std::fabs(insert_it->second - sub_id_angle_pair.second) > tolerance) {
                throw std::runtime_error {
                    "ERROR: found same parameter in if-statement subcircuit with different value!\n"
                };
            }

            const auto& id = sub_id_angle_pair.first;
            const auto count_in_sub = subcircuit.parameter_data_.at(id).count;
            parameter_data_.at(id).count += count_in_sub;
        }
        else {
            const auto& id = sub_id_angle_pair.first;
            if (parameter_data_.contains(id)) {
                throw std::runtime_error {
                    "ERROR: found same parameter in if-statement subcircuit with different value!\n"
                };
            }
            parameter_data_[id] = subcircuit.parameter_data_.at(id);
        }
    }
}

}  // namespace ket

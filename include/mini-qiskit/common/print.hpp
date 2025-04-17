#pragma once

#include <complex>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>

#include "mini-qiskit/gates/primitive_gate.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/circuit/circuit.hpp"
#include "mini-qiskit/state/state.hpp"


namespace impl_mqis
{

namespace formatting
{

static constexpr auto DEFAULT_INTEGER_WIDTH = std::size_t {2};
static constexpr auto DEFAULT_ANGLE_PRECISION = std::size_t {6};
static constexpr auto DEFAULT_ANGLE_WIDTH = std::size_t {11};

static constexpr auto DEFAULT_CONTROL = std::string (DEFAULT_INTEGER_WIDTH, ' ');
static constexpr auto DEFAULT_TARGET = std::string  (DEFAULT_INTEGER_WIDTH, ' ');
static constexpr auto DEFAULT_ANGLE = std::string   (DEFAULT_ANGLE_WIDTH  , ' ');

}

static const auto GATE_TO_STRING = std::unordered_map<mqis::Gate, std::string> {
    {mqis::Gate::H, "H"},
    {mqis::Gate::X, "X"},
    {mqis::Gate::Y, "Y"},
    {mqis::Gate::Z, "Z"},
    {mqis::Gate::RX, "RX"},
    {mqis::Gate::RY, "RY"},
    {mqis::Gate::RZ, "RZ"},
    {mqis::Gate::P, "P"},
    {mqis::Gate::CX, "CX"},
    {mqis::Gate::CY, "CY"},
    {mqis::Gate::CZ, "CZ"},
    {mqis::Gate::CRX, "CRX"},
    {mqis::Gate::CRY, "CRY"},
    {mqis::Gate::CRZ, "CRZ"},
    {mqis::Gate::CP, "CP"},
    {mqis::Gate::U, "U"},
    {mqis::Gate::CU, "CU"},
    {mqis::Gate::M, "M"},
    {mqis::Gate::CONTROL, "CONTROL"}
};

auto left_padded_integer_(std::size_t x, std::size_t minimum_width = formatting::DEFAULT_INTEGER_WIDTH) -> std::string
{
    const auto number_as_string = [&]() {
        auto number = std::stringstream {};
        number << x;
        return number.str();
    }();

    if (minimum_width <= number_as_string.size()) {
        return number_as_string;
    }

    const auto padding = minimum_width - number_as_string.size();
    return std::string (padding, ' ') + number_as_string;
}

auto left_sign_padding_(double x) -> char
{
    if (x < 0) {
        return '-';
    } else {
        return ' ';
    }
}

auto left_padded_double_(double x, std::size_t precision = formatting::DEFAULT_ANGLE_PRECISION, std::size_t minimum_width = formatting::DEFAULT_ANGLE_WIDTH) -> std::string
{
    const auto number_as_string = [&]() {
        auto number = std::stringstream {};
        number << std::fixed;
        number << std::setprecision(static_cast<int>(precision));
        number << left_sign_padding_(x);
        number << std::abs(x);

        return number.str();
    }();

    if (minimum_width <= number_as_string.size()) {
        return number_as_string;
    }

    const auto padding = minimum_width - number_as_string.size();
    return std::string (padding, ' ') + number_as_string;
}

auto format_gate_control_target_angle_(const mqis::GateInfo& info)
    -> std::tuple<std::string, std::optional<std::size_t>>
{
    namespace gid = impl_mqis::gate_id;
    using G = mqis::Gate;

    auto output = std::stringstream {};
    output << GATE_TO_STRING.at(info.gate) << '\n';
    output << "(control, target, angle) = (";

    auto control = formatting::DEFAULT_CONTROL;
    auto target = formatting::DEFAULT_TARGET;
    auto angle = formatting::DEFAULT_ANGLE;
    auto i_matrix = std::optional<std::size_t> {std::nullopt};

    if (gid::is_one_target_transform_gate(info.gate)) {
        const auto temp_target = unpack_one_target_gate(info);
        target = left_padded_integer_(temp_target);
    }
    else if (gid::is_one_target_one_angle_transform_gate(info.gate))
    {
        const auto [temp_target, temp_angle] = unpack_one_target_one_angle_gate(info);
        target = left_padded_integer_(temp_target);
        angle = left_padded_double_(temp_angle);
    }
    else if (gid::is_one_control_one_target_transform_gate(info.gate)) {
        const auto [temp_control, temp_target] = unpack_one_control_one_target_gate(info);
        control = left_padded_integer_(temp_control);
        target = left_padded_integer_(temp_target);
    }
    else if (gid::is_one_control_one_target_one_angle_transform_gate(info.gate)) {
        const auto [temp_control, temp_target, temp_angle] = unpack_one_control_one_target_one_angle_gate(info);
        control = left_padded_integer_(temp_control);
        target = left_padded_integer_(temp_target);
        angle = left_padded_double_(temp_angle);
    }
    else if (info.gate == G::U) {
        const auto [temp_target, temp_matrix] = unpack_u_gate(info);
        target = left_padded_integer_(temp_target);
        i_matrix = temp_matrix;
    }
    else if (info.gate == G::CU) {
        const auto [temp_control, temp_target, temp_matrix] = unpack_cu_gate(info);
        control = left_padded_integer_(temp_control);
        target = left_padded_integer_(temp_target);
        i_matrix = temp_matrix;
    }
    else if (info.gate == G::M) {
        [[maybe_unused]]
        const auto [temp_target, ignore] = unpack_m_gate(info);
        target = left_padded_integer_(temp_target);
    }
    // TODO: make this print statement more meaningful; it doesn't make sense right now
    else if (info.gate == G::CONTROL) {
        const auto cfi_index = control::unpack_control_flow_index(info);
        const auto cfi_kind = control::unpack_control_flow_kind(info);
        target = left_padded_integer_(cfi_index);
        control= left_padded_integer_(cfi_kind);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid gate found when formatting gate print output.\n"};
    }

    output << control << ", ";
    output << target << ", ";
    output << angle << ")";

    return {output.str(), i_matrix};
}

auto format_complex_(const std::complex<double>& value) -> std::string
{
    auto output = std::stringstream {};
    output << "(";
    output << left_padded_double_(value.real()) << ", " << left_padded_double_(value.imag());
    output << ")";

    return output.str();
}

auto format_matrix_(const mqis::Matrix2X2& matrix) -> std::string
{
    const auto elem00 = format_complex_(matrix.elem00);
    const auto elem01 = format_complex_(matrix.elem01);
    const auto elem10 = format_complex_(matrix.elem10);
    const auto elem11 = format_complex_(matrix.elem11);

    auto output = std::stringstream {};
    output << "[\n";
    output << "    " << elem00 << "   " << elem01 << '\n';
    output << "    " << elem10 << "   " << elem11 << '\n';
    output << "]";

    return output.str();
}

auto ae_err_msg_diff_number_of_qubits_(std::size_t n_left_qubits, std::size_t n_right_qubits)
-> std::string
{
    auto err_msg = std::stringstream {};
    err_msg << "FALSE: ALMOST_EQ_WITH_PRINT()\n";
    err_msg << "REASON: different number of qubits in the states\n";
    err_msg << "left state: " << n_left_qubits << '\n';
    err_msg << "right state: " << n_right_qubits << '\n';

    return err_msg.str();
}

void print_state_(const mqis::QuantumState& state)
{
    // for the time being, fix this as being little-endian
    const auto endian = mqis::QuantumStateEndian::LITTLE;

    for (std::size_t i {0}; i < state.n_states(); ++i) {
        const auto bitstring = mqis::state_index_to_bitstring(i, state.n_qubits(), endian);
        std::cout << bitstring << " : (" << state[i].real() << ", " << state[i].imag() << ")\n";
    }
}

auto ae_err_msg_diff_states_(
    const mqis::QuantumState& left,
    const mqis::QuantumState& right
) -> std::string
{
    auto err_msg = std::stringstream {};
    err_msg << "FALSE: ALMOST_EQ_WITH_PRINT()\n";
    err_msg << "REASON: different states\n";

    err_msg << "LEFT STATE:\n";
    print_state_(left);

    err_msg << "RIGHT STATE:\n";
    print_state_(right);

    return err_msg.str();
}

}  // namespace impl_mqis


namespace mqis
{

void print_circuit(const QuantumCircuit& circuit)
{
    for (auto gate_info : circuit) {
        std::cout << "-------------------------------------------------------\n";
        const auto [formatted_gate, i_matrix] = impl_mqis::format_gate_control_target_angle_(gate_info);

        std::cout << formatted_gate << '\n';

        if (i_matrix) {
            const auto& matrix = circuit.unitary_gate(*i_matrix);
            std::cout << impl_mqis::format_matrix_(matrix) << '\n';
        }
    }
}

void print_state(const QuantumState& state)
{
    impl_mqis::print_state_(state);
}

enum PrintAlmostEq
{
    PRINT,
    NOPRINT
};

constexpr auto almost_eq_with_print(
    const QuantumState& left,
    const QuantumState& right,
    PrintAlmostEq print_state = PrintAlmostEq::PRINT,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool
{
    using PAE = PrintAlmostEq;

    if (left.n_qubits() != right.n_qubits()) {
        if (print_state == PAE::PRINT) {
            std::cout << impl_mqis::ae_err_msg_diff_number_of_qubits_(left.n_qubits(), right.n_qubits());
        }
        return false;
    }

    for (std::size_t i {0}; i < left.n_states(); ++i) {
        if (!almost_eq(left[i], right[i], tolerance_sq)) {
            if (print_state == PAE::PRINT) {
                std::cout << impl_mqis::ae_err_msg_diff_states_(left, right);
            }
            return false;
        }
    }

    return true;
}


}  // namespace mqis

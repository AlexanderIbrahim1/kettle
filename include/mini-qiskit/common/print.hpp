#pragma once

#include <complex>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/circuit.hpp"
#include "mini-qiskit/state.hpp"


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

auto gate_to_string_(mqis::Gate gate) -> std::string
{
    using G = mqis::Gate;
    // returns the gate as a string, since we still don't have reflection
    switch (gate)
    {
        case G::X : {
            return "X";
        }
        case G::RX : {
            return "RX";
        }
        case G::H : {
            return "H";
        }
        case G::CX : {
            return "CX";
        }
        case G::CRX : {
            return "CRX";
        }
        case G::CP : {
            return "CP";
        }
        case G::U : {
            return "U";
        }
        case G::CU : {
            return "CU";
        }
        case G::M : {
            return "M";
        }
        default : {
            throw std::runtime_error {"UNREACHABLE: invalid Gate enum class passed to function."};
        }
    }
}

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
    using G = mqis::Gate;

    auto output = std::stringstream {};
    output << gate_to_string_(info.gate) << '\n';
    output << "(control, target, angle) = (";

    auto control = formatting::DEFAULT_CONTROL;
    auto target = formatting::DEFAULT_TARGET;
    auto angle = formatting::DEFAULT_ANGLE;
    auto i_matrix = std::optional<std::size_t> {std::nullopt};

    switch (info.gate)
    {
        case G::X : {
            const auto temp_target = unpack_x_gate(info);
            target = left_padded_integer_(temp_target);
            break;
        }
        case G::RX : {
            const auto [temp_angle, temp_target] = unpack_rx_gate(info);
            target = left_padded_integer_(temp_target);
            angle = left_padded_double_(temp_angle);
            break;
        }
        case G::H : {
            const auto temp_target = unpack_h_gate(info);
            target = left_padded_integer_(temp_target);
            break;
        }
        case G::CX : {
            const auto [temp_control, temp_target] = unpack_cx_gate(info);
            control = left_padded_integer_(temp_control);
            target = left_padded_integer_(temp_target);
            break;
        }
        case G::CRX : {
            const auto [temp_control, temp_target, temp_angle] = unpack_crx_gate(info);
            control = left_padded_integer_(temp_control);
            target = left_padded_integer_(temp_target);
            angle = left_padded_double_(temp_angle);
            break;
        }
        case G::CP : {
            const auto [temp_control, temp_target, temp_angle] = unpack_cp_gate(info);
            control = left_padded_integer_(temp_control);
            target = left_padded_integer_(temp_target);
            angle = left_padded_double_(temp_angle);
            break;
        }
        case G::U : {
            const auto [temp_target, temp_matrix] = unpack_u_gate(info);
            target = left_padded_integer_(temp_target);
            i_matrix = temp_matrix;
            break;
        }
        case G::CU : {
            const auto [temp_control, temp_target, temp_matrix] = unpack_cu_gate(info);
            control = left_padded_integer_(temp_control);
            target = left_padded_integer_(temp_target);
            i_matrix = temp_matrix;
            break;
        }
        case G::M : {
            [[maybe_unused]]
            const auto [temp_target, ignore] = unpack_m_gate(info);
            target = left_padded_integer_(temp_target);
            break;
        }
        default : {
            throw std::runtime_error {"UNREACHABLE: invalid Gate enum class passed to function."};
        }
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
    for (std::size_t i {0}; i < state.n_states(); ++i) {
        const auto bitstring = mqis::state_as_bitstring(i, state.n_qubits());
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
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_EQ
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

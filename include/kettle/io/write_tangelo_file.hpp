#pragma once

#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/primitive_gate_map.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/io/io_control_flow.hpp"

/*
This script writes the gates of a `QuantumCircuit` instance into a tangelo-like format file

Some examples:
```
H         target : [4]
RX        target : [5]   parameter : 1.5707963267948966
CNOT      target : [4]   control : [2]
RZ        target : [5]   parameter : 12.533816585267923
```
*/

namespace impl_ket
{

inline auto format_double_(double x) -> std::string
{
    auto output = std::format("{: .17f}", x);
    if (output.size() > 16) {
        output = output.substr(0, 16);
    }

    return output;
}

inline auto format_matrix2x2_(const ket::Matrix2X2& mat) -> std::string
{
    auto output = std::stringstream {};
    output << std::format("    [{}, {}]", format_double_(mat.elem00.real()), format_double_(mat.elem00.imag()));
    output << std::format("   [{}, {}]\n", format_double_(mat.elem01.real()), format_double_(mat.elem01.imag()));
    output << std::format("    [{}, {}]", format_double_(mat.elem10.real()), format_double_(mat.elem10.imag()));
    output << std::format("   [{}, {}]\n", format_double_(mat.elem11.real()), format_double_(mat.elem11.imag()));

    return output.str();
}

inline auto format_one_target_gate_(const ket::GateInfo& info) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto target = unpack_single_qubit_gate_index(info);

    return std::format("{:<10}target : [{}]\n", gate_name, target);
}

inline auto format_one_control_one_target_gate_(const ket::GateInfo& info) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto [control, target] = unpack_double_qubit_gate_indices(info);

    return std::format("{:<10}target : [{}]   control : [{}]\n", gate_name, target, control);
}

inline auto format_one_target_one_angle_gate_(const ket::GateInfo& info) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto [target, angle] = unpack_one_target_one_angle_gate(info);

    return std::format("{:<10}target : [{}]   parameter : {:.16f}\n", gate_name, target, angle);
}

inline auto format_one_control_one_target_one_angle_gate_(const ket::GateInfo& info) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto [control, target, angle] = unpack_one_control_one_target_one_angle_gate(info);

    return std::format("{:<10}target : [{}]   control : [{}]   parameter : {:.16f}\n", gate_name, target, control, angle);
}

inline auto format_m_gate_(const ket::GateInfo& info) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto [qubit, bit] = unpack_m_gate(info);

    return std::format("{:<10}target : [{}]   bit : [{}]\n", gate_name, qubit, bit);
}

inline auto format_u_gate_(const ket::GateInfo& info, const ket::Matrix2X2& mat) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto target = unpack_single_qubit_gate_index(info);

    auto output = std::stringstream {};
    output << std::format("{:<10}target : [{}]\n", gate_name, target);
    output << format_matrix2x2_(mat);

    return output.str();
}

inline auto format_cu_gate_(const ket::GateInfo& info, const ket::Matrix2X2& mat) -> std::string
{
    const auto gate_name = PRIMITIVE_GATES_TO_STRING.at(info.gate);
    const auto [control, target] = unpack_double_qubit_gate_indices(info);

    auto output = std::stringstream {};
    output << std::format("{:<10}target : [{}]   control : [{}]\n", gate_name, target, control);
    output << format_matrix2x2_(mat);

    return output.str();
}

}  // namespace impl_ket

namespace ket
{

// NOLINTNEXTLINE(misc-no-recursion, readability-function-cognitive-complexity)
inline void write_tangelo_circuit(
    const ket::QuantumCircuit& circuit,
    std::ostream& stream,
    std::size_t n_leading_whitespace = 0
)
{
    namespace gid = impl_ket::gate_id;
    using G = ket::Gate;

    const auto whitespace = std::string(n_leading_whitespace, ' ');

    for (const auto& circuit_element : circuit) {
        if (circuit_element.is_control_flow()) {
            const auto& control_flow = circuit_element.get_control_flow();

            if (control_flow.is_if_statement()) {
                const auto& stmt = control_flow.get_if_statement();
                const auto if_part = impl_ket::format_classical_if_statement_header_(stmt);
                stream << if_part << '\n';
                write_tangelo_circuit(*stmt.circuit(), stream, 4);
            }
            else if (control_flow.is_if_else_statement()) {
                const auto& stmt = control_flow.get_if_else_statement();
                const auto [if_part, else_part] = impl_ket::format_classical_if_else_statement_header_(stmt);
                stream << if_part << '\n';
                write_tangelo_circuit(*stmt.if_circuit(), stream, 4);
                stream << else_part << '\n';
                write_tangelo_circuit(*stmt.else_circuit(), stream, 4);
            }
            else {
                throw std::runtime_error {"DEV ERROR: invalid control flow statement encountered for write\n"};
            }

        } else {
            const auto& gate_info = circuit_element.get_gate();

            if (gid::is_one_target_transform_gate(gate_info.gate)) {
                stream << whitespace << impl_ket::format_one_target_gate_(gate_info);
            }
            else if (gid::is_one_control_one_target_transform_gate(gate_info.gate)) {
                stream << whitespace << impl_ket::format_one_control_one_target_gate_(gate_info);
            }
            else if (gid::is_one_target_one_angle_transform_gate(gate_info.gate)) {
                stream << whitespace << impl_ket::format_one_target_one_angle_gate_(gate_info);
            }
            else if (gid::is_one_control_one_target_one_angle_transform_gate(gate_info.gate)) {
                stream << whitespace << impl_ket::format_one_control_one_target_one_angle_gate_(gate_info);
            }
            else if (gate_info.gate == G::M) {
                stream << whitespace << impl_ket::format_m_gate_(gate_info);
            }
            else if (gate_info.gate == G::U) {
                const auto& unitary_ptr = impl_ket::unpack_unitary_matrix(gate_info);
                stream << whitespace << impl_ket::format_u_gate_(gate_info, *unitary_ptr);
            }
            else if (gate_info.gate == G::CU) {
                const auto& unitary_ptr = impl_ket::unpack_unitary_matrix(gate_info);
                stream << whitespace << impl_ket::format_cu_gate_(gate_info, *unitary_ptr);
            }
            else {
                throw std::runtime_error {"DEV ERROR: A gate type with no implemented output has been encountered.\n"};
            }
        }
    }
}

inline void write_tangelo_circuit(const QuantumCircuit& circuit, const std::filesystem::path& filepath)
{
    auto outstream = std::ofstream {filepath};

    if (!outstream.is_open()) {
        auto err_msg = std::stringstream {};
        err_msg << "ERROR: unable to open file to write tangelo-style circuit: '" << filepath << "'\n";

        throw std::ios::failure {err_msg.str()};
    }

    write_tangelo_circuit(circuit, outstream);
}

inline void print_tangelo_circuit(const QuantumCircuit& circuit)
{
    write_tangelo_circuit(circuit, std::cout);
}

}  // namespace ket

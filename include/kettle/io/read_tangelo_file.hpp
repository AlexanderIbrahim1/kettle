#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include <kettle/gates/primitive_gate.hpp>
#include <kettle/gates/primitive_gate_map.hpp>
#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/swap.hpp>

/*
This script parses the file of gates produced by the tangelo code.

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

/*
    Certain names of primitive gates do not match between the tangelo codebase and this codebase;
    this function converts tangelo-specific names to names used here.
*/
inline auto tangelo_to_local_name_(const std::string& name) -> std::string
{
    if (name == "CPHASE") {
        return "CP";
    }
    else if (name == "CNOT") {
        return "CX";
    }
    else if (name == "PHASE") {
        return "P";
    }
    else {
        return name;
    }
}

inline void parse_swap_gate_(ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit0;
    std::size_t target_qubit1;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit0; // target qubit 0
    stream >> dummy_ch;     // ','
    stream >> target_qubit1; // target qubit 1
    stream >> dummy_ch;     // ']'

    ket::apply_swap(circuit, target_qubit0, target_qubit1);
}

inline void parse_one_target_gate_(ket::Gate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'

    const auto func = GATE_TO_FUNCTION_1T.at(gate);
    (circuit.*func)(target_qubit);
}

inline void parse_one_control_one_target_gate_(ket::Gate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;
    std::size_t control_qubit;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'control'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> control_qubit; // control qubit
    stream >> dummy_ch;     // ']'

    const auto func = GATE_TO_FUNCTION_1C1T.at(gate);
    (circuit.*func)(control_qubit, target_qubit);
}

inline void parse_one_target_one_angle_gate_(ket::Gate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;
    double angle;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'parameter'
    stream >> dummy_str;    // ':'
    stream >> angle;        // angle

    const auto func = GATE_TO_FUNCTION_1T1A.at(gate);
    (circuit.*func)(target_qubit, angle);
}

inline void parse_one_control_one_target_one_angle_gate_(ket::Gate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;
    std::size_t control_qubit;
    double angle;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'control'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> control_qubit; // control qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'parameter'
    stream >> dummy_str;    // ':'
    stream >> angle;        // angle

    const auto func = GATE_TO_FUNCTION_1C1T1A.at(gate);
    (circuit.*func)(control_qubit, target_qubit, angle);
}

inline void parse_m_gate_(ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t qubit;
    std::size_t bit;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> qubit;        // qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'bit'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> bit;          // bit
    stream >> dummy_ch;     // ']'

    circuit.add_m_gate(qubit, bit);
}

inline auto parse_complex_(std::stringstream& stream) -> std::complex<double>
{
    char dummy_char;
    double real;
    double imag;

    stream >> dummy_char;    // '['
    stream >> real;          // real component
    stream >> dummy_char;    // ','
    stream >> imag;          // imaginary component
    stream >> dummy_char;    // ']'

    return {real, imag};
}

inline auto parse_matrix2x2_(std::istream& stream) -> ket::Matrix2X2
{
    std::string first_line;
    std::string second_line;

    std::getline(stream, first_line);

    auto sstream_first = std::stringstream {first_line};

    std::getline(stream, second_line);
    auto sstream_second = std::stringstream {second_line};

    const auto elem00 = parse_complex_(sstream_first);
    const auto elem01 = parse_complex_(sstream_first);
    const auto elem10 = parse_complex_(sstream_second);
    const auto elem11 = parse_complex_(sstream_second);

    return {elem00, elem01, elem10, elem11};
}

inline void parse_u_gate_(ket::QuantumCircuit& circuit, std::stringstream& gateline_stream, std::istream& circuit_stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;

    gateline_stream >> dummy_str;    // 'target'
    gateline_stream >> dummy_str;    // ':'
    gateline_stream >> dummy_ch;     // '['
    gateline_stream >> target_qubit; // target qubit
    gateline_stream >> dummy_ch;     // ']'

    const auto unitary = parse_matrix2x2_(circuit_stream);
    circuit.add_u_gate(unitary, target_qubit);
}

inline void parse_cu_gate_(ket::QuantumCircuit& circuit, std::stringstream& gateline_stream, std::istream& circuit_stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;
    std::size_t control_qubit;

    gateline_stream >> dummy_str;    // 'target'
    gateline_stream >> dummy_str;    // ':'
    gateline_stream >> dummy_ch;     // '['
    gateline_stream >> target_qubit; // target qubit
    gateline_stream >> dummy_ch;     // ']'
    gateline_stream >> dummy_str;    // 'control'
    gateline_stream >> dummy_str;    // ':'
    gateline_stream >> dummy_ch;     // '['
    gateline_stream >> control_qubit; // control qubit
    gateline_stream >> dummy_ch;     // ']'

    const auto unitary = parse_matrix2x2_(circuit_stream);
    circuit.add_cu_gate(unitary, control_qubit, target_qubit);
}

}  // namespace impl_ket


namespace ket
{

inline auto read_tangelo_circuit(std::size_t n_qubits, std::istream& stream, std::size_t n_skip_lines) -> QuantumCircuit
{
    namespace gid = impl_ket::gate_id;
    using G = ket::Gate;

    auto circuit = ket::QuantumCircuit {n_qubits};

    std::string line;

    for (std::size_t i {0}; i < n_skip_lines; ++i) {
        std::getline(stream, line);
    }

    while (std::getline(stream, line)) {
        auto gatestream = std::stringstream {line};

        std::string gate_name;
        gatestream >> gate_name;

        if (gate_name == "") {
            continue;
        }

        const auto local_name = impl_ket::tangelo_to_local_name_(gate_name);

        // handle the special cases where tangelo has primitive gates that don't exist in the local code
        if (local_name == "SWAP") {
            impl_ket::parse_swap_gate_(circuit, gatestream);
            continue;
        }

        // attempt to parse the gate
        const auto gate = [&]() {
            try {
                return impl_ket::PRIMITIVE_GATES_TO_STRING.at_reverse(local_name);
            }
            catch (const std::runtime_error& e) {
                auto err_msg = std::stringstream {};
                err_msg << "Unknown gate found in `read_tangelo_file()` : " << local_name << '\n';
                throw std::runtime_error {err_msg.str()};
            }
        }();

        if (gid::is_one_target_transform_gate(gate)) {
            impl_ket::parse_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_transform_gate(gate)) {
            impl_ket::parse_one_control_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_target_one_angle_transform_gate(gate)) {
            impl_ket::parse_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_one_angle_transform_gate(gate)) {
            impl_ket::parse_one_control_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gate == G::M) {
            impl_ket::parse_m_gate_(circuit, gatestream);
        }
        else if (gate == G::U) {
            impl_ket::parse_u_gate_(circuit, gatestream, stream);
        }
        else if (gate == G::CU) {
            impl_ket::parse_cu_gate_(circuit, gatestream, stream);
        }
        else {
            throw std::runtime_error {"DEV ERROR: A gate type with no implemented conversion has been encountered.\n"};
        }
    }

    return circuit;
}

inline auto read_tangelo_circuit(std::size_t n_qubits, const std::filesystem::path& filepath, std::size_t n_skip_lines) -> QuantumCircuit
{
    auto instream = std::ifstream {filepath};

    if (!instream.is_open()) {
        auto err_msg = std::stringstream {};
        err_msg << "ERROR: unable to read tangelo circuit from : '" << filepath << "'\n";

        throw std::ios::failure {err_msg.str()};
    }

    return read_tangelo_circuit(n_qubits, instream, n_skip_lines);
}

}  // namespace ket

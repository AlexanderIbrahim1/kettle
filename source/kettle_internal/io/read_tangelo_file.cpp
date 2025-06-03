#include <cstddef>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/swap.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/io/read_tangelo_file.hpp"

#include "kettle_internal/gates/primitive_gate/gate_id.hpp"
#include "kettle_internal/gates/primitive_gate_map.hpp"
#include "kettle_internal/io/io_control_flow.hpp"

namespace
{

/*
    Certain names of primitive gates do not match between the tangelo codebase and this codebase;
    this function converts tangelo-specific names to names used here.
*/
auto tangelo_to_local_name_(const std::string& name) -> std::string
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

void parse_swap_gate_(ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit0;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit1;  // NOLINT(cppcoreguidelines-init-variables)

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit0; // target qubit 0
    stream >> dummy_ch;     // ','
    stream >> target_qubit1; // target qubit 1
    stream >> dummy_ch;     // ']'

    ket::apply_swap(circuit, target_qubit0, target_qubit1);
}

void parse_one_target_gate_(ket::PrimitiveGate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'

    const auto func = ket::internal::GATE_TO_FUNCTION_1T.at(gate);
    (circuit.*func)(target_qubit);
}

void parse_one_control_one_target_gate_(ket::PrimitiveGate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t control_qubit;  // NOLINT(cppcoreguidelines-init-variables)

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

    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T.at(gate);
    (circuit.*func)(control_qubit, target_qubit);
}

void parse_one_target_one_angle_gate_(ket::PrimitiveGate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)
    double angle;  // NOLINT(cppcoreguidelines-init-variables)

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'
    stream >> dummy_str;    // 'parameter'
    stream >> dummy_str;    // ':'
    stream >> angle;        // angle

    const auto func = ket::internal::GATE_TO_FUNCTION_1T1A.at(gate);
    (circuit.*func)(target_qubit, angle);
}

void parse_one_control_one_target_one_angle_gate_(ket::PrimitiveGate gate, ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t control_qubit;  // NOLINT(cppcoreguidelines-init-variables)
    double angle;  // NOLINT(cppcoreguidelines-init-variables)

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

    const auto func = ket::internal::GATE_TO_FUNCTION_1C1T1A.at(gate);
    (circuit.*func)(control_qubit, target_qubit, angle);
}

void parse_m_gate_(ket::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t qubit;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t bit;  // NOLINT(cppcoreguidelines-init-variables)

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

auto parse_complex_(std::stringstream& stream) -> std::complex<double>
{
    char dummy_char;  // NOLINT(cppcoreguidelines-init-variables)
    double real;  // NOLINT(cppcoreguidelines-init-variables)
    double imag;  // NOLINT(cppcoreguidelines-init-variables)

    stream >> dummy_char;    // '['
    stream >> real;          // real component
    stream >> dummy_char;    // ','
    stream >> imag;          // imaginary component
    stream >> dummy_char;    // ']'

    return {real, imag};
}

auto parse_matrix2x2_(std::istream& stream) -> ket::Matrix2X2
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

    return {.elem00=elem00, .elem01=elem01, .elem10=elem10, .elem11=elem11};
}

void parse_u_gate_(ket::QuantumCircuit& circuit, std::stringstream& gateline_stream, std::istream& circuit_stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)

    gateline_stream >> dummy_str;    // 'target'
    gateline_stream >> dummy_str;    // ':'
    gateline_stream >> dummy_ch;     // '['
    gateline_stream >> target_qubit; // target qubit
    gateline_stream >> dummy_ch;     // ']'

    const auto unitary = parse_matrix2x2_(circuit_stream);
    circuit.add_u_gate(unitary, target_qubit);
}

void parse_cu_gate_(ket::QuantumCircuit& circuit, std::stringstream& gateline_stream, std::istream& circuit_stream)
{
    std::string dummy_str;
    char dummy_ch;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t target_qubit;  // NOLINT(cppcoreguidelines-init-variables)
    std::size_t control_qubit;  // NOLINT(cppcoreguidelines-init-variables)

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

}  // namespace


namespace ket
{

auto read_tangelo_circuit(  // NOLINT(misc-no-recursion, readability-function-cognitive-complexity)
    std::size_t n_qubits,
    std::istream& stream,
    std::size_t n_skip_lines,
    std::optional<std::size_t> line_starts_with_spaces
) -> QuantumCircuit
{
    namespace gid = ket::internal::gate_id;
    namespace io_par = ket::internal::parse;
    using G = ket::PrimitiveGate;

    constexpr auto n_whitespace = ket::internal::CONTROL_FLOW_WHITESPACE_DEFAULT;

    auto circuit = ket::QuantumCircuit {n_qubits};

    std::string line;

    for (std::size_t i {0}; i < n_skip_lines; ++i) {
        std::getline(stream, line);
    }

    auto curr_pos = stream.tellg();
    while (std::getline(stream, line)) {
        auto gatestream = std::stringstream {line};

        // if the start of the line needs to satisfy a certain condition, and it doesn't; break early
        if (line_starts_with_spaces.has_value())
        {
            const auto whitespace = std::string(line_starts_with_spaces.value(), ' ');

            if (!line.starts_with(whitespace)) {
                stream.seekg(curr_pos);
                return circuit;
            }
        }

        std::string name;
        gatestream >> name;

        if (name == "") {
            continue;
        }

        if (name == "IF") {
            auto predicate = io_par::parse_control_flow_predicate_(gatestream);
            
            auto if_circuit = read_tangelo_circuit(n_qubits, stream, 0, n_whitespace);
            circuit.add_if_statement(std::move(predicate), std::move(if_circuit));

            continue;
        }

        if (name == "ELSE") {
            const auto n_elements = circuit.n_circuit_elements();
            const auto top_element = circuit[n_elements - 1];
            circuit.pop_back();

            if (!top_element.is_control_flow() || !top_element.get_control_flow().is_if_statement()) {
                throw std::runtime_error {"ERROR: encountered an 'ELSE' statement, but no previous matching 'IF' statement was found.\n"};
            }

            const auto& if_stmt = top_element.get_control_flow().get_if_statement();

            auto else_circuit = read_tangelo_circuit(n_qubits, stream, 0, n_whitespace);
            circuit.add_if_else_statement(if_stmt.predicate(), *if_stmt.circuit(), std::move(else_circuit));
            continue;
        }

        const auto local_name = tangelo_to_local_name_(name);

        // handle the special cases where tangelo has primitive gates that don't exist in the local code
        if (local_name == "SWAP") {
            parse_swap_gate_(circuit, gatestream);
            continue;
        }

        // attempt to parse the gate
        const auto gate = [&]() {
            try {
                return ket::internal::PRIMITIVE_GATES_TO_STRING.at_reverse(local_name);
            }
            catch (const std::runtime_error& e) {
                auto err_msg = std::stringstream {};
                err_msg << "Unknown gate found in `read_tangelo_file()` : " << local_name << '\n';
                throw std::runtime_error {err_msg.str()};
            }
        }();

        if (gid::is_one_target_transform_gate(gate)) {
            parse_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_transform_gate(gate)) {
            parse_one_control_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_target_one_angle_transform_gate(gate)) {
            parse_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_one_angle_transform_gate(gate)) {
            parse_one_control_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gate == G::M) {
            parse_m_gate_(circuit, gatestream);
        }
        else if (gate == G::U) {
            parse_u_gate_(circuit, gatestream, stream);
        }
        else if (gate == G::CU) {
            parse_cu_gate_(circuit, gatestream, stream);
        }
        else {
            throw std::runtime_error {"DEV ERROR: A gate type with no implemented conversion has been encountered.\n"};
        }

        curr_pos = stream.tellg();
    }

    return circuit;
}

auto read_tangelo_circuit(
    std::size_t n_qubits,
    const std::filesystem::path& filepath,
    std::size_t n_skip_lines
) -> QuantumCircuit
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

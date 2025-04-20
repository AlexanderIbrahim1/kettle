#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include <mini-qiskit/gates/primitive_gate.hpp>
#include <mini-qiskit/gates/primitive_gate_map.hpp>
#include <mini-qiskit/circuit/circuit.hpp>
#include <mini-qiskit/gates/swap.hpp>

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

namespace impl_mqis
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

inline void parse_swap_gate_(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

    mqis::apply_swap(circuit, target_qubit0, target_qubit1);
}

inline void parse_one_target_gate_(mqis::Gate gate, mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

inline void parse_one_control_one_target_gate_(mqis::Gate gate, mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

inline void parse_one_target_one_angle_gate_(mqis::Gate gate, mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

inline void parse_one_control_one_target_one_angle_gate_(mqis::Gate gate, mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

}  // namespace impl_mqis


namespace mqis
{

inline auto read_tangelo_circuit(std::size_t n_qubits, std::istream& stream, std::size_t n_skip_lines) -> QuantumCircuit
{
    namespace gid = impl_mqis::gate_id;
    using G = mqis::Gate;

    auto circuit = mqis::QuantumCircuit {n_qubits};

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

        const auto local_name = impl_mqis::tangelo_to_local_name_(gate_name);

        // handle the special cases where tangelo has primitive gates that don't exist in the local code
        if (local_name == "SWAP") {
            impl_mqis::parse_swap_gate_(circuit, gatestream);
            continue;
        }

        // attempt to parse the gate
        const auto gate = [&]() {
            try {
                return impl_mqis::PRIMITIVE_GATES_TO_STRING.at_reverse(local_name);
            }
            catch (const std::runtime_error& e) {
                auto err_msg = std::stringstream {};
                err_msg << "Unknown gate found in `read_tangelo_file()` : " << local_name << '\n';
                throw std::runtime_error {err_msg.str()};
            }
        }();

        if (gid::is_one_target_transform_gate(gate)) {
            impl_mqis::parse_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_transform_gate(gate)) {
            impl_mqis::parse_one_control_one_target_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_target_one_angle_transform_gate(gate)) {
            impl_mqis::parse_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gid::is_one_control_one_target_one_angle_transform_gate(gate)) {
            impl_mqis::parse_one_control_one_target_one_angle_gate_(gate, circuit, gatestream);
        }
        else if (gate == G::M || gate == G::U || gate == G::CU || gate == G::CONTROL) {
            // DO NOTHING FOR NOW
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

}  // namespace mqis

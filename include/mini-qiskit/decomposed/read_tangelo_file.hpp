#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <mini-qiskit/primitive_gate.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/common/utils.hpp>
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

template <mqis::Gate GateType>
void parse_one_target_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'

    if constexpr (GateType == mqis::Gate::H) {
        circuit.add_h_gate(target_qubit);
    }
    else {
        static_assert(impl_mqis::always_false<void>::value, "Invalid gate template provided to `parse_r_gate()`");
    }
}

inline void parse_swap_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

inline void parse_cx_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

    circuit.add_cx_gate(control_qubit, target_qubit);
}

template <mqis::Gate GateType>
void parse_one_target_one_angle_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

    if constexpr (GateType == mqis::Gate::RX) {
        circuit.add_rx_gate(target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::RY) {
        circuit.add_ry_gate(target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::RZ) {
        circuit.add_rz_gate(target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::P) {
        circuit.add_p_gate(target_qubit, angle);
    }
    else {
        static_assert(
            impl_mqis::always_false<void>::value,
            "Invalid gate template provided to `parse_one_target_one_angle_gate()`"
        );
    }
}

template <mqis::Gate GateType>
void parse_one_target_one_control_one_angle_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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

    if constexpr (GateType == mqis::Gate::CP) {
        circuit.add_cp_gate(control_qubit, target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::CRX) {
        circuit.add_crx_gate(control_qubit, target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::CRY) {
        circuit.add_cry_gate(control_qubit, target_qubit, angle);
    }
    else if constexpr (GateType == mqis::Gate::CRZ) {
        circuit.add_crz_gate(control_qubit, target_qubit, angle);
    }
    else {
        static_assert(
            impl_mqis::always_false<void>::value,
            "Invalid gate template provided to `parse_one_target_one_control_one_angle_gate()`"
        );
    }
}

}  // namespace impl_mqis


namespace mqis
{

inline auto read_tangelo_circuit(std::size_t n_qubits, std::istream& stream, std::size_t n_skip_lines) -> QuantumCircuit
{
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

        if (gate_name == "H") {
            impl_mqis::parse_one_target_gate<Gate::H>(circuit, gatestream);
        }
        else if (gate_name == "RX") {
            impl_mqis::parse_one_target_one_angle_gate<Gate::RX>(circuit, gatestream);
        }
        else if (gate_name == "RY") {
            impl_mqis::parse_one_target_one_angle_gate<Gate::RY>(circuit, gatestream);
        }
        else if (gate_name == "RZ") {
            impl_mqis::parse_one_target_one_angle_gate<Gate::RZ>(circuit, gatestream);
        }
        else if (gate_name == "PHASE") {
            impl_mqis::parse_one_target_one_angle_gate<Gate::P>(circuit, gatestream);
        }
        else if (gate_name == "CNOT") {
            impl_mqis::parse_cx_gate(circuit, gatestream);
        }
        else if (gate_name == "CPHASE") {
            impl_mqis::parse_one_target_one_control_one_angle_gate<Gate::CP>(circuit, gatestream);
        }
        else if (gate_name == "CRX") {
            impl_mqis::parse_one_target_one_control_one_angle_gate<Gate::CRX>(circuit, gatestream);
        }
        else if (gate_name == "CRY") {
            impl_mqis::parse_one_target_one_control_one_angle_gate<Gate::CRY>(circuit, gatestream);
        }
        else if (gate_name == "CRZ") {
            impl_mqis::parse_one_target_one_control_one_angle_gate<Gate::CRZ>(circuit, gatestream);
        }
        else if (gate_name == "SWAP") {
            impl_mqis::parse_swap_gate(circuit, gatestream);
        }
        else {
            auto err_msg = std::stringstream {};
            err_msg << "Unknown gate found in `read_tangelo_file()` : " << gate_name << '\n';
            throw std::runtime_error {err_msg.str()};
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

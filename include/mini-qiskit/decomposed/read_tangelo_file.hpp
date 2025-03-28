#pragma once

#include <cstddef>
#include <sstream>
#include <string>

#include <mini-qiskit/primitive_gate.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/common/utils.hpp>

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

inline void parse_h_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;

    stream >> dummy_str;    // 'target'
    stream >> dummy_str;    // ':'
    stream >> dummy_ch;     // '['
    stream >> target_qubit; // target qubit
    stream >> dummy_ch;     // ']'

    circuit.add_h_gate(target_qubit);
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
inline void parse_r_gate(mqis::QuantumCircuit& circuit, std::stringstream& stream)
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
        circuit.add_rx_gate(angle, target_qubit);
    }
    else if constexpr (GateType == mqis::Gate::RZ) {
        circuit.add_rz_gate(angle, target_qubit);
    }
    else {
        static_assert(impl_mqis::always_false<void>::value, "Invalid gate template provided to `parse_r_gate()`");
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

        if (gate_name == "H") {
            impl_mqis::parse_h_gate(circuit, gatestream);
        }
        else if (gate_name == "RX") {
            impl_mqis::parse_r_gate<Gate::RX>(circuit, gatestream);
        }
        else if (gate_name == "RZ") {
            impl_mqis::parse_r_gate<Gate::RZ>(circuit, gatestream);
        }
        else if (gate_name == "CNOT") {
            impl_mqis::parse_cx_gate(circuit, gatestream);
        }
        else {
            throw std::runtime_error {"`read_tangelo_file()` can only handle the following gates: 'H', 'RX', 'RZ', 'CNOT'"};
        }
    }

    return circuit;
}

}  // namespace mqis

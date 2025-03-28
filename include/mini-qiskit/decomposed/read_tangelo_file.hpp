#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include <mini-qiskit/primitive_gate.hpp>

/*
This script parses the file of gates produced by the tangelo code.
*/

/*
H         target : [4]   
RX        target : [5]   parameter : 1.5707963267948966
CNOT      target : [4]   control : [2]   
CNOT      target : [5]   control : [4]   
RZ        target : [5]   parameter : 12.533816585267923
*/

namespace impl_mqis
{

inline auto parse_h_gate(std::stringstream& stream) -> mqis::GateInfo
{
    std::string dummy_str;
    char dummy_ch;
    std::size_t target_qubit;
    stream >> dummy_str; // 'target'
    stream >> dummy_str; // ':'
    stream >> dummy_ch; // '['
    stream >> target_qubit;
    stream >> dummy_ch;

    return impl_mqis::create_h_gate(target_qubit);
}

}  // namespace impl_mqis


namespace mqis
{

inline auto read_tangelo_file(std::istream& stream) -> std::vector<GateInfo>
{
    auto output = std::vector<GateInfo> {};

    std::string line;

    while (std::getline(stream, line)) {
        auto gatestream = std::stringstream {line};

        std::string gate_name;
        gatestream >> gate_name;
        if (gate_name == "H") {
            output.emplace_back(impl_mqis::parse_h_gate(gatestream));
        }
    }

    return output;
}

}  // namespace mqis

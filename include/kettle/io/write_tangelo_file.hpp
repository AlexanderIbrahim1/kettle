#pragma once

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"

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

namespace ket::internal
{

auto format_double_(double x) -> std::string;

auto format_matrix2x2_(const ket::Matrix2X2& mat) -> std::string;

auto format_one_target_gate_(const ket::GateInfo& info) -> std::string;

auto format_one_control_one_target_gate_(const ket::GateInfo& info) -> std::string;

auto format_one_target_one_angle_gate_(const ket::GateInfo& info) -> std::string;

auto format_one_control_one_target_one_angle_gate_(const ket::GateInfo& info) -> std::string;

auto format_m_gate_(const ket::GateInfo& info) -> std::string;

auto format_u_gate_(const ket::GateInfo& info, const ket::Matrix2X2& mat) -> std::string;

auto format_cu_gate_(const ket::GateInfo& info, const ket::Matrix2X2& mat) -> std::string;

}  // namespace ket::internal


namespace ket
{

/*
    The underlying helper function for `write_tangelo_circuit()`, that takes an output stream `stream`
    as an argument instead of the path to the file.

    MAYBE TODO: change to allow nested control flow?
*/
void write_tangelo_circuit(  // NOLINT(misc-no-recursion, readability-function-cognitive-complexity)
    const ket::QuantumCircuit& circuit,
    std::ostream& stream,
    std::size_t n_leading_whitespace = 0
);

void write_tangelo_circuit(const QuantumCircuit& circuit, const std::filesystem::path& filepath);

void print_tangelo_circuit(const QuantumCircuit& circuit);

}  // namespace ket

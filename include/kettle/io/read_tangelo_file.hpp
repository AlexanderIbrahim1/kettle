#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>

#include <kettle/circuit/circuit.hpp>

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


namespace ket
{

/*
    The underlying helper function for `read_tangelo_circuit()`, that takes an input stream `stream`
    as an argument instead of the path to the file.
*/
auto read_tangelo_circuit(  // NOLINT(misc-no-recursion, readability-function-cognitive-complexity)
    std::size_t n_qubits,
    std::istream& stream,
    std::size_t n_skip_lines,
    std::optional<std::size_t> line_starts_with_spaces = std::nullopt
) -> QuantumCircuit;

/*
    Takes a file `filepath` that holds a quantum circuit in the tangelo format, and reconstructs
    the `QuantumCircuit` instance of `n_qubits` qubits that it represents.

    This function can skip the first `n_skip_lines` lines of the file.

    For the time being, only primitive gates, SWAP gates, and control flow elements can be read.
    Any circuit elements related to logging are ignored.
*/
auto read_tangelo_circuit(
    std::size_t n_qubits,
    const std::filesystem::path& filepath,
    std::size_t n_skip_lines
) -> QuantumCircuit;

}  // namespace ket

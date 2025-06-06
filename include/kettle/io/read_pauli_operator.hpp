#pragma once

#include <cstddef>
#include <filesystem>

#include "kettle/operator/pauli/pauli_operator.hpp"

/*
    This file contains the `read_pauli_operator()` function, which takes an output file
    from the Python qpe_dipolar_planar_rotor project, and reads it into a `PauliOperator`
    instance.
*/

namespace ket
{

auto read_pauli_operator(std::istream& instream, std::size_t n_qubits) -> ket::PauliOperator;

auto read_pauli_operator(const std::filesystem::path& filepath, std::size_t n_qubits) -> ket::PauliOperator;

}  // namespace ket

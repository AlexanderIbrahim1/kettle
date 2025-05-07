#pragma once

#include <iostream>
#include <filesystem>

#include "kettle/state/state.hpp"


namespace ket
{

auto read_numpy_statevector(
    std::istream& instream,
    QuantumStateEndian input_endian = QuantumStateEndian::LITTLE
) -> QuantumState;

auto read_numpy_statevector(
    const std::filesystem::path& filepath,
    QuantumStateEndian input_endian = QuantumStateEndian::LITTLE
) -> QuantumState;

}  // namespace ket

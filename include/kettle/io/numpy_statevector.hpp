#pragma once

#include <iostream>
#include <filesystem>

#include "kettle/state/statevector.hpp"


namespace ket
{

auto read_numpy_statevector(
    std::istream& instream,
    Endian input_endian = Endian::LITTLE
) -> Statevector;

auto read_numpy_statevector(
    const std::filesystem::path& filepath,
    Endian input_endian = Endian::LITTLE
) -> Statevector;

}  // namespace ket

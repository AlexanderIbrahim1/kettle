#pragma once

#include <filesystem>
#include <iostream>

#include "kettle/state/statevector.hpp"


namespace ket
{

void save_statevector(
    std::ostream& outstream,
    const Statevector& state,
    Endian endian = Endian::LITTLE
);

void save_statevector(
    const std::filesystem::path& filepath,
    const Statevector& state,
    Endian endian = Endian::LITTLE
);

auto load_statevector(std::istream& instream) -> Statevector;

auto load_statevector(const std::filesystem::path& filepath) -> Statevector;

}  // namespace ket

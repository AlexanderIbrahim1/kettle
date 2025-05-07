#pragma once

#include <filesystem>
#include <iostream>

#include "kettle/state/state.hpp"


namespace ket
{

void save_statevector(
    std::ostream& outstream,
    const QuantumState& state,
    QuantumStateEndian endian = QuantumStateEndian::LITTLE
);

void save_statevector(
    const std::filesystem::path& filepath,
    const QuantumState& state,
    QuantumStateEndian endian = QuantumStateEndian::LITTLE
);

auto load_statevector(std::istream& instream) -> QuantumState;

auto load_statevector(const std::filesystem::path& filepath) -> QuantumState;

}  // namespace ket

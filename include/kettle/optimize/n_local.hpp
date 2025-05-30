#pragma once

#include <cstdint>
#include <vector>

#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/primitive_gate.hpp>

namespace ket
{

enum class NLocalEntangelement : std::uint8_t
{
    FULL,
    LINEAR
};

struct skip_last_rotation_layer
{
    explicit skip_last_rotation_layer() = default;
};

inline auto n_local(
    std::size_t n_qubits,
    const std::vector<Gate>& rotation_blocks,
    const std::vector<Gate>& entanglement_blocks,
    NLocalEntangelement entangelemnt_kind,
    std::size_t n_repetitions,
    skip_last_rotation_layer key
)
{

}

}  // namespace ket

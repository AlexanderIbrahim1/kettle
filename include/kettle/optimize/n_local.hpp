#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include <kettle/circuit/circuit.hpp>
#include <kettle/gates/compound_gate.hpp>
#include <kettle/gates/primitive_gate.hpp>

namespace ket
{

inline constexpr auto DEFAULT_NLOCAL_GATE_PARAMETER = double {0.0};

using GeneralGate = std::variant<Gate, CompoundGate>;

enum class NLocalEntangelement : std::uint8_t
{
    FULL,
    LINEAR
};

enum class SkipLastRotationLayerFlag : std::uint8_t
{
    TRUE,
    FALSE
};

auto n_local(
    std::size_t n_qubits,
    const std::vector<GeneralGate>& rotation_blocks,
    const std::vector<GeneralGate>& entanglement_blocks,
    NLocalEntangelement entanglement_kind,
    std::size_t n_repetitions,
    SkipLastRotationLayerFlag flag = SkipLastRotationLayerFlag::FALSE
) -> std::tuple<QuantumCircuit, std::vector<ket::param::ParameterID>>;

}  // namespace ket

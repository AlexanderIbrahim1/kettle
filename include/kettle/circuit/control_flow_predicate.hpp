#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "kettle/circuit/classical_register.hpp"

namespace ket
{

enum class ControlFlowBooleanKind : std::uint8_t
{
    IF,
    IF_NOT
};

class ControlFlowPredicate
{
public:
    ControlFlowPredicate(
        std::vector<std::size_t> bit_indices_to_check,
        std::vector<int> expected_bits,
        ControlFlowBooleanKind control_kind
    );

    auto operator()(const ClassicalRegister& creg) const -> bool;

    [[nodiscard]]
    constexpr auto bit_indices_to_check() const -> const std::vector<std::size_t>&
    {
        return bit_indices_to_check_;
    }

    [[nodiscard]]
    constexpr auto expected_bits() const -> const std::vector<int>&
    {
        return expected_bits_;
    }

    [[nodiscard]]
    constexpr auto control_kind() const -> ControlFlowBooleanKind
    {
        return control_kind_;
    }

    friend constexpr auto operator<=>(const ControlFlowPredicate& left, const ControlFlowPredicate& right) noexcept = default;

private:
    std::vector<std::size_t> bit_indices_to_check_;
    std::vector<int> expected_bits_;
    ControlFlowBooleanKind control_kind_;
};

}  // namespace ket

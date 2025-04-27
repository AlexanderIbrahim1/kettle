#pragma once

#include <cstddef>
#include <optional>
#include <vector>

/*
    The ClassicalRegister holds measured bits from a simulation.
*/

namespace ket
{

class ClassicalRegister
{
public:
    explicit ClassicalRegister(std::size_t n_bits)
        : measured_bits_ (n_bits, std::nullopt)
    {}

    [[nodiscard]]
    constexpr auto is_measured(std::size_t qubit_index) const -> bool
    {
        return measured_bits_[qubit_index].has_value();
    }

    [[nodiscard]]
    constexpr auto get(std::size_t qubit_index) const -> int
    {
        return measured_bits_[qubit_index].value();
    }

    constexpr void set(std::size_t qubit_index, int value)
    {
        measured_bits_[qubit_index] = value;
    }

private:
    std::vector<std::optional<int>> measured_bits_;
};

}  // namespace ket

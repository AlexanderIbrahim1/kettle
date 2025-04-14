#pragma once

#include <cstddef>
#include <optional>
#include <vector>

/*
    The ClassicalRegister holds measured bits from a simulation.
*/

namespace mqis
{

class ClassicalRegister
{
public:
    explicit ClassicalRegister(std::size_t n_bits)
        : measured_bits_ (n_bits, std::nullopt)
    {}

    constexpr auto is_measured(std::size_t qubit_index) const -> bool
    {
        return measured_bits_[qubit_index].has_value();
    }

    constexpr auto get(std::size_t qubit_index) const -> int
    {
        return measured_bits_[qubit_index].value();
    }

    constexpr void set(std::size_t qubit_index)
    {
        measured_bits_[qubit_index] = qubit_index;
    }

private:
    std::vector<std::optional<int>> measured_bits_;
};

}  // namespace mqis

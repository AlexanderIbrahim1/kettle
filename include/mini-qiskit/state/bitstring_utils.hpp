#pragma once

#include <bitset>
#include <cstddef>
#include <limits>
#include <string>

namespace impl_mqis
{

constexpr auto endian_flip(std::size_t value, std::size_t n_relevant_bits) -> std::size_t
{
    auto backward = std::size_t {0};

    for (std::size_t i {0}; i < n_relevant_bits; ++i) {
        backward <<= 1;
        backward |= (value & 1);
        value >>= 1;
    }

    return backward;
}

inline auto qubit_string_to_state_index(const std::string& computational_state) -> std::size_t
{
    constexpr auto n_bits = std::numeric_limits<std::size_t>::digits;
    const auto bits = std::bitset<n_bits> {computational_state};

    return static_cast<std::size_t>(bits.to_ullong());
}

}  // namespace impl_mqis

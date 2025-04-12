#pragma once

#include <optional>
#include <random>

/*
    This header file contains functions related to random number generation and sampling.
*/

namespace impl_mqis
{

inline auto get_prng_(std::optional<int> seed) -> std::mt19937
{
    if (seed) {
        const auto seed_val = static_cast<std::mt19937::result_type>(seed.value());
        return std::mt19937 {seed_val};
    }
    else {
        auto device = std::random_device {};
        return std::mt19937 {device()};
    }
}

}  // namespace impl_mqis

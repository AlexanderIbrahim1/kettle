#pragma once

#include <cstdint>
#include <optional>
#include <random>

#include "kettle/parameter/parameter.hpp"
#include "kettle_internal/common/prng.hpp"


namespace ket::internal
{

template <ket::internal::UniformIntDistribution Distribution = std::uniform_int_distribution<std::uint8_t>>
auto create_parameter_id_helper(std::optional<int> seed = std::nullopt) -> ParameterID
{
    auto prng = ket::internal::get_prng_(seed);
    auto id = ParameterID {};

    auto gen = Distribution {0, std::numeric_limits<std::uint8_t>::max()};

    for (auto& elem : id) {
        elem = gen(prng);
    }

    return id;
}

auto create_parameter_id(std::optional<int> seed = std::nullopt) -> ParameterID;

}  // namespace ket::internal

#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <string>

#include "kettle_internal/common/prng.hpp"


namespace ket
{

constexpr inline auto PARAMETER_ID_SIZE = std::size_t {16};

using ParameterID = std::array<std::uint8_t, PARAMETER_ID_SIZE>;

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

inline auto create_parameter_id(std::optional<int> seed = std::nullopt) -> ParameterID
{
    return create_parameter_id_helper<std::uniform_int_distribution<std::uint8_t>>(seed);
}

class Parameter
{
public:
    Parameter(std::string name, const ParameterID& id)
        : name_ {std::move(name)}
        , id_ {id}
    {}

    Parameter(std::string name, int seed)
        : name_ {std::move(name)}
        , id_ {create_parameter_id(seed)}
    {}

    explicit Parameter(std::string name)
        : name_ {std::move(name)}
        , id_ {create_parameter_id()}
    {}

    [[nodiscard]]
    constexpr auto name() const -> const std::string&
    {
        return name_;
    }

    [[nodiscard]]
    constexpr auto id() const -> const ParameterID&
    {
        return id_;
    }

private:
    std::string name_;
    ParameterID id_;
};

}  // namespace ket

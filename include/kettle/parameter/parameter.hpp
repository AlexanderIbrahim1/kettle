#pragma once

#include <array>
#include <cstdint>
#include <string>


namespace ket
{

constexpr inline auto PARAMETER_ID_SIZE = std::size_t {16};

using ParameterID = std::array<std::uint8_t, PARAMETER_ID_SIZE>;

class Parameter
{
public:
    Parameter(std::string name, const ParameterID& id);

    Parameter(std::string name, int seed);

    explicit Parameter(std::string name);

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

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>


namespace ket::param
{

/*
    A tag type passed to member functions of `QuantumCircuit` that create parameterized
    gates (RX, RY, CRX, etc.). Passing this tag indicates that the gate should be parameterized,
    and that a new `Parameter` instance is created.
*/
struct parameterized
{
    explicit parameterized() = default;
};

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

struct ParameterIdHash
{
    auto operator()(const ParameterID& id) const noexcept -> std::size_t;
};

struct ParameterData
{
    std::optional<double> value;
    std::string name;
    std::size_t count;
};

using ParameterDataMap = std::unordered_map<ParameterID, ParameterData, ParameterIdHash>;
using EvaluatedParameterDataMap = std::unordered_map<ParameterID, double, ParameterIdHash>;

}  // namespace ket::param

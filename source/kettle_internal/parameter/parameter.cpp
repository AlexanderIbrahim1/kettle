#include <cstdint>
#include <optional>
#include <random>
#include <string>

#include "kettle/parameter/parameter.hpp"
#include "kettle_internal/parameter/parameter_internal.hpp"


namespace ket::internal
{

auto create_parameter_id(std::optional<int> seed) -> ParameterID
{
    return create_parameter_id_helper<std::uniform_int_distribution<std::uint8_t>>(seed);
}

}  // namespace ket::internal

namespace ket
{

Parameter::Parameter(std::string name, const ParameterID& id)
    : name_ {std::move(name)}
    , id_ {id}
{}

Parameter::Parameter(std::string name, int seed)
    : name_ {std::move(name)}
    , id_ {ket::internal::create_parameter_id(seed)}
{}

Parameter::Parameter(std::string name)
    : name_ {std::move(name)}
    , id_ {ket::internal::create_parameter_id()}
{}

}  // namespace ket

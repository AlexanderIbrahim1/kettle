#pragma once

#include <cstdint>


namespace ket
{

/*
    The `CompoundGate` type represents quantum gates that are not directly simulated on a
    simulator. A `GeneralGate` must be decomposed into one or more `Gate` instances to be
    simulated.
*/
enum class CompoundGate : std::uint8_t
{
    CCX,
    CCY,
    CCZ
};

}  // namespace ket

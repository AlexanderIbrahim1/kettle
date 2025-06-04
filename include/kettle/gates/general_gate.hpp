#pragma once

#include <cstdint>


namespace ket
{

/*
    The `GeneralGate` type represents quantum gates that are not directly simulated on a
    simulator. A `GeneralGate` must be decomposed into one or more `Gate` instances to be
    simulated.
*/
enum class GeneralGate : std::uint8_t
{
    CCX,
    CCY,
    CCZ
};

}  // namespace ket

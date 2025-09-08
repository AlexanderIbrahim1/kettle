#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/parameter/parameter_expression.hpp"

namespace ket
{

/*
    The `Gate` type represents quantum gates that are directly simulated on a simulator. If
    a gate is not one of these gates, then it must be decomposed into one or more of these
    gates to be simulated.
*/
enum class Gate : std::uint8_t
{
    H,
    X,
    Y,
    Z,
    S,
    SDAG,
    T,
    TDAG,
    SX,
    SXDAG,
    RX,
    RY,
    RZ,
    P,
    CH,
    CX,
    CY,
    CZ,
    CS,
    CSDAG,
    CT,
    CTDAG,
    CSX,
    CSXDAG,
    CRX,
    CRY,
    CRZ,
    CP,
    U,
    CU,
    M,
    RESET
};

/*
    The `GateInfo` type holds all the information needed to describe any of the primitive gates in
    the project's specification. This implementation forces the gate to carry the information needed
    for every possible type of gate, even if some parameters are not used. For example, the `GateInfo`
    instance for an X-gate only needs the target index (one `std::size_t`), but it still holds all the
    other parameters.

    Each of the primitive gates can have up to two index arguments:
      - a target qubit index
      - possibly a control qubit index
      - in the case of measurement gates, a qubit index and a classical bit index

    Some of the primitive gates can have one real parameter, an angle.

    The U and CU primitive gates can hold a pointer to a unitary 2x2 matrix.

*/
struct GateInfo
{
    Gate gate;
    std::size_t arg0;
    std::size_t arg1;
    double arg2;
    ket::ClonePtr<Matrix2X2> unitary_ptr;
    ket::ClonePtr<ket::param::ParameterExpression> param_expression_ptr;
};

}  // namespace ket


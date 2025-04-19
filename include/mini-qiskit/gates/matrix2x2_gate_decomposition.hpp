#pragma once

#include <algorithm>
#include <compare>
#include <optional>

#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/gates/primitive_gate.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"

/*
    This header file contains functions for decomposing a general 2x2 unitary matrix
    to a product of primitive 2x2 quantum gates.
*/

namespace impl_mqis
{

struct PrimitiveGateInfo
{
    mqis::Gate gate;
    std::optional<double> parameter = std::nullopt;

    friend bool operator<=>(const PrimitiveGateInfo& left, const PrimitiveGateInfo& right) = default;
};

/*
    Decompose a 2x2 unitary matrix into one of several primitive 1-qubit unitary gates.

    This function attempts to construct unparameterized gates before paramterized gates. For
    example, if the matrix [1, 0; 0 -1] is passed as an input, this function will attempt
    to decompose it as a Z gate instead of an RZ gate with an angle of (-M_PI/2).
*/
inline auto decomp_to_primitive_gate(
    const mqis::Matrix2X2& unitary,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::optional<impl_mqis::PrimitiveGateInfo>
{
    using Info = impl_mqis::PrimitiveGateInfo;
    // NOTES:
    // - the H, X, Y, and Z gates take no arguments, and so we can check them directly
    // - the RX, RY, RZ gates all share the feature that the angle can be recovered
    //   from the arccos of the real component of elem11
    // - the P gate requires information from both the real and imaginary components of elem11
    if (almost_eq(unitary, mqis::h_gate(), tolerance_sq)) {
        return Info {mqis::Gate::H, {}};
    }
    else if (almost_eq(unitary, mqis::x_gate(), tolerance_sq)) {
        return Info {mqis::Gate::X, {}};
    }
    else if (almost_eq(unitary, mqis::y_gate(), tolerance_sq)) {
        return Info {mqis::Gate::Y, {}};
    }
    else if (almost_eq(unitary, mqis::z_gate(), tolerance_sq)) {
        return Info {mqis::Gate::Z, {}};
    }
    if (almost_eq(unitary, mqis::sx_gate(), tolerance_sq)) {
        return Info {mqis::Gate::SX, {}};
    }
    else {
        const auto real_11 = std::clamp(unitary.elem11.real(), -1.0, 1.0);
        const auto imag_11 = std::clamp(unitary.elem11.imag(), -1.0, 1.0);
        const auto theta = std::acos(real_11);
        const auto p_theta = std::atan2(imag_11, real_11);

        if (almost_eq(unitary, mqis::rx_gate(2.0 * theta), tolerance_sq)) {
            return Info {mqis::Gate::RX, 2.0 * theta};
        }
        else if (almost_eq(unitary, mqis::ry_gate(2.0 * theta), tolerance_sq)) {
            return Info {mqis::Gate::RY, 2.0 * theta};
        }
        else if (almost_eq(unitary, mqis::rz_gate(2.0 * theta), tolerance_sq)) {
            return Info {mqis::Gate::RZ, 2.0 * theta};
        }
        else if (almost_eq(unitary, mqis::p_gate(p_theta), tolerance_sq)) {
            return Info {mqis::Gate::P, p_theta};
        }
    }

    return std::nullopt;
}

}  // namespace impl_mqis

#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/primitive_gate_map.hpp"
#include "kettle/gates/common_u_gates.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"

/*
    This header file contains functions for decomposing a general 2x2 unitary matrix
    to a product of primitive 2x2 quantum gates.
*/

namespace impl_ket
{

struct PrimitiveGateInfo
{
    ket::Gate gate;
    std::optional<double> parameter = std::nullopt;
};

/*
    Find the angle of the determinant.
*/
inline auto determinant_angle_(const ket::Matrix2X2& matrix) -> double
{
    const auto det = ket::determinant(matrix);
    return std::atan2(det.imag(), det.real());
}

/*
    Decompose a 2x2 unitary matrix into one of several primitive 1-qubit unitary gates.

    This function attempts to construct unparameterized gates before paramterized gates. For
    example, if the matrix [1, 0; 0 -1] is passed as an input, this function will attempt
    to decompose it as a Z gate instead of an RZ gate with an angle of (-M_PI/2).
*/
inline auto decomp_to_single_primitive_gate_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::optional<impl_ket::PrimitiveGateInfo>
{
    using Info = impl_ket::PrimitiveGateInfo;
    // NOTES:
    // - the H, X, Y, and Z gates take no arguments, and so we can check them directly
    // - the RX, RY, RZ gates all share the feature that the angle can be recovered
    //   from the arccos of the real component of elem11
    // - the P gate requires information from both the real and imaginary components of elem11
    if (almost_eq(unitary, ket::h_gate(), tolerance_sq)) {
        return Info {ket::Gate::H, {}};
    }
    else if (almost_eq(unitary, ket::x_gate(), tolerance_sq)) {
        return Info {ket::Gate::X, {}};
    }
    else if (almost_eq(unitary, ket::y_gate(), tolerance_sq)) {
        return Info {ket::Gate::Y, {}};
    }
    else if (almost_eq(unitary, ket::z_gate(), tolerance_sq)) {
        return Info {ket::Gate::Z, {}};
    }
    if (almost_eq(unitary, ket::sx_gate(), tolerance_sq)) {
        return Info {ket::Gate::SX, {}};
    }
    else {
        const auto real_11 = std::clamp(unitary.elem11.real(), -1.0, 1.0);
        const auto theta = std::acos(real_11);

        if (almost_eq(unitary, ket::rx_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::Gate::RX, 2.0 * theta};
        }
        else if (almost_eq(unitary, ket::ry_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::Gate::RY, 2.0 * theta};
        }
        else if (almost_eq(unitary, ket::rz_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::Gate::RZ, 2.0 * theta};
        }

        const auto imag_11 = std::clamp(unitary.elem11.imag(), -1.0, 1.0);
        const auto p_theta = std::atan2(imag_11, real_11);

        if (almost_eq(unitary, ket::p_gate(p_theta), tolerance_sq)) {
            return Info {ket::Gate::P, p_theta};
        }
    }

    return std::nullopt;
}

/*
    The implementation of this decomposition is taken directly from the following file:
        https://github.com/fedimser/quantum_decomp/blob/master/quantum_decomp/src/decompose_2x2.py
    
    The author of the repository if fedimser.
    The repo is published under the MIT license.
*/
inline auto decomp_special_unitary_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<impl_ket::PrimitiveGateInfo>
{
    using Info = impl_ket::PrimitiveGateInfo;

    const auto abs00 = std::clamp(std::abs(unitary.elem00), 0.0, 1.0);

    const auto theta = -std::acos(abs00);
    const auto lambda = -std::atan2(unitary.elem00.imag(), unitary.elem00.real());
    const auto mu = -std::atan2(unitary.elem01.imag(), unitary.elem01.real());

    auto output = std::vector<Info> {};

    if (std::fabs(lambda - mu) > tolerance_sq) {
        output.emplace_back(ket::Gate::RZ, lambda - mu);
    }

    if (std::fabs(2.0 * theta) > tolerance_sq) {
        output.emplace_back(ket::Gate::RY, 2.0 * theta);
    }

    if (std::fabs(lambda + mu) > tolerance_sq) {
        output.emplace_back(ket::Gate::RZ, lambda + mu);
    }

    return output;
}

inline auto decomp_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<impl_ket::PrimitiveGateInfo>
{
    const auto primitive = decomp_to_single_primitive_gate_(unitary);
    if (primitive) {
        return {primitive.value()};
    }

    const auto det_angle = determinant_angle_(unitary);

    if (std::fabs(det_angle) < tolerance_sq) {
        return decomp_special_unitary_to_primitive_gates_(unitary, tolerance_sq);
    }
    else {
        const auto special_unitary = ket::p_gate(-det_angle) * unitary;
        auto output = decomp_special_unitary_to_primitive_gates_(special_unitary, tolerance_sq);
        output.emplace_back(ket::Gate::P, det_angle);

        return output;
    }
}

inline auto decomp_to_one_target_primitive_gates_(
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<ket::GateInfo>
{
    namespace cre = ket::internal::create;

    const auto primitives = decomp_to_primitive_gates_(unitary, tolerance_sq);

    auto output = std::vector<ket::GateInfo> {};
    for (const auto& primitive : primitives) {
        if (primitive.parameter.has_value()) {
            const auto angle = primitive.parameter.value();
            output.emplace_back(cre::create_one_target_one_angle_gate(primitive.gate, target, angle));
        } else {
            output.emplace_back(cre::create_one_target_gate(primitive.gate, target));
        }
    }

    return output;
}

inline auto decomp_to_one_control_one_target_primitive_gates_(
    std::size_t control,
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<ket::GateInfo>
{
    namespace cre = ket::internal::create;

    const auto primitives = decomp_to_primitive_gates_(unitary, tolerance_sq);

    auto output = std::vector<ket::GateInfo> {};
    for (const auto& primitive : primitives) {
        const auto ctrl_gate = UNCONTROLLED_TO_CONTROLLED_GATE.at(primitive.gate);

        if (primitive.parameter.has_value()) {
            const auto angle = primitive.parameter.value();
            output.emplace_back(cre::create_one_control_one_target_one_angle_gate(ctrl_gate, control, target, angle));
        } else {
            output.emplace_back(cre::create_one_control_one_target_gate(ctrl_gate, control, target));
        }
    }

    return output;
}

}  // namespace impl_ket

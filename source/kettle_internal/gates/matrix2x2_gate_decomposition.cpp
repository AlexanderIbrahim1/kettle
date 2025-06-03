#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/gates/common_u_gates.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/gates/primitive_gate_map.hpp"

#include "kettle_internal/gates/matrix2x2_gate_decomposition.hpp"

/*
    This header file contains functions for decomposing a general 2x2 unitary matrix
    to a product of primitive 2x2 quantum gates.
*/

namespace ket::internal
{

auto determinant_angle_(const ket::Matrix2X2& matrix) -> double
{
    const auto det = ket::determinant(matrix);
    return std::atan2(det.imag(), det.real());
}

auto decomp_to_single_primitive_gate_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq
) -> std::optional<Matrix2X2GateInfo_>
{
    using Info = Matrix2X2GateInfo_;
    // NOTES:
    // - the H, X, Y, and Z gates take no arguments, and so we can check them directly
    // - the RX, RY, RZ gates all share the feature that the angle can be recovered
    //   from the arccos of the real component of elem11
    // - the P gate requires information from both the real and imaginary components of elem11
    if (almost_eq(unitary, ket::h_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::H, {}};
    }
    else if (almost_eq(unitary, ket::x_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::X, {}};
    }
    else if (almost_eq(unitary, ket::y_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::Y, {}};
    }
    else if (almost_eq(unitary, ket::z_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::Z, {}};
    }
    else if (almost_eq(unitary, ket::s_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::S, {}};
    }
    else if (almost_eq(unitary, ket::sdag_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::SDAG, {}};
    }
    else if (almost_eq(unitary, ket::t_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::T, {}};
    }
    else if (almost_eq(unitary, ket::tdag_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::TDAG, {}};
    }
    if (almost_eq(unitary, ket::sx_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::SX, {}};
    }
    if (almost_eq(unitary, ket::sxdag_gate(), tolerance_sq)) {
        return Info {ket::PrimitiveGate::SXDAG, {}};
    }
    else {
        const auto real_11 = std::clamp(unitary.elem11.real(), -1.0, 1.0);
        const auto theta = std::acos(real_11);

        if (almost_eq(unitary, ket::rx_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::PrimitiveGate::RX, 2.0 * theta};
        }
        else if (almost_eq(unitary, ket::ry_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::PrimitiveGate::RY, 2.0 * theta};
        }
        else if (almost_eq(unitary, ket::rz_gate(2.0 * theta), tolerance_sq)) {
            return Info {ket::PrimitiveGate::RZ, 2.0 * theta};
        }

        const auto imag_11 = std::clamp(unitary.elem11.imag(), -1.0, 1.0);
        const auto p_theta = std::atan2(imag_11, real_11);

        if (almost_eq(unitary, ket::p_gate(p_theta), tolerance_sq)) {
            return Info {ket::PrimitiveGate::P, p_theta};
        }
    }

    return std::nullopt;
}

auto decomp_special_unitary_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq
) -> std::vector<Matrix2X2GateInfo_>
{
    const auto abs00 = std::clamp(std::abs(unitary.elem00), 0.0, 1.0);

    const auto theta = -std::acos(abs00);
    const auto lambda = -std::atan2(unitary.elem00.imag(), unitary.elem00.real());
    const auto mu = -std::atan2(unitary.elem01.imag(), unitary.elem01.real());

    auto output = std::vector<Matrix2X2GateInfo_> {};

    if (std::fabs(lambda - mu) > tolerance_sq) {
        output.emplace_back(ket::PrimitiveGate::RZ, lambda - mu);
    }

    if (std::fabs(2.0 * theta) > tolerance_sq) {
        output.emplace_back(ket::PrimitiveGate::RY, 2.0 * theta);
    }

    if (std::fabs(lambda + mu) > tolerance_sq) {
        output.emplace_back(ket::PrimitiveGate::RZ, lambda + mu);
    }

    return output;
}

auto decomp_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq
) -> std::vector<Matrix2X2GateInfo_>
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
        output.emplace_back(ket::PrimitiveGate::P, det_angle);

        return output;
    }
}

auto decomp_to_one_target_primitive_gates_(
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq
) -> std::vector<ket::PrimitiveGateInfo>
{
    namespace cre = ket::internal::create;

    const auto primitives = decomp_to_primitive_gates_(unitary, tolerance_sq);

    auto output = std::vector<ket::PrimitiveGateInfo> {};
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

auto decomp_to_one_control_one_target_primitive_gates_(
    std::size_t control,
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq
) -> std::vector<ket::PrimitiveGateInfo>
{
    namespace cre = ket::internal::create;

    const auto primitives = decomp_to_primitive_gates_(unitary, tolerance_sq);

    auto output = std::vector<ket::PrimitiveGateInfo> {};
    for (const auto& primitive : primitives) {
        const auto ctrl_gate = ket::internal::UNCONTROLLED_TO_CONTROLLED_GATE.at(primitive.gate);

        if (primitive.parameter.has_value()) {
            const auto angle = primitive.parameter.value();
            output.emplace_back(cre::create_one_control_one_target_one_angle_gate(ctrl_gate, control, target, angle));
        } else {
            output.emplace_back(cre::create_one_control_one_target_gate(ctrl_gate, control, target));
        }
    }

    return output;
}

}  // namespace ket::internal

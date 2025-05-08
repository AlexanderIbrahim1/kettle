#pragma once

#include "kettle/gates/primitive_gate.hpp"

namespace ket::internal::compare
{

constexpr inline auto GATE_ANGLE_TOLERANCE_ = double {1.0e-6};

auto is_m_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool;

auto is_1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool;

auto is_1c1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool;

auto is_1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol = GATE_ANGLE_TOLERANCE_
) -> bool;

auto is_1c1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol = GATE_ANGLE_TOLERANCE_
) -> bool;

}  // namespace ket::internal::compare

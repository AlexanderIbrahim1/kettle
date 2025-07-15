#include "kettle/gates/primitive_gate.hpp"
#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/gates/primitive_gate/gate_compare.hpp"

namespace ket::internal::compare
{

namespace create = ket::internal::create;

auto is_m_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return create::unpack_m_gate(info0) == create::unpack_m_gate(info1);
}

auto is_reset_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return create::unpack_reset_gate(info0) == create::unpack_reset_gate(info1);
}

auto is_1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return create::unpack_one_target_gate(info0) == create::unpack_one_target_gate(info1);
}

auto is_1c1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return create::unpack_one_control_one_target_gate(info0) == create::unpack_one_control_one_target_gate(info1);
}

auto is_1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol
) -> bool
{
    const auto [target0, angle0] = create::unpack_one_target_one_angle_gate(info0);
    const auto [target1, angle1] = create::unpack_one_target_one_angle_gate(info1);

    return target0 == target1 && std::fabs(angle0 - angle1) < tol;
}

auto is_1c1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol
) -> bool
{
    const auto [control0, target0, angle0] = create::unpack_one_control_one_target_one_angle_gate(info0);
    const auto [control1, target1, angle1] = create::unpack_one_control_one_target_one_angle_gate(info1);

    return control0 == control1 && target0 == target1 && std::fabs(angle0 - angle1) < tol;
}

}  // namespace ket::internal::compare

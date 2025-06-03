#include "kettle/gates/primitive_gate.hpp"
#include "kettle_internal/gates/primitive_gate/gate_id.hpp"

namespace ket::internal::gate_id
{

auto is_one_target_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return gate == G::H || \
        gate == G::X || gate == G::Y || gate == G::Z || \
        gate == G::S || gate == G::T || gate == G::SX || \
        gate == G::SDAG || gate == G::TDAG || gate == G::SXDAG;
}

auto is_one_target_one_angle_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return gate == G::RX || gate == G::RY || gate == G::RZ || gate == G::P;
}

auto is_one_control_one_target_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return gate == G::CH || \
        gate == G::CX || gate == G::CY || gate == G::CZ || \
        gate == G::CS || gate == G::CT || gate == G::CSX || \
        gate == G::CSDAG || gate == G::CTDAG || gate == G::CSXDAG;
}

auto is_one_control_one_target_one_angle_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return gate == G::CRX || gate == G::CRY || gate == G::CRZ || gate == G::CP;
}

auto is_single_qubit_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return is_one_target_transform_gate(gate) || is_one_target_one_angle_transform_gate(gate) || gate == G::U;
}

auto is_double_qubit_transform_gate(ket::PrimitiveGate gate) -> bool
{
    using G = ket::PrimitiveGate;
    return is_one_control_one_target_one_angle_transform_gate(gate) \
        || is_one_control_one_target_transform_gate(gate) \
        || gate == G::CU;
}

auto is_non_angle_transform_gate(ket::PrimitiveGate gate) -> bool
{
    return is_one_target_transform_gate(gate) || is_one_control_one_target_transform_gate(gate);
}

auto is_angle_transform_gate(ket::PrimitiveGate gate) -> bool
{
    return is_one_target_one_angle_transform_gate(gate) || is_one_control_one_target_one_angle_transform_gate(gate);
}

auto is_primitive_gate(ket::PrimitiveGate gate) -> bool
{
    return is_non_angle_transform_gate(gate) || is_angle_transform_gate(gate);
}

}  // namespace ket::internal::gate_id


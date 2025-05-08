#include "kettle/gates/primitive_gate.hpp"
#include "kettle_internal/gates/primitive_gate/gate_id.hpp"

namespace ket::internal::gate_id
{

auto is_one_target_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::H || gate == G::X || gate == G::Y || gate == G::Z || gate == G::SX;
}

auto is_one_target_one_angle_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::RX || gate == G::RY || gate == G::RZ || gate == G::P;
}

auto is_one_control_one_target_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::CH || gate == G::CX || gate == G::CY || gate == G::CZ || gate == G::CSX;
}

auto is_one_control_one_target_one_angle_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::CRX || gate == G::CRY || gate == G::CRZ || gate == G::CP;
}

auto is_single_qubit_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return is_one_target_transform_gate(gate) || is_one_target_one_angle_transform_gate(gate) || gate == G::U;
}

auto is_double_qubit_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return is_one_control_one_target_one_angle_transform_gate(gate) \
        || is_one_control_one_target_transform_gate(gate) \
        || gate == G::CU;
}

auto is_non_angle_transform_gate(ket::Gate gate) -> bool
{
    return is_one_target_transform_gate(gate) || is_one_control_one_target_transform_gate(gate);
}

auto is_angle_transform_gate(ket::Gate gate) -> bool
{
    return is_one_target_one_angle_transform_gate(gate) || is_one_control_one_target_one_angle_transform_gate(gate);
}

auto is_primitive_gate(ket::Gate gate) -> bool
{
    return is_non_angle_transform_gate(gate) || is_angle_transform_gate(gate);
}

}  // namespace ket::internal::gate_id


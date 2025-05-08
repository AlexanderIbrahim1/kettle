#pragma once

#include "kettle/gates/primitive_gate.hpp"


namespace ket::internal::gate_id
{

auto is_one_target_transform_gate(ket::Gate gate) -> bool;

auto is_one_target_one_angle_transform_gate(ket::Gate gate) -> bool;

auto is_one_control_one_target_transform_gate(ket::Gate gate) -> bool;

auto is_one_control_one_target_one_angle_transform_gate(ket::Gate gate) -> bool;

auto is_single_qubit_transform_gate(ket::Gate gate) -> bool;

auto is_double_qubit_transform_gate(ket::Gate gate) -> bool;

auto is_non_angle_transform_gate(ket::Gate gate) -> bool;

auto is_angle_transform_gate(ket::Gate gate) -> bool;

auto is_primitive_gate(ket::Gate gate) -> bool;

}  // namespace ket::internal::gate_id


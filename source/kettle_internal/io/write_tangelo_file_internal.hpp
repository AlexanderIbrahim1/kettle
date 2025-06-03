#pragma once

#include <string>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"


namespace ket::internal
{

auto format_double_(double x) -> std::string;

auto format_matrix2x2_(const ket::Matrix2X2& mat) -> std::string;

auto format_one_target_gate_(const ket::PrimitiveGateInfo& info) -> std::string;

auto format_one_control_one_target_gate_(const ket::PrimitiveGateInfo& info) -> std::string;

auto format_one_target_one_angle_gate_(const ket::PrimitiveGateInfo& info) -> std::string;

auto format_one_control_one_target_one_angle_gate_(const ket::PrimitiveGateInfo& info) -> std::string;

auto format_m_gate_(const ket::PrimitiveGateInfo& info) -> std::string;

auto format_u_gate_(const ket::PrimitiveGateInfo& info, const ket::Matrix2X2& mat) -> std::string;

auto format_cu_gate_(const ket::PrimitiveGateInfo& info, const ket::Matrix2X2& mat) -> std::string;

}  // namespace ket::internal

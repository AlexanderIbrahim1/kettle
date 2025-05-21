#pragma once

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/common/matrix2x2.hpp"

namespace ket
{

auto i_gate() noexcept -> Matrix2X2;

auto h_gate() noexcept -> Matrix2X2;

auto x_gate() noexcept -> Matrix2X2;

auto y_gate() noexcept -> Matrix2X2;

auto z_gate() noexcept -> Matrix2X2;

auto s_gate() noexcept -> Matrix2X2;

auto t_gate() noexcept -> Matrix2X2;

auto rx_gate(double angle) noexcept -> Matrix2X2;

auto ry_gate(double angle) noexcept -> Matrix2X2;

auto rz_gate(double angle) noexcept -> Matrix2X2;

auto p_gate(double angle) noexcept -> Matrix2X2;

auto sx_gate() noexcept -> Matrix2X2;

auto non_angle_gate(Gate gate) -> Matrix2X2;

auto angle_gate(Gate gate, double angle) -> Matrix2X2;

}  // namespace ket

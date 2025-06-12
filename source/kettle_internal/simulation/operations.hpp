#pragma once

#include "kettle/common/matrix2x2.hpp"
#include "kettle/state/statevector.hpp"

/*
    This header file contains the common operations performed on two states in the
    Statevector object.
*/

namespace ket::internal
{

void apply_h_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);

void apply_x_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);

void apply_y_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);

void apply_z_gate(ket::Statevector& state, std::size_t i1);

void apply_s_gate(ket::Statevector& state, std::size_t i1);

void apply_sdag_gate(ket::Statevector& state, std::size_t i1);

void apply_t_gate(ket::Statevector& state, std::size_t i1);

void apply_tdag_gate(ket::Statevector& state, std::size_t i1);

void apply_sx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);

void apply_sxdag_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);

void apply_rx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);

void apply_ry_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);

void apply_rz_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);

void apply_p_gate(ket::Statevector& state, std::size_t i1, double theta);

void apply_u_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, const ket::Matrix2X2& mat);

}  // namespace ket::internal

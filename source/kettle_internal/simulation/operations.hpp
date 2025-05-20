#pragma once

#include "kettle/common/matrix2x2.hpp"
#include "kettle/state/state.hpp"

/*
    This header file contains the common operations performed on two states in the
    QuantumState object.
*/

namespace ket::internal
{

void apply_h_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1);

void apply_x_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1);

void apply_y_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1);

void apply_z_gate(ket::QuantumState& state, std::size_t i1);

void apply_s_gate(ket::QuantumState& state, std::size_t i1);

void apply_rx_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta);

void apply_sx_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1);

void apply_ry_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta);

void apply_rz_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta);

void apply_p_gate(ket::QuantumState& state, std::size_t i1, double theta);

void apply_u_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, const ket::Matrix2X2& mat);

}  // namespace ket::internal

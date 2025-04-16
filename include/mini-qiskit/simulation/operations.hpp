#pragma once

#include <cmath>
#include <complex>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/state/state.hpp"

/*
    This header file contains the common operations performed on two states in the
    QuantumState object.
*/

namespace impl_mqis
{

constexpr void apply_x_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1)
{
    std::swap(state[i0], state[i1]);
}

constexpr void apply_y_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto real0 = state1.imag();
    const auto imag0 = -state1.real();
    const auto real1 = -state0.imag();
    const auto imag1 = state0.real();

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

constexpr void apply_z_gate(mqis::QuantumState& state, std::size_t i1)
{
    state[i1] *= -1.0;
}

constexpr void apply_h_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto real_add = M_SQRT1_2 * (state0.real() + state1.real());
    const auto imag_add = M_SQRT1_2 * (state0.imag() + state1.imag());
    const auto real_sub = M_SQRT1_2 * (state0.real() - state1.real());
    const auto imag_sub = M_SQRT1_2 * (state0.imag() - state1.imag());

    state[i0] = std::complex<double> {real_add, imag_add};
    state[i1] = std::complex<double> {real_sub, imag_sub};
}

constexpr void apply_rx_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = state0.real() * cost + state1.imag() * sint;
    const auto imag0 = state0.imag() * cost - state1.real() * sint;
    const auto real1 = state1.real() * cost + state0.imag() * sint;
    const auto imag1 = state1.imag() * cost - state0.real() * sint;

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

constexpr void apply_ry_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = state0.real() * cost - state1.real() * sint;
    const auto imag0 = state0.imag() * cost - state1.imag() * sint;
    const auto real1 = state1.real() * cost + state0.real() * sint;
    const auto imag1 = state1.imag() * cost + state0.imag() * sint;

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

constexpr void apply_rz_gate(mqis::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = state0.real() * cost + state0.imag() * sint;
    const auto imag0 = state0.imag() * cost - state0.real() * sint;
    const auto real1 = state1.real() * cost - state1.imag() * sint;
    const auto imag1 = state1.imag() * cost + state1.real() * sint;

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

constexpr void apply_p_gate(mqis::QuantumState& state, std::size_t i1, double theta)
{
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta);
    const auto sint = std::sin(theta);

    const auto real1 = state1.real() * cost - state1.imag() * sint;
    const auto imag1 = state1.imag() * cost + state1.real() * sint;

    state[i1] = std::complex<double> {real1, imag1};
}

constexpr void apply_u_gate(
    mqis::QuantumState& state,
    std::size_t i0,
    std::size_t i1,
    const mqis::Matrix2X2& mat
)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto new_state0 = state0 * mat.elem00 + state1 * mat.elem01;
    const auto new_state1 = state0 * mat.elem10 + state1 * mat.elem11;

    state[i0] = new_state0;
    state[i1] = new_state1;
}

}  // namespace impl_mqis

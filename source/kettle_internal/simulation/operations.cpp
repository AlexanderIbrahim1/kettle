#include <cmath>
#include <complex>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/state/state.hpp"

#include "kettle_internal/simulation/operations.hpp"


namespace ket::internal
{

void apply_h_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1)
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

void apply_x_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1)
{
    std::swap(state[i0], state[i1]);
}

void apply_y_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1)
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

void apply_z_gate(ket::QuantumState& state, std::size_t i1)
{
    state[i1] *= -1.0;
}

void apply_s_gate(ket::QuantumState& state, std::size_t i1)
{
    const auto state1 = state[i1];
    state[i1] = {-state1.imag(), state1.real()};
}

void apply_rx_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = (state0.real() * cost) + (state1.imag() * sint);
    const auto imag0 = (state0.imag() * cost) - (state1.real() * sint);
    const auto real1 = (state1.real() * cost) + (state0.imag() * sint);
    const auto imag1 = (state1.imag() * cost) - (state0.real() * sint);

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

void apply_sx_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto real0 = 0.5 * (  state0.real() - state0.imag() + state1.real() + state1.imag());
    const auto imag0 = 0.5 * (  state0.real() + state0.imag() - state1.real() + state1.imag());
    const auto real1 = 0.5 * (  state0.real() + state0.imag() + state1.real() - state1.imag());
    const auto imag1 = 0.5 * (- state0.real() + state0.imag() + state1.real() + state1.imag());

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

void apply_ry_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = (state0.real() * cost) - (state1.real() * sint);
    const auto imag0 = (state0.imag() * cost) - (state1.imag() * sint);
    const auto real1 = (state1.real() * cost) + (state0.real() * sint);
    const auto imag1 = (state1.imag() * cost) + (state0.imag() * sint);

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

void apply_rz_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = (state0.real() * cost) + (state0.imag() * sint);
    const auto imag0 = (state0.imag() * cost) - (state0.real() * sint);
    const auto real1 = (state1.real() * cost) - (state1.imag() * sint);
    const auto imag1 = (state1.imag() * cost) + (state1.real() * sint);

    state[i0] = std::complex<double> {real0, imag0};
    state[i1] = std::complex<double> {real1, imag1};
}

void apply_p_gate(ket::QuantumState& state, std::size_t i1, double theta)
{
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta);
    const auto sint = std::sin(theta);

    const auto real1 = (state1.real() * cost) - (state1.imag() * sint);
    const auto imag1 = (state1.imag() * cost) + (state1.real() * sint);

    state[i1] = std::complex<double> {real1, imag1};
}

void apply_u_gate(ket::QuantumState& state, std::size_t i0, std::size_t i1, const ket::Matrix2X2& mat)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto new_state0 = state0 * mat.elem00 + state1 * mat.elem01;
    const auto new_state1 = state0 * mat.elem10 + state1 * mat.elem11;

    state[i0] = new_state0;
    state[i1] = new_state1;
}

}  // namespace ket::internal

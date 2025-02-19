#pragma once

#include <cmath>

#include "mini-qiskit/common/complex.hpp"
#include "mini-qiskit/state.hpp"

/*
    This header file contains the common operations performed on two states in the
    QuantumState object.
*/

namespace impl_mqis
{

constexpr void swap_states(mqis::QuantumState& state, std::size_t i0, std::size_t i1)
{
    std::swap(state[i0], state[i1]);
}

constexpr void superpose_states(mqis::QuantumState& state, std::size_t i0, std::size_t i1)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto real_add = M_SQRT1_2 * (state0.real + state1.real);
    const auto imag_add = M_SQRT1_2 * (state0.imag + state1.imag);
    const auto real_sub = M_SQRT1_2 * (state0.real - state1.real);
    const auto imag_sub = M_SQRT1_2 * (state0.imag - state1.imag);

    state[i0] = mqis::Complex {real_add, imag_add};
    state[i1] = mqis::Complex {real_sub, imag_sub};
}

constexpr void turn_states(mqis::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = state0.real * cost + state1.imag * sint;
    const auto imag0 = state0.imag * cost - state1.real * sint;
    const auto real1 = state1.real * cost + state0.imag * sint;
    const auto imag1 = state1.imag * cost - state0.real * sint;

    state[i0] = mqis::Complex {real0, imag0};
    state[i1] = mqis::Complex {real1, imag1};
}

constexpr void phaseturn_states(mqis::QuantumState& state, std::size_t i0, std::size_t i1, double theta)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto cost = std::cos(theta / 2.0);
    const auto sint = std::sin(theta / 2.0);

    const auto real0 = state0.real * cost + state0.imag * sint;
    const auto imag0 = state0.imag * cost - state0.real * sint;
    const auto real1 = state1.real * cost - state1.imag * sint;
    const auto imag1 = state1.imag * cost + state1.real * sint;

    state[i0] = mqis::Complex {real0, imag0};
    state[i1] = mqis::Complex {real1, imag1};
}

}  // namespace impl_mqis

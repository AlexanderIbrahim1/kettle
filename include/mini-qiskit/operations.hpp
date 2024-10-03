#pragma once

#include <cmath>

#include "mini-qiskit/state.hpp"

/*
    This header file contains the common operations performed on two states in the
    QuantumState object.
*/


namespace mqis
{

constexpr void swap_states(QuantumState& state, std::size_t i0, std::size_t i1)
{
    std::swap(state[i0], state[i1]);
}

constexpr void superpose_states(QuantumState& state, std::size_t i0, std::size_t i1)
{
    const auto& state0 = state[i0];
    const auto& state1 = state[i1];

    const auto real_add = M_SQRT1_2 * (state0.real + state1.real);
    const auto imag_add = M_SQRT1_2 * (state0.imag + state1.imag);
    const auto real_sub = M_SQRT1_2 * (state0.real - state1.real);
    const auto imag_sub = M_SQRT1_2 * (state0.imag - state1.imag);

    state[i0] = Complex {real_add, imag_add};
    state[i1] = Complex {real_sub, imag_sub};
}

}  // namespace mqis

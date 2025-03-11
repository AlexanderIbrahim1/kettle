#pragma once

#include "mini-qiskit/common/matrix2x2.hpp"

namespace mqis
{

constexpr auto x_gate() noexcept -> Matrix2X2
{
    return {
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 0.0},
        {0.0, 0.0}
    };
}

constexpr auto y_gate() noexcept -> Matrix2X2
{
    return {
        {0.0, 0.0},
        {0.0, -1.0},
        {0.0, 1.0},
        {0.0, 0.0}
    };
}

constexpr auto z_gate() noexcept -> Matrix2X2
{
    return {
        {1.0, 0.0},
        {0.0, 0.0},
        {0.0, 0.0},
        {-1.0, 0.0}
    };
}

constexpr auto sqrt_x_gate() noexcept -> Matrix2X2
{
    return {
        {0.5, 0.5 },
        {0.5, -0.5},
        {0.5, -0.5},
        {0.5, 0.5 }
    };
}

constexpr auto sqrt_x_gate_conj() noexcept -> Matrix2X2
{
    return mqis::conjugate_transpose(sqrt_x_gate());
}

}  // namespace mqis

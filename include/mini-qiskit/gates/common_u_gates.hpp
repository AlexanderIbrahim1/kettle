#pragma once

#include "mini-qiskit/common/matrix2x2.hpp"

namespace mqis
{

constexpr auto h_gate() noexcept -> Matrix2X2
{
    return {
        {M_SQRT1_2, 0.0},
        {M_SQRT1_2, 0.0},
        {M_SQRT1_2, 0.0},
        {-M_SQRT1_2, 0.0}
    };
}

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

constexpr auto rx_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        {cost, 0.0},
        {0.0, -sint},
        {0.0, -sint},
        {cost, 0.0}
    };
}

constexpr auto ry_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        {cost, 0.0},
        {-sint, 0.0},
        {sint, 0.0},
        {cost, 0.0}
    };
}

constexpr auto rz_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        {cost, -sint},
        {0.0, 0.0},
        {0.0, 0.0},
        {cost, sint}
    };
}

constexpr auto p_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle);
    const auto sint = std::sin(angle);

    return {
        {1.0, 0.0},
        {0.0, 0.0},
        {0.0, 0.0},
        {cost, sint}
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

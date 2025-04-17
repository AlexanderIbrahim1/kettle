#pragma once

#include "mini-qiskit/gates/primitive_gate.hpp"
#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"

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

constexpr auto non_angle_gate(Gate gate) -> Matrix2X2
{
    if (gate == Gate::H) {
        return h_gate();
    }
    else if (gate == Gate::X || gate == Gate::CX) {
        return x_gate();
    }
    else if (gate == Gate::Y || gate == Gate::CY) {
        return y_gate();
    }
    else if (gate == Gate::Z || gate == Gate::CZ) {
        return z_gate();
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid non-angle gate provided\n"};
    }
}

constexpr auto angle_gate(Gate gate, double angle) -> Matrix2X2
{
    if (gate == Gate::RX || gate == Gate::CRX) {
        return rx_gate(angle);
    }
    else if (gate == Gate::RY || gate == Gate::CRY) {
        return ry_gate(angle);
    }
    else if (gate == Gate::RZ || gate == Gate::CRZ) {
        return rz_gate(angle);
    }
    else if (gate == Gate::P || gate == Gate::CP) {
        return p_gate(angle);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid angle gate provided\n"};
    }
}

}  // namespace mqis

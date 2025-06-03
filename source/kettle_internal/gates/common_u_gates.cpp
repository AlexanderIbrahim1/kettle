#include "kettle/gates/primitive_gate.hpp"
#include "kettle/common/matrix2x2.hpp"
#include <cmath>

#include "kettle/gates/common_u_gates.hpp"

namespace ket
{

auto i_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={1.0, 0.0}
    };
}

auto h_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={M_SQRT1_2, 0.0},
        .elem01={M_SQRT1_2, 0.0},
        .elem10={M_SQRT1_2, 0.0},
        .elem11={-M_SQRT1_2, 0.0}
    };
}

auto x_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={0.0, 0.0},
        .elem01={1.0, 0.0},
        .elem10={1.0, 0.0},
        .elem11={0.0, 0.0}
    };
}

auto y_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={0.0, 0.0},
        .elem01={0.0, -1.0},
        .elem10={0.0, 1.0},
        .elem11={0.0, 0.0}
    };
}

auto z_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={-1.0, 0.0}
    };
}

auto s_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={0.0, 1.0}
    };
}

auto sdag_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={0.0, -1.0}
    };
}

auto t_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={M_SQRT1_2, M_SQRT1_2}
    };
}

auto tdag_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={M_SQRT1_2, -M_SQRT1_2}
    };
}

auto sx_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={0.5, 0.5},
        .elem01={0.5, -0.5},
        .elem10={0.5, -0.5},
        .elem11={0.5, 0.5}
    };
}

auto sxdag_gate() noexcept -> Matrix2X2
{
    return {
        .elem00={0.5, -0.5},
        .elem01={0.5, 0.5},
        .elem10={0.5, 0.5},
        .elem11={0.5, -0.5}
    };
}

auto rx_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        .elem00={cost, 0.0},
        .elem01={0.0, -sint},
        .elem10={0.0, -sint},
        .elem11={cost, 0.0}
    };
}

auto ry_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        .elem00={cost, 0.0},
        .elem01={-sint, 0.0},
        .elem10={sint, 0.0},
        .elem11={cost, 0.0}
    };
}

auto rz_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle / 2.0);
    const auto sint = std::sin(angle / 2.0);

    return {
        .elem00={cost, -sint},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={cost, sint}
    };
}

auto p_gate(double angle) noexcept -> Matrix2X2
{
    const auto cost = std::cos(angle);
    const auto sint = std::sin(angle);

    return {
        .elem00={1.0, 0.0},
        .elem01={0.0, 0.0},
        .elem10={0.0, 0.0},
        .elem11={cost, sint}
    };
}

auto non_angle_gate(PrimitiveGate gate) -> Matrix2X2
{
    if (gate == PrimitiveGate::H || gate == PrimitiveGate::CH) {
        return h_gate();
    }
    else if (gate == PrimitiveGate::X || gate == PrimitiveGate::CX) {
        return x_gate();
    }
    else if (gate == PrimitiveGate::Y || gate == PrimitiveGate::CY) {
        return y_gate();
    }
    else if (gate == PrimitiveGate::Z || gate == PrimitiveGate::CZ) {
        return z_gate();
    }
    else if (gate == PrimitiveGate::S || gate == PrimitiveGate::CS) {
        return s_gate();
    }
    else if (gate == PrimitiveGate::SDAG || gate == PrimitiveGate::CSDAG) {
        return sdag_gate();
    }
    else if (gate == PrimitiveGate::T || gate == PrimitiveGate::CT) {
        return t_gate();
    }
    else if (gate == PrimitiveGate::TDAG || gate == PrimitiveGate::CTDAG) {
        return tdag_gate();
    }
    else if (gate == PrimitiveGate::SX || gate == PrimitiveGate::CSX) {
        return sx_gate();
    }
    else if (gate == PrimitiveGate::SXDAG || gate == PrimitiveGate::CSXDAG) {
        return sxdag_gate();
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid non-angle gate provided\n"};
    }
}

auto angle_gate(PrimitiveGate gate, double angle) -> Matrix2X2
{
    if (gate == PrimitiveGate::RX || gate == PrimitiveGate::CRX) {
        return rx_gate(angle);
    }
    else if (gate == PrimitiveGate::RY || gate == PrimitiveGate::CRY) {
        return ry_gate(angle);
    }
    else if (gate == PrimitiveGate::RZ || gate == PrimitiveGate::CRZ) {
        return rz_gate(angle);
    }
    else if (gate == PrimitiveGate::P || gate == PrimitiveGate::CP) {
        return p_gate(angle);
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error, invalid angle gate provided\n"};
    }
}

}  // namespace ket

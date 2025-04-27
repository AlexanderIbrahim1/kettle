#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace ket
{

enum class Gate : std::uint8_t
{
    H,
    X,
    Y,
    Z,
    SX,
    RX,
    RY,
    RZ,
    P,
    CH,
    CX,
    CY,
    CZ,
    CSX,
    CRX,
    CRY,
    CRZ,
    CP,
    U,
    CU,
    M
};

/*
    Each gate in the reference specification can have either 1, 2, or 3 arguments. This implementation
    forces the type that carries the information for each gate to hold enough data for the largest
    possible number of arguments (3 in this case).

    This wastes a fair bit of memory.

    A more memory-considerate implementation might treat the information needed for each gate simply
    as a collection of bytes, and use an opcode to determine how many arguments are required.

    But this implementation is not concerned with that.
*/
struct GateInfo
{
    Gate gate;
    std::size_t arg0;
    std::size_t arg1;
    double arg2;
    std::size_t arg3;
};

}  // namespace ket


namespace impl_ket::gate_id
{

constexpr auto is_one_target_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::H || gate == G::X || gate == G::Y || gate == G::Z || gate == G::SX;
}

constexpr auto is_one_target_one_angle_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::RX || gate == G::RY || gate == G::RZ || gate == G::P;
}

constexpr auto is_one_control_one_target_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::CH || gate == G::CX || gate == G::CY || gate == G::CZ || gate == G::CSX;
}

constexpr auto is_one_control_one_target_one_angle_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return gate == G::CRX || gate == G::CRY || gate == G::CRZ || gate == G::CP;
}

constexpr auto is_single_qubit_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return is_one_target_transform_gate(gate) || is_one_target_one_angle_transform_gate(gate) || gate == G::U;
}

constexpr auto is_double_qubit_transform_gate(ket::Gate gate) -> bool
{
    using G = ket::Gate;
    return is_one_control_one_target_one_angle_transform_gate(gate) \
        || is_one_control_one_target_transform_gate(gate) \
        || gate == G::CU;
}

constexpr auto is_non_angle_transform_gate(ket::Gate gate) -> bool
{
    return is_one_target_transform_gate(gate) || is_one_control_one_target_transform_gate(gate);
}

constexpr auto is_angle_transform_gate(ket::Gate gate) -> bool
{
    return is_one_target_one_angle_transform_gate(gate) || is_one_control_one_target_one_angle_transform_gate(gate);
}

}  // namespace impl_ket::gate_id


namespace impl_ket
{
/*
    Parameters indicating to the developer that a given gate does not use a certain data member in
    a ket::GateInfo instance.
*/
constexpr static auto DUMMY_ARG1 = std::size_t {0};
constexpr static auto DUMMY_ARG2 = double {0.0};
constexpr static auto DUMMY_ARG3 = std::size_t {0};

constexpr auto create_one_target_gate(ket::Gate gate, std::size_t target_index) -> ket::GateInfo
{
    if (!gate_id::is_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .arg3=DUMMY_ARG3};
}

constexpr auto unpack_one_target_gate(const ket::GateInfo& info) -> std::size_t
{
    return info.arg0;  // target_index
}

constexpr auto create_one_target_one_angle_gate(ket::Gate gate, std::size_t target_index, double theta) -> ket::GateInfo
{
    if (!gate_id::is_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=theta, .arg3=DUMMY_ARG3};
}

constexpr auto unpack_one_target_one_angle_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, double>
{
    return {info.arg0, info.arg2};  // target_index, angle
}

constexpr auto create_one_control_one_target_gate(ket::Gate gate, std::size_t control_index, std::size_t target_index) -> ket::GateInfo
{
    if (!gate_id::is_one_control_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .arg3=DUMMY_ARG3};
}

constexpr auto unpack_one_control_one_target_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control_index, target_index
}

constexpr auto create_one_control_one_target_one_angle_gate(ket::Gate gate, std::size_t control_index, std::size_t target_index, double theta) -> ket::GateInfo
{
    if (!gate_id::is_one_control_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=theta, .arg3=DUMMY_ARG3};
}

constexpr auto unpack_one_control_one_target_one_angle_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t, double>
{
    return {info.arg0, info.arg1, info.arg2};  // control_index, target_index, angle
}

/* Apply the U-gate, with the 2x2 matrix identified by `matrix_index` to the qubit at index `target_index` */
constexpr auto create_u_gate(std::size_t target_index, std::size_t matrix_index) -> ket::GateInfo
{
    return {.gate=ket::Gate::U, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .arg3=matrix_index};
}

/* Parse the relevant information for the U-gate */
constexpr auto unpack_u_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg3};  // target_index, matrix_index
}

/* Apply CU-gate, with the 2x2 matrix identified by `matrix_index` to qubits at the `control_index` and `target_index` */
constexpr auto create_cu_gate(std::size_t control_index, std::size_t target_index, std::size_t matrix_index)
    -> ket::GateInfo
{
    return {.gate=ket::Gate::CU, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .arg3=matrix_index};
}

/* Parse the relevant information for the CU-gate */
constexpr auto unpack_cu_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t, std::size_t>
{
    return {info.arg0, info.arg1, info.arg3};  // control index, target index, matrix index
}

/* Apply a measurement gate to a given qubit and bit */
constexpr auto create_m_gate(std::size_t qubit_index, std::size_t bit_index) -> ket::GateInfo
{
    return {.gate=ket::Gate::M, .arg0=qubit_index, .arg1=bit_index, .arg2=DUMMY_ARG2, .arg3=DUMMY_ARG3};
}

/* Parse the relevant information for the M-gate */
constexpr auto unpack_m_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // qubit index, bit index
}

constexpr auto unpack_single_qubit_gate_index(const ket::GateInfo& info) -> std::size_t
{
    return info.arg0;  // target_index
}

constexpr auto unpack_double_qubit_gate_indices(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control_index, target_index
}

constexpr auto unpack_gate_angle(const ket::GateInfo& info) -> double
{
    return info.arg2;  // angle
}

constexpr auto unpack_gate_matrix_index(const ket::GateInfo& info) -> std::size_t
{
    return info.arg3;  // matrix_index
}

}  // namespace impl_ket

namespace impl_ket::compare
{

inline constexpr auto GATE_ANGLE_TOLERANCE_ = double {1.0e-6};

constexpr auto is_m_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_m_gate(info0) == unpack_m_gate(info1);
}

constexpr auto is_1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_one_target_gate(info0) == unpack_one_target_gate(info1);
}

constexpr auto is_1c1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_one_control_one_target_gate(info0) == unpack_one_control_one_target_gate(info1);
}

constexpr auto is_1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol = GATE_ANGLE_TOLERANCE_
) -> bool
{
    const auto [target0, angle0] = unpack_one_target_one_angle_gate(info0);
    const auto [target1, angle1] = unpack_one_target_one_angle_gate(info1);

    return target0 == target1 && std::fabs(angle0 - angle1) < tol;
}

constexpr auto is_1c1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol = GATE_ANGLE_TOLERANCE_
) -> bool
{
    const auto [control0, target0, angle0] = unpack_one_control_one_target_one_angle_gate(info0);
    const auto [control1, target1, angle1] = unpack_one_control_one_target_one_angle_gate(info1);

    return control0 == control1 && target0 == target1 && std::fabs(angle0 - angle1) < tol;
}

}  // namespace impl_ket::compare

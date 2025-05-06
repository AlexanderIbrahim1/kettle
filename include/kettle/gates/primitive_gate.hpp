#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <tuple>

#include "kettle/common/matrix2x2.hpp"
#include "kettle_internal/common/clone_ptr.hpp"

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
    The `GateInfo` type holds all the information needed to describe any of the primitive gates in
    the project's specification. This implementation forces the gate to carry the information needed
    for every possible type of gate, even if some parameters are not used. For example, the `GateInfo`
    instance for an X-gate only needs the target index (one `std::size_t`), but it still holds all the
    other parameters.

    Each of the primitive gates can have up to two index arguments:
      - a target qubit index
      - possibly a control qubit index
      - in the case of measurement gates, a qubit index and a classical bit index

    Some of the primitive gates can have one real parameter, an angle.

    The U and CU primitive gates can hold a pointer to a unitary 2x2 matrix.

*/
struct GateInfo
{
    Gate gate;
    std::size_t arg0;
    std::size_t arg1;
    double arg2;
    ket::internal::ClonePtr<Matrix2X2> unitary_ptr;
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

    Note that DUMMY_ARG3 cannot be constexpr, because ClonePtr holds `std::unique_ptr<T>`, which is
    not constexpr in C++20 (only in C++23).
*/
constexpr static auto DUMMY_ARG1 = std::size_t {0};
constexpr static auto DUMMY_ARG2 = double {0.0};
const static auto DUMMY_ARG3 = ket::internal::ClonePtr<ket::Matrix2X2> {nullptr};

/*
    Create a single-qubit gate with no parameters.
*/
inline auto create_one_target_gate(ket::Gate gate, std::size_t target_index) -> ket::GateInfo
{
    if (!gate_id::is_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3};
}

/*
    Returns the `target_qubit` of a single-qubit gate with no parameters.
*/
inline auto unpack_one_target_gate(const ket::GateInfo& info) -> std::size_t
{
    return info.arg0;  // target index
}

/*
    Create a single-qubit gate with an angle parameter.
*/
inline auto create_one_target_one_angle_gate(ket::Gate gate, std::size_t target_index, double theta) -> ket::GateInfo
{
    if (!gate_id::is_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=theta, .unitary_ptr=DUMMY_ARG3};
}

/*
    Returns the `{target_qubit, angle}` of a single-qubit gate with an angle parameter.
*/
inline auto unpack_one_target_one_angle_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, double>
{
    return {info.arg0, info.arg2};  // target index, angle
}

/*
    Create a controlled gate with no parameters.
*/
inline auto create_one_control_one_target_gate(ket::Gate gate, std::size_t control_index, std::size_t target_index) -> ket::GateInfo
{
    if (!gate_id::is_one_control_one_target_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3};
}

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate with no parameters.
*/
inline auto unpack_one_control_one_target_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control index, target_index
}

/*
    Create a controlled gate with an angle parameter.
*/
inline auto create_one_control_one_target_one_angle_gate(ket::Gate gate, std::size_t control_index, std::size_t target_index, double theta) -> ket::GateInfo
{
    if (!gate_id::is_one_control_one_target_one_angle_transform_gate(gate)) {
        throw std::runtime_error {"DEV ERROR: invalid one-control-one-target-one-angle gate provided.\n"};
    }

    return {.gate=gate, .arg0=control_index, .arg1=target_index, .arg2=theta, .unitary_ptr=DUMMY_ARG3};
}

/*
    Returns the `{control_qubit, target_qubit, angle}` of a double-qubit gate with an angle parameter.
*/
inline auto unpack_one_control_one_target_one_angle_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t, double>
{
    return {info.arg0, info.arg1, info.arg2};  // control index, target index, angle
}

/*
    Create a U-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`.
*/
inline auto create_u_gate(std::size_t target_index, ket::internal::ClonePtr<ket::Matrix2X2> unitary) -> ket::GateInfo
{
    return {.gate=ket::Gate::U, .arg0=target_index, .arg1=DUMMY_ARG1, .arg2=DUMMY_ARG2, .unitary_ptr=std::move(unitary)};
}

/*
    Returns the `{target_qubit, unitary_ptr}` of a U-gate.
*/
inline auto unpack_u_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, const ket::internal::ClonePtr<ket::Matrix2X2>&>
{
    return {info.arg0, info.unitary_ptr};  // target index, unitary_ptr
}

/*
    Create a CU-gate, which applies the 2x2 unitary matrix `unitary` to the qubit at index `target_index`,
    controlled by the qubit at index `control_index`.
*/
inline auto create_cu_gate(std::size_t control_index, std::size_t target_index, ket::internal::ClonePtr<ket::Matrix2X2> unitary) -> ket::GateInfo
{
    return {.gate=ket::Gate::CU, .arg0=control_index, .arg1=target_index, .arg2=DUMMY_ARG2, .unitary_ptr=std::move(unitary)};
}

/*
    Returns the `{control_qubit, target_qubit, unitary_ptr}` of a CU-gate.
*/
inline auto unpack_cu_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t, const ket::internal::ClonePtr<ket::Matrix2X2>&>
{
    return {info.arg0, info.arg1, info.unitary_ptr};  // control index, target index, unitary_ptr
}

/*
    Create an M-gate, which measures the qubit at `qubit_index`, and stores the result at `bit_index`.
*/
inline auto create_m_gate(std::size_t qubit_index, std::size_t bit_index) -> ket::GateInfo
{
    return {.gate=ket::Gate::M, .arg0=qubit_index, .arg1=bit_index, .arg2=DUMMY_ARG2, .unitary_ptr=DUMMY_ARG3};
}

/*
    Returns the `{qubit_index, bit_index}` of an M-gate.
*/
inline auto unpack_m_gate(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // qubit index, bit index
}

/*
    Returns the `target_qubit` of a single-qubit gate, with or without parameters.
*/
inline auto unpack_single_qubit_gate_index(const ket::GateInfo& info) -> std::size_t
{
    return info.arg0;  // target_index
}

/*
    Returns the `{control_qubit, target_qubit}` of a double-qubit gate, with or without parameters.
*/
inline auto unpack_double_qubit_gate_indices(const ket::GateInfo& info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // control_index, target_index
}

/*
    Returns the `angle` of a single-qubit gate or double-qubit gate, as long as it is parameterized.
*/
inline auto unpack_gate_angle(const ket::GateInfo& info) -> double
{
    return info.arg2;  // angle
}

/*
    Returns the `unitary_ptr` of a U-gate or CU-gate.
*/
inline auto unpack_unitary_matrix(const ket::GateInfo& info) -> const ket::internal::ClonePtr<ket::Matrix2X2>&
{
    return info.unitary_ptr;  // unitary_ptr
}

}  // namespace impl_ket


namespace impl_ket::compare
{

static constexpr auto GATE_ANGLE_TOLERANCE_ = double {1.0e-6};

inline auto is_m_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_m_gate(info0) == unpack_m_gate(info1);
}

inline auto is_1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_one_target_gate(info0) == unpack_one_target_gate(info1);
}

inline auto is_1c1t_gate_equal(const ket::GateInfo& info0, const ket::GateInfo& info1) -> bool
{
    return unpack_one_control_one_target_gate(info0) == unpack_one_control_one_target_gate(info1);
}

inline auto is_1t1a_gate_equal(
    const ket::GateInfo& info0,
    const ket::GateInfo& info1,
    double tol = GATE_ANGLE_TOLERANCE_
) -> bool
{
    const auto [target0, angle0] = unpack_one_target_one_angle_gate(info0);
    const auto [target1, angle1] = unpack_one_target_one_angle_gate(info1);

    return target0 == target1 && std::fabs(angle0 - angle1) < tol;
}

inline auto is_1c1t1a_gate_equal(
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

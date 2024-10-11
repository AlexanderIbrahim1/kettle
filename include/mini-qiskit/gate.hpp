#pragma once

#include <cstddef>
#include <tuple>


namespace impl_mqis
{

/*
    Parameters indicating to the developer that a given gate does not use a certain data member in
    a GateInfo instance.
*/
constexpr static auto DUMMY_ARG0 = std::size_t {0};
constexpr static auto DUMMY_ARG1 = std::size_t {0};
constexpr static auto DUMMY_ARG2 = double {0.0};

}  // namespace impl_mqis

namespace mqis
{

enum class Gate
{
    X,
    RX,
    H,
    CX,
    CRX,
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
};

/* Apply the X-gate to the qubit at index `qubit_index` */
constexpr auto create_x_gate(std::size_t qubit_index) -> GateInfo
{
    return {Gate::X, qubit_index, impl_mqis::DUMMY_ARG1, impl_mqis::DUMMY_ARG2};
}

/* Parse the relevant information for the X-gate */
constexpr auto unpack_x_gate(GateInfo info) -> std::size_t
{
    return info.arg0;  // qubit index
}

/* Apply the RX-gate with a rotation `theta` to the qubit at index `qubit_index` */
constexpr auto create_rx_gate(double theta, std::size_t qubit_index) -> GateInfo
{
    return {Gate::RX, qubit_index, impl_mqis::DUMMY_ARG1, theta};
}

/* Parse the relevant information for the RX-gate */
constexpr auto unpack_rx_gate(GateInfo info) -> std::tuple<double, std::size_t>
{
    return {info.arg2, info.arg0};  // theta, qubit index
}

/* Apply the H-gate to the qubit at index `qubit_index` */
constexpr auto create_h_gate(std::size_t qubit_index) -> GateInfo
{
    return {Gate::H, qubit_index, impl_mqis::DUMMY_ARG1, impl_mqis::DUMMY_ARG2};
}

/* Parse the relevant information for the H-gate */
constexpr auto unpack_h_gate(GateInfo info) -> std::size_t
{
    return info.arg0;  // qubit index
}

/* Apply the CX-gate to qubits at the `source_index` and `target_index` */
constexpr auto create_cx_gate(std::size_t source_index, std::size_t target_index) -> GateInfo
{
    return {Gate::CX, source_index, target_index, impl_mqis::DUMMY_ARG2};
}

/* Parse the relevant information for the CX-gate */
constexpr auto unpack_cx_gate(GateInfo info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // source index, target index
}

/* Apply the CRX-gate with a rotation `theta` to qubits at the `source_index` and `target_index` */
constexpr auto create_crx_gate(std::size_t source_index, std::size_t target_index, double theta) -> GateInfo
{
    return {Gate::CRX, source_index, target_index, theta};
}

/* Parse the relevant information for the CRX-gate */
constexpr auto unpack_crx_gate(GateInfo info) -> std::tuple<std::size_t, std::size_t, double>
{
    return {info.arg0, info.arg1, info.arg2};  // source index, target index, theta
}

/* Apply a measurement gate to a given qubit and bit */
constexpr auto create_m_gate(std::size_t qubit_index, std::size_t bit_index) -> GateInfo
{
    return {Gate::M, qubit_index, bit_index, impl_mqis::DUMMY_ARG2};
}

/* Parse the relevant information for the M-gate */
constexpr auto unpack_m_gate(GateInfo info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};  // qubit index, bit index
}

constexpr auto unpack_single_qubit_gate_index(GateInfo info) -> std::size_t
{
    return info.arg0;
}

constexpr auto unpack_double_qubit_gate_indices(GateInfo info) -> std::tuple<std::size_t, std::size_t>
{
    return {info.arg0, info.arg1};
}

constexpr auto unpack_rx_gate_angle(GateInfo info) -> double
{
    return info.arg2;
}

constexpr auto unpack_crx_gate_angle(GateInfo info) -> double
{
    return info.arg2;
}

}  // namespace mqis

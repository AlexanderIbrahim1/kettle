#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>

#include "mini-qiskit/common/linear_bijective_map.hpp"
#include "mini-qiskit/gates/primitive_gate.hpp"
#include "mini-qiskit/circuit/circuit.hpp"


namespace impl_mqis
{

static constexpr auto UNCONTROLLED_TO_CONTROLLED_GATE = LinearBijectiveMap<mqis::Gate, mqis::Gate, 10> {
    std::pair {mqis::Gate::H, mqis::Gate::CH},
    std::pair {mqis::Gate::X, mqis::Gate::CX},
    std::pair {mqis::Gate::Y, mqis::Gate::CY},
    std::pair {mqis::Gate::Z, mqis::Gate::CZ},
    std::pair {mqis::Gate::SX, mqis::Gate::CSX},
    std::pair {mqis::Gate::RX, mqis::Gate::CRX},
    std::pair {mqis::Gate::RY, mqis::Gate::CRY},
    std::pair {mqis::Gate::RZ, mqis::Gate::CRZ},
    std::pair {mqis::Gate::P, mqis::Gate::CP},
    std::pair {mqis::Gate::U, mqis::Gate::CU}
};

static constexpr auto PRIMITIVE_GATES_TO_STRING = LinearBijectiveMap<mqis::Gate, std::string, 22>
{
    std::pair {mqis::Gate::H, "H"},
    std::pair {mqis::Gate::X, "X"},
    std::pair {mqis::Gate::Y, "Y"},
    std::pair {mqis::Gate::Z, "Z"},
    std::pair {mqis::Gate::SX, "SX"},
    std::pair {mqis::Gate::RX, "RX"},
    std::pair {mqis::Gate::RY, "RY"},
    std::pair {mqis::Gate::RZ, "RZ"},
    std::pair {mqis::Gate::P, "P"},
    std::pair {mqis::Gate::CH, "CH"},
    std::pair {mqis::Gate::CX, "CX"},
    std::pair {mqis::Gate::CY, "CY"},
    std::pair {mqis::Gate::CZ, "CZ"},
    std::pair {mqis::Gate::CSX, "CSX"},
    std::pair {mqis::Gate::CRX, "CRX"},
    std::pair {mqis::Gate::CRY, "CRY"},
    std::pair {mqis::Gate::CRZ, "CRZ"},
    std::pair {mqis::Gate::CP, "CP"},
    std::pair {mqis::Gate::U, "U"},
    std::pair {mqis::Gate::CU, "CU"},
    std::pair {mqis::Gate::M, "M"},
    std::pair {mqis::Gate::CONTROL, "CONTROL"}
};

using GateFuncPtr1T = void(mqis::QuantumCircuit::*)(std::size_t);
using GateFuncPtr1T1A = void(mqis::QuantumCircuit::*)(std::size_t, double);
using GateFuncPtr1C1T = void(mqis::QuantumCircuit::*)(std::size_t, std::size_t);
using GateFuncPtr1C1T1A = void(mqis::QuantumCircuit::*)(std::size_t, std::size_t, double);

static auto GATE_TO_FUNCTION_1T = LinearBijectiveMap<mqis::Gate, GateFuncPtr1T, 5>
{
    std::pair {mqis::Gate::H, static_cast<GateFuncPtr1T>(&mqis::QuantumCircuit::add_h_gate)},
    std::pair {mqis::Gate::X, static_cast<GateFuncPtr1T>(&mqis::QuantumCircuit::add_x_gate)},
    std::pair {mqis::Gate::Y, static_cast<GateFuncPtr1T>(&mqis::QuantumCircuit::add_y_gate)},
    std::pair {mqis::Gate::Z, static_cast<GateFuncPtr1T>(&mqis::QuantumCircuit::add_z_gate)},
    std::pair {mqis::Gate::SX, static_cast<GateFuncPtr1T>(&mqis::QuantumCircuit::add_sx_gate)}
};

static auto GATE_TO_FUNCTION_1T1A = LinearBijectiveMap<mqis::Gate, GateFuncPtr1T1A, 4>
{
    std::pair {mqis::Gate::RX, static_cast<GateFuncPtr1T1A>(&mqis::QuantumCircuit::add_rx_gate)},
    std::pair {mqis::Gate::RY, static_cast<GateFuncPtr1T1A>(&mqis::QuantumCircuit::add_ry_gate)},
    std::pair {mqis::Gate::RZ, static_cast<GateFuncPtr1T1A>(&mqis::QuantumCircuit::add_rz_gate)},
    std::pair {mqis::Gate::P, static_cast<GateFuncPtr1T1A>(&mqis::QuantumCircuit::add_p_gate)},
};

static auto GATE_TO_FUNCTION_1C1T = LinearBijectiveMap<mqis::Gate, GateFuncPtr1C1T, 5>
{
    std::pair {mqis::Gate::CH, static_cast<GateFuncPtr1C1T>(&mqis::QuantumCircuit::add_ch_gate)},
    std::pair {mqis::Gate::CX, static_cast<GateFuncPtr1C1T>(&mqis::QuantumCircuit::add_cx_gate)},
    std::pair {mqis::Gate::CY, static_cast<GateFuncPtr1C1T>(&mqis::QuantumCircuit::add_cy_gate)},
    std::pair {mqis::Gate::CZ, static_cast<GateFuncPtr1C1T>(&mqis::QuantumCircuit::add_cz_gate)},
    std::pair {mqis::Gate::CSX, static_cast<GateFuncPtr1C1T>(&mqis::QuantumCircuit::add_csx_gate)}
};

static auto GATE_TO_FUNCTION_1C1T1A = LinearBijectiveMap<mqis::Gate, GateFuncPtr1C1T1A, 4>
{
    std::pair {mqis::Gate::CRX, static_cast<GateFuncPtr1C1T1A>(&mqis::QuantumCircuit::add_crx_gate)},
    std::pair {mqis::Gate::CRY, static_cast<GateFuncPtr1C1T1A>(&mqis::QuantumCircuit::add_cry_gate)},
    std::pair {mqis::Gate::CRZ, static_cast<GateFuncPtr1C1T1A>(&mqis::QuantumCircuit::add_crz_gate)},
    std::pair {mqis::Gate::CP, static_cast<GateFuncPtr1C1T1A>(&mqis::QuantumCircuit::add_cp_gate)},
};



}  // namespace impl_mqis

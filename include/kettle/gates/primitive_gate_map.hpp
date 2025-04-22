#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>

#include "kettle/common/linear_bijective_map.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"


namespace impl_ket
{

static constexpr auto UNCONTROLLED_TO_CONTROLLED_GATE = LinearBijectiveMap<ket::Gate, ket::Gate, 10> {
    std::pair {ket::Gate::H, ket::Gate::CH},
    std::pair {ket::Gate::X, ket::Gate::CX},
    std::pair {ket::Gate::Y, ket::Gate::CY},
    std::pair {ket::Gate::Z, ket::Gate::CZ},
    std::pair {ket::Gate::SX, ket::Gate::CSX},
    std::pair {ket::Gate::RX, ket::Gate::CRX},
    std::pair {ket::Gate::RY, ket::Gate::CRY},
    std::pair {ket::Gate::RZ, ket::Gate::CRZ},
    std::pair {ket::Gate::P, ket::Gate::CP},
    std::pair {ket::Gate::U, ket::Gate::CU}
};

static constexpr auto PRIMITIVE_GATES_TO_STRING = LinearBijectiveMap<ket::Gate, std::string, 22>
{
    std::pair {ket::Gate::H, "H"},
    std::pair {ket::Gate::X, "X"},
    std::pair {ket::Gate::Y, "Y"},
    std::pair {ket::Gate::Z, "Z"},
    std::pair {ket::Gate::SX, "SX"},
    std::pair {ket::Gate::RX, "RX"},
    std::pair {ket::Gate::RY, "RY"},
    std::pair {ket::Gate::RZ, "RZ"},
    std::pair {ket::Gate::P, "P"},
    std::pair {ket::Gate::CH, "CH"},
    std::pair {ket::Gate::CX, "CX"},
    std::pair {ket::Gate::CY, "CY"},
    std::pair {ket::Gate::CZ, "CZ"},
    std::pair {ket::Gate::CSX, "CSX"},
    std::pair {ket::Gate::CRX, "CRX"},
    std::pair {ket::Gate::CRY, "CRY"},
    std::pair {ket::Gate::CRZ, "CRZ"},
    std::pair {ket::Gate::CP, "CP"},
    std::pair {ket::Gate::U, "U"},
    std::pair {ket::Gate::CU, "CU"},
    std::pair {ket::Gate::M, "M"},
    std::pair {ket::Gate::CONTROL, "CONTROL"}
};

using GateFuncPtr1T = void(ket::QuantumCircuit::*)(std::size_t);
using GateFuncPtr1T1A = void(ket::QuantumCircuit::*)(std::size_t, double);
using GateFuncPtr1C1T = void(ket::QuantumCircuit::*)(std::size_t, std::size_t);
using GateFuncPtr1C1T1A = void(ket::QuantumCircuit::*)(std::size_t, std::size_t, double);

static auto GATE_TO_FUNCTION_1T = LinearBijectiveMap<ket::Gate, GateFuncPtr1T, 5>
{
    std::pair {ket::Gate::H, static_cast<GateFuncPtr1T>(&ket::QuantumCircuit::add_h_gate)},
    std::pair {ket::Gate::X, static_cast<GateFuncPtr1T>(&ket::QuantumCircuit::add_x_gate)},
    std::pair {ket::Gate::Y, static_cast<GateFuncPtr1T>(&ket::QuantumCircuit::add_y_gate)},
    std::pair {ket::Gate::Z, static_cast<GateFuncPtr1T>(&ket::QuantumCircuit::add_z_gate)},
    std::pair {ket::Gate::SX, static_cast<GateFuncPtr1T>(&ket::QuantumCircuit::add_sx_gate)}
};

static auto GATE_TO_FUNCTION_1T1A = LinearBijectiveMap<ket::Gate, GateFuncPtr1T1A, 4>
{
    std::pair {ket::Gate::RX, static_cast<GateFuncPtr1T1A>(&ket::QuantumCircuit::add_rx_gate)},
    std::pair {ket::Gate::RY, static_cast<GateFuncPtr1T1A>(&ket::QuantumCircuit::add_ry_gate)},
    std::pair {ket::Gate::RZ, static_cast<GateFuncPtr1T1A>(&ket::QuantumCircuit::add_rz_gate)},
    std::pair {ket::Gate::P, static_cast<GateFuncPtr1T1A>(&ket::QuantumCircuit::add_p_gate)},
};

static auto GATE_TO_FUNCTION_1C1T = LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T, 5>
{
    std::pair {ket::Gate::CH, static_cast<GateFuncPtr1C1T>(&ket::QuantumCircuit::add_ch_gate)},
    std::pair {ket::Gate::CX, static_cast<GateFuncPtr1C1T>(&ket::QuantumCircuit::add_cx_gate)},
    std::pair {ket::Gate::CY, static_cast<GateFuncPtr1C1T>(&ket::QuantumCircuit::add_cy_gate)},
    std::pair {ket::Gate::CZ, static_cast<GateFuncPtr1C1T>(&ket::QuantumCircuit::add_cz_gate)},
    std::pair {ket::Gate::CSX, static_cast<GateFuncPtr1C1T>(&ket::QuantumCircuit::add_csx_gate)}
};

static auto GATE_TO_FUNCTION_1C1T1A = LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T1A, 4>
{
    std::pair {ket::Gate::CRX, static_cast<GateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_crx_gate)},
    std::pair {ket::Gate::CRY, static_cast<GateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_cry_gate)},
    std::pair {ket::Gate::CRZ, static_cast<GateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_crz_gate)},
    std::pair {ket::Gate::CP, static_cast<GateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_cp_gate)},
};



}  // namespace impl_ket

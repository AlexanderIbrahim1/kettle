#include <string>

#include "kettle_internal/common/linear_bijective_map.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"

#include "kettle_internal/gates/primitive_gate_map.hpp"

namespace ket::internal
{

using G = ket::PrimitiveGate;

// NOTE: can't use `auto` here because the corresponding header file uses `extern` declarations

const ket::internal::LinearBijectiveMap<G, G, 15> UNCONTROLLED_TO_CONTROLLED_GATE = {
    std::pair {G::H, G::CH},
    std::pair {G::X, G::CX},
    std::pair {G::Y, G::CY},
    std::pair {G::Z, G::CZ},
    std::pair {G::S, G::CS},
    std::pair {G::SDAG, G::CSDAG},
    std::pair {G::T, G::CT},
    std::pair {G::TDAG, G::CTDAG},
    std::pair {G::SX, G::CSX},
    std::pair {G::SXDAG, G::CSXDAG},
    std::pair {G::RX, G::CRX},
    std::pair {G::RY, G::CRY},
    std::pair {G::RZ, G::CRZ},
    std::pair {G::P, G::CP},
    std::pair {G::U, G::CU}
};

// NOLINTNEXTLINE(cert-err58-cpp)
const ket::internal::LinearBijectiveMap<G, std::string, 31> PRIMITIVE_GATES_TO_STRING = {
    std::pair {G::H, "H"},
    std::pair {G::X, "X"},
    std::pair {G::Y, "Y"},
    std::pair {G::Z, "Z"},
    std::pair {G::S, "S"},
    std::pair {G::SDAG, "SDAG"},
    std::pair {G::T, "T"},
    std::pair {G::TDAG, "TDAG"},
    std::pair {G::SX, "SX"},
    std::pair {G::SXDAG, "SXDAG"},
    std::pair {G::RX, "RX"},
    std::pair {G::RY, "RY"},
    std::pair {G::RZ, "RZ"},
    std::pair {G::P, "P"},
    std::pair {G::CH, "CH"},
    std::pair {G::CX, "CX"},
    std::pair {G::CY, "CY"},
    std::pair {G::CZ, "CZ"},
    std::pair {G::CS, "CS"},
    std::pair {G::CSDAG, "CSDAG"},
    std::pair {G::CT, "CT"},
    std::pair {G::CTDAG, "CTDAG"},
    std::pair {G::CSX, "CSX"},
    std::pair {G::CSXDAG, "CSXDAG"},
    std::pair {G::CRX, "CRX"},
    std::pair {G::CRY, "CRY"},
    std::pair {G::CRZ, "CRZ"},
    std::pair {G::CP, "CP"},
    std::pair {G::U, "U"},
    std::pair {G::CU, "CU"},
    std::pair {G::M, "M"},
};

const ket::internal::LinearBijectiveMap<G, PrimitiveGateFuncPtr1T, 10> GATE_TO_FUNCTION_1T = {
    std::pair {G::H, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_h_gate)},
    std::pair {G::X, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_x_gate)},
    std::pair {G::Y, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_y_gate)},
    std::pair {G::Z, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_z_gate)},
    std::pair {G::S, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_s_gate)},
    std::pair {G::SDAG, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_sdag_gate)},
    std::pair {G::T, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_t_gate)},
    std::pair {G::TDAG, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_tdag_gate)},
    std::pair {G::SX, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_sx_gate)},
    std::pair {G::SXDAG, static_cast<PrimitiveGateFuncPtr1T>(&ket::QuantumCircuit::add_sxdag_gate)}
};

const ket::internal::LinearBijectiveMap<G, PrimitiveGateFuncPtr1T1A, 4> GATE_TO_FUNCTION_1T1A = {
    std::pair {G::RX, static_cast<PrimitiveGateFuncPtr1T1A>(&ket::QuantumCircuit::add_rx_gate)},
    std::pair {G::RY, static_cast<PrimitiveGateFuncPtr1T1A>(&ket::QuantumCircuit::add_ry_gate)},
    std::pair {G::RZ, static_cast<PrimitiveGateFuncPtr1T1A>(&ket::QuantumCircuit::add_rz_gate)},
    std::pair {G::P, static_cast<PrimitiveGateFuncPtr1T1A>(&ket::QuantumCircuit::add_p_gate)},
};

const ket::internal::LinearBijectiveMap<G, PrimitiveGateFuncPtr1C1T, 10> GATE_TO_FUNCTION_1C1T = {
    std::pair {G::CH, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_ch_gate)},
    std::pair {G::CX, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_cx_gate)},
    std::pair {G::CY, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_cy_gate)},
    std::pair {G::CZ, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_cz_gate)},
    std::pair {G::CS, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_cs_gate)},
    std::pair {G::CSDAG, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_csdag_gate)},
    std::pair {G::CT, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_ct_gate)},
    std::pair {G::CTDAG, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_ctdag_gate)},
    std::pair {G::CSX, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_csx_gate)},
    std::pair {G::CSXDAG, static_cast<PrimitiveGateFuncPtr1C1T>(&ket::QuantumCircuit::add_csxdag_gate)}
};

const ket::internal::LinearBijectiveMap<G, PrimitiveGateFuncPtr1C1T1A, 4> GATE_TO_FUNCTION_1C1T1A = {
    std::pair {G::CRX, static_cast<PrimitiveGateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_crx_gate)},
    std::pair {G::CRY, static_cast<PrimitiveGateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_cry_gate)},
    std::pair {G::CRZ, static_cast<PrimitiveGateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_crz_gate)},
    std::pair {G::CP, static_cast<PrimitiveGateFuncPtr1C1T1A>(&ket::QuantumCircuit::add_cp_gate)},
};

}  // namespace ket::internal

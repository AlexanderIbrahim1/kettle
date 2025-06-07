#include "kettle_internal/common/linear_bijective_map.hpp"
#include "kettle/gates/compound_gate.hpp"
#include "kettle/circuit/circuit.hpp"

#include "kettle_internal/gates/compound_gate_map.hpp"

namespace ket::internal
{

using CG = ket::CompoundGate;

const ket::internal::LinearBijectiveMap<ket::CompoundGate, GateFuncPtr2C1T, 3> GATE_TO_FUNCTION_2C1T = {
    std::pair {CG::CCX, static_cast<GateFuncPtr2C1T>(&ket::QuantumCircuit::add_ccx_gate)},
    std::pair {CG::CCY, static_cast<GateFuncPtr2C1T>(&ket::QuantumCircuit::add_ccy_gate)},
    std::pair {CG::CCZ, static_cast<GateFuncPtr2C1T>(&ket::QuantumCircuit::add_ccz_gate)},
};


}  // namespace ket::internal

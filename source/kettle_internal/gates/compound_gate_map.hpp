#pragma once

#include "kettle_internal/common/linear_bijective_map.hpp"
#include "kettle/gates/compound_gate.hpp"
#include "kettle/circuit/circuit.hpp"
// #include "kettle/parameter/parameter.hpp"


namespace ket::internal
{

// unparameterized gates
using GateFuncPtr2C1T = void(ket::QuantumCircuit::*)(std::size_t, std::size_t, std::size_t);

extern const ket::internal::LinearBijectiveMap<ket::CompoundGate, GateFuncPtr2C1T, 3> GATE_TO_FUNCTION_2C1T;

}  // namespace ket::internal

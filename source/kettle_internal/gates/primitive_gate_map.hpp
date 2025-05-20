#pragma once

#include <string>

#include "kettle_internal/common/linear_bijective_map.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"


namespace ket::internal
{

using GateFuncPtr1T = void(ket::QuantumCircuit::*)(std::size_t);
using GateFuncPtr1T1A = void(ket::QuantumCircuit::*)(std::size_t, double);
using GateFuncPtr1C1T = void(ket::QuantumCircuit::*)(std::size_t, std::size_t);
using GateFuncPtr1C1T1A = void(ket::QuantumCircuit::*)(std::size_t, std::size_t, double);

extern const ket::internal::LinearBijectiveMap<ket::Gate, ket::Gate, 11> UNCONTROLLED_TO_CONTROLLED_GATE;

extern const ket::internal::LinearBijectiveMap<ket::Gate, std::string, 23> PRIMITIVE_GATES_TO_STRING;

extern const ket::internal::LinearBijectiveMap<ket::Gate, GateFuncPtr1T, 6> GATE_TO_FUNCTION_1T;

extern const ket::internal::LinearBijectiveMap<ket::Gate, GateFuncPtr1T1A, 4> GATE_TO_FUNCTION_1T1A;

extern const ket::internal::LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T, 6> GATE_TO_FUNCTION_1C1T;

extern const ket::internal::LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T1A, 4> GATE_TO_FUNCTION_1C1T1A;

}  // namespace ket::internal

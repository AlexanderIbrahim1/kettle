#pragma once

#include <string>

#include "kettle_internal/common/linear_bijective_map.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"


namespace ket::internal
{

using PrimitiveGateFuncPtr1T = void(ket::QuantumCircuit::*)(std::size_t);
using PrimitiveGateFuncPtr1T1A = void(ket::QuantumCircuit::*)(std::size_t, double);
using PrimitiveGateFuncPtr1C1T = void(ket::QuantumCircuit::*)(std::size_t, std::size_t);
using PrimitiveGateFuncPtr1C1T1A = void(ket::QuantumCircuit::*)(std::size_t, std::size_t, double);

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, ket::PrimitiveGate, 15> UNCONTROLLED_TO_CONTROLLED_GATE;

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, std::string, 31> PRIMITIVE_GATES_TO_STRING;

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, PrimitiveGateFuncPtr1T, 10> GATE_TO_FUNCTION_1T;

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, PrimitiveGateFuncPtr1T1A, 4> GATE_TO_FUNCTION_1T1A;

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, PrimitiveGateFuncPtr1C1T, 10> GATE_TO_FUNCTION_1C1T;

extern const ket::internal::LinearBijectiveMap<ket::PrimitiveGate, PrimitiveGateFuncPtr1C1T1A, 4> GATE_TO_FUNCTION_1C1T1A;

}  // namespace ket::internal

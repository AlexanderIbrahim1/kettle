#pragma once

#include <string>

#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/parameter/parameter.hpp"

#include "kettle_internal/common/linear_bijective_map.hpp"


namespace ket::internal
{

// unparameterized gates
using GateFuncPtr1T = void(ket::QuantumCircuit::*)(std::size_t);
using GateFuncPtr1T1A = void(ket::QuantumCircuit::*)(std::size_t, double);
using GateFuncPtr1C1T = void(ket::QuantumCircuit::*)(std::size_t, std::size_t);
using GateFuncPtr1C1T1A = void(ket::QuantumCircuit::*)(std::size_t, std::size_t, double);
using NoParamMatrixFuncPtr = Matrix2X2(*)() noexcept;
using ParamMatrixFuncPtr = Matrix2X2(*)(double) noexcept;

// parameterized gates
using GateFuncPtr1T1A_init_param = ket::param::ParameterID(ket::QuantumCircuit::*)(std::size_t, double, ket::param::parameterized);
using GateFuncPtr1C1T1A_init_param = ket::param::ParameterID(ket::QuantumCircuit::*)(std::size_t, std::size_t, double, ket::param::parameterized);

extern const LinearBijectiveMap<ket::Gate, ket::Gate, 15> UNCONTROLLED_TO_CONTROLLED_GATE;

extern const LinearBijectiveMap<ket::Gate, std::string, 31> PRIMITIVE_GATES_TO_STRING;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1T, 10> GATE_TO_FUNCTION_1T;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1T1A, 4> GATE_TO_FUNCTION_1T1A;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T, 10> GATE_TO_FUNCTION_1C1T;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T1A, 4> GATE_TO_FUNCTION_1C1T1A;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1T1A_init_param, 4> GATE_TO_FUNCTION_1T1A_INIT_PARAM;

extern const LinearBijectiveMap<ket::Gate, GateFuncPtr1C1T1A_init_param, 4> GATE_TO_FUNCTION_1C1T1A_INIT_PARAM;

extern const LinearBijectiveMap<ket::Gate, NoParamMatrixFuncPtr, 20> GATE_TO_MATRIX_FUNCTION_NO_PARAM;

extern const LinearBijectiveMap<ket::Gate, ParamMatrixFuncPtr, 8> GATE_TO_MATRIX_FUNCTION_PARAM;

}  // namespace ket::internal

#pragma once

#include <optional>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/tolerance.hpp"
#include "kettle/gates/primitive_gate.hpp"

/*
    This header file contains functions for decomposing a general 2x2 unitary matrix
    to a product of primitive 2x2 quantum gates.
*/

namespace ket::internal
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
struct Matrix2X2GateInfo_
{
    ket::PrimitiveGate gate;
    std::optional<double> parameter = std::nullopt;
};

/*
    Find the angle of the determinant.
*/
auto determinant_angle_(const ket::Matrix2X2& matrix) -> double;

/*
    Decompose a 2x2 unitary matrix into one of several primitive 1-qubit unitary gates.

    This function attempts to construct unparameterized gates before paramterized gates. For
    example, if the matrix [1, 0; 0 -1] is passed as an input, this function will attempt
    to decompose it as a Z gate instead of an RZ gate with an angle of (-M_PI/2).
*/
auto decomp_to_single_primitive_gate_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::optional<Matrix2X2GateInfo_>;

/*
    The implementation of this decomposition is taken directly from the following file:
        https://github.com/fedimser/quantum_decomp/blob/master/quantum_decomp/src/decompose_2x2.py
    
    The author of the repository if fedimser.
    The repo is published under the MIT license.
*/
auto decomp_special_unitary_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<Matrix2X2GateInfo_>;

auto decomp_to_primitive_gates_(
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<Matrix2X2GateInfo_>;

auto decomp_to_one_target_primitive_gates_(
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<ket::PrimitiveGateInfo>;

auto decomp_to_one_control_one_target_primitive_gates_(
    std::size_t control,
    std::size_t target,
    const ket::Matrix2X2& unitary,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> std::vector<ket::PrimitiveGateInfo>;

}  // namespace ket::internal

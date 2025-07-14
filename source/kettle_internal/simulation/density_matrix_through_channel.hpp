#pragma once

#include <optional>

#include <Eigen/Dense>

#include "kettle/parameter/parameter.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/operator/channels/mixed_unitary_channel.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/pauli_channel.hpp"

#include "kettle_internal/simulation/simulate_utils.hpp"

/*
    This file contains code for applying a quantum channel to a density matrix, thus evolving
    it into another density matrix.
*/

namespace ket
{

void simulate_one_qubit_kraus_channel(
    DensityMatrix& state,
    const OneQubitKrausChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& writing_buffer,
    Eigen::MatrixXcd& left_mul_buffer,
    Eigen::MatrixXcd& right_mul_buffer
);

void simulate_multi_qubit_kraus_channel(
    DensityMatrix& state,
    const MultiQubitKrausChannel& channel,
    Eigen::MatrixXcd& writing_buffer
);

void simulate_pauli_channel(
    DensityMatrix& state,
    const PauliChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& accumulation_buffer,
    Eigen::MatrixXcd& multiplication_buffer,
    Eigen::MatrixXcd& state_buffer
);

void simulate_mixed_unitary_channel(
    DensityMatrix& state,
    const MixedUnitaryChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& single_pair,
    const internal::FlatIndexPair<Eigen::Index>& double_pair,
    Eigen::MatrixXcd& accumulation_buffer,
    Eigen::MatrixXcd& multiplication_buffer,
    Eigen::MatrixXcd& state_buffer,
    const std::optional<ket::param::EvaluatedParameterDataMap>& param_map = std::nullopt
);

}  // namespace ket

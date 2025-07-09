#pragma once

#include <Eigen/Dense>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"

#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/operations_density_matrix.hpp"

/*
    This file contains code for applying a quantum channel to a density matrix, thus evolving
    it into another density matrix.
*/

namespace ket
{

inline void simulate_one_qubit_kraus_channel(
    DensityMatrix& state,
    const OneQubitKrausChannel& channel,
    const internal::FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& writing_buffer,
    Eigen::MatrixXcd& left_mul_buffer,
    Eigen::MatrixXcd& right_mul_buffer
)
{
    const auto target_index = static_cast<Eigen::Index>(channel.target_index());
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = ket::internal::SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = ket::internal::SingleQubitGatePairGenerator {target_index, n_qubits};

    const auto n_kraus_matrices = channel.matrices().size();

    // function reference to reduce line length
    const auto& left_mul = internal::apply_left_one_qubit_matrix_;
    const auto& right_mul = internal::apply_right_one_qubit_matrix_;

    for (std::size_t i {0}; i < n_kraus_matrices; ++i) {
        const auto mat = channel.matrices()[i];
        const auto mat_adj = ket::conjugate_transpose(mat);

        left_mul(state.matrix(), left_mul_buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
        right_mul(left_mul_buffer, right_mul_buffer, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);

        // skip setting all elements in the buffer to 0, by overwriting on the first iteration
        if (i == 0) {
            writing_buffer = right_mul_buffer;
        } else {
            writing_buffer += right_mul_buffer;
        }
    }

    state.matrix() = writing_buffer;
}


inline void simulate_multi_qubit_kraus_channel(
    DensityMatrix& state,
    const MultiQubitKrausChannel& channel,
    Eigen::MatrixXcd& writing_buffer
)
{
    const auto n_kraus_matrices = channel.matrices().size();

    for (std::size_t i {0}; i < n_kraus_matrices; ++i) {
        const auto& matrix = channel.matrices()[i];
        if (i == 0) {
            writing_buffer = matrix * state.matrix() * matrix.adjoint();
        } else {
            writing_buffer += matrix * state.matrix() * matrix.adjoint();
        }
    }

    state.matrix() = writing_buffer;
}

}  // namespace ket

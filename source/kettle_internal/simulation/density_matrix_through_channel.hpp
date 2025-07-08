#pragma once

#include <Eigen/Dense>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/state/density_matrix.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"

#include "kettle_internal/simulation/simulate_utils.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"

/*
    This file contains code for applying a quantum channel to a density matrix, thus evolving
    it into another density matrix.
*/

namespace ket::internal
{

/*
    Perform the multiplcation of K * rho

    TODO: this is nearly identical to the `apply_u_gate_first_()` function; once I get all
    of this to work, I should spend some time on clean-up and code reduction.
*/
inline void apply_left_multiplication_(
    const Eigen::MatrixXcd& original_state,
    Eigen::MatrixXcd& output_buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
)
{
    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_row0, i_row1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_col0, i_col1] = pair_iterator_inner.next();

            const auto rho00 = original_state(i_row0, i_col0);
            const auto rho10 = original_state(i_row1, i_col0);
            const auto rho01 = original_state(i_row0, i_col1);
            const auto rho11 = original_state(i_row1, i_col1);

            output_buffer(i_row0, i_col0) = (rho00 * mat.elem00) + (rho10 * mat.elem01);
            output_buffer(i_row1, i_col0) = (rho00 * mat.elem10) + (rho10 * mat.elem11);
            output_buffer(i_row0, i_col1) = (rho01 * mat.elem00) + (rho11 * mat.elem01);
            output_buffer(i_row1, i_col1) = (rho01 * mat.elem10) + (rho11 * mat.elem11);
        }
    }
}


/*
    Perform the multiplcation of (K * rho) * K^t

    TODO: this is nearly identical to the `apply_u_gate_second_()` function; once I get all
    of this to work, I should spend some time on clean-up and code reduction.
*/
inline void apply_right_multiplication_(
    const Eigen::MatrixXcd& left_product,
    Eigen::MatrixXcd& output_buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat_adj
)
{
    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_col0, i_col1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_row0, i_row1] = pair_iterator_inner.next();

            const auto buf00 = left_product(i_row0, i_col0);
            const auto buf10 = left_product(i_row1, i_col0);
            const auto buf01 = left_product(i_row0, i_col1);
            const auto buf11 = left_product(i_row1, i_col1);

            output_buffer(i_row0, i_col0) = (buf00 * mat_adj.elem00) + (buf01 * mat_adj.elem10);
            output_buffer(i_row1, i_col0) = (buf10 * mat_adj.elem00) + (buf11 * mat_adj.elem10);
            output_buffer(i_row0, i_col1) = (buf00 * mat_adj.elem01) + (buf01 * mat_adj.elem11);
            output_buffer(i_row1, i_col1) = (buf10 * mat_adj.elem01) + (buf11 * mat_adj.elem11);
        }
    }
}

}  // namespace ket::internal

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
    const auto& left_mul = internal::apply_left_multiplication_;
    const auto& right_mul = internal::apply_right_multiplication_;

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

}  // namespace ket

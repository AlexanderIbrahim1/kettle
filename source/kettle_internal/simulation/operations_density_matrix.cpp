#include <cmath>
#include <complex>

#include "kettle/common/matrix2x2.hpp"

#include "kettle_internal/simulation/operations_density_matrix.hpp"


namespace ket::internal
{

auto dot(const ElementPair& left, const ElementPair& right) -> std::complex<double>
{
    return (left.x0 * right.x0) + (left.x1 * right.x1);
}

/*
    Perform the multiplcation of `K * rho`, where:
      - `K` is a `Matrix2X2` instance representing a single-qubit operator, which we refer to
        as `original_state`
      - `rho` is the density matrix
    
    The output is written to `output_buffer`.
*/
void apply_left_one_qubit_matrix_(
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
    Perform the multiplcation of `(K * rho) * K^t`, where:
      - `(K * rho)` is the product of the 1-qubit operator `K` and the density matrix `rho`,
        which was calculated earlier in the `apply_left_one_qubit_matrix_()` function,
        and which we refer to as `left_product`
      - `K^t` is the adjoint of the 1-qubit operator
    
    The output is written to `output_buffer`.
*/
void apply_right_one_qubit_matrix_(
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


/*
    Perform the full multiplcation of `((K * rho) * K^t)`, where:
      - `(K * rho)` is the product of the 1-qubit operator `K` and the density matrix `rho`,
        which was calculated earlier in the `apply_left_one_qubit_matrix_()` function,
        and which we refer to as `left_product`
      - `K^t` is the adjoint of the 1-qubit operator
    
    The output is written to `output_buffer`.
*/
void simulate_u_gate_(
    Eigen::MatrixXcd& state,
    Eigen::MatrixXcd& buffer,
    Eigen::Index target_index,
    Eigen::Index n_qubits,
    const ket::Matrix2X2& mat,
    const FlatIndexPair<Eigen::Index>& pair
)
{
    auto pair_iterator_outer = SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    apply_left_one_qubit_matrix_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);

    const auto mat_adj = ket::conjugate_transpose(mat);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    apply_right_one_qubit_matrix_(buffer, state, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);
}


void apply_cu_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
)
{
    const auto matpair_00_01 = ElementPair {.x0=mat.elem00, .x1=mat.elem01};
    const auto matpair_10_11 = ElementPair {.x0=mat.elem10, .x1=mat.elem11};

    // TODO: swap which iterator is set to 0, and which is set to i_lower?
    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_col0, i_col2, i_col1, i_col3] = pair_iterator_outer.next_unset_and_set();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair_inner {pair.i_lower}; i_pair_inner < pair.i_upper; ++i_pair_inner) {
            const auto [i_row0, i_row2, i_row1, i_row3] = pair_iterator_inner.next_unset_and_set();

            const auto rhopair_10_30 = ElementPair {.x0=state.matrix()(i_row1, i_col0), .x1=state.matrix()(i_row3, i_col0)};
            const auto rhopair_11_31 = ElementPair {.x0=state.matrix()(i_row1, i_col1), .x1=state.matrix()(i_row3, i_col1)};
            const auto rhopair_12_32 = ElementPair {.x0=state.matrix()(i_row1, i_col2), .x1=state.matrix()(i_row3, i_col2)};
            const auto rhopair_13_33 = ElementPair {.x0=state.matrix()(i_row1, i_col3), .x1=state.matrix()(i_row3, i_col3)};

            buffer(i_row0, i_col0) = state.matrix()(i_row0, i_col0);
            buffer(i_row1, i_col0) = dot(matpair_00_01, rhopair_10_30);
            buffer(i_row2, i_col0) = state.matrix()(i_row2, i_col0);
            buffer(i_row3, i_col0) = dot(matpair_10_11, rhopair_10_30);

            buffer(i_row0, i_col1) = state.matrix()(i_row0, i_col1);
            buffer(i_row1, i_col1) = dot(matpair_00_01, rhopair_11_31);
            buffer(i_row2, i_col1) = state.matrix()(i_row2, i_col1);
            buffer(i_row3, i_col1) = dot(matpair_10_11, rhopair_11_31);

            buffer(i_row0, i_col2) = state.matrix()(i_row0, i_col2);
            buffer(i_row1, i_col2) = dot(matpair_00_01, rhopair_12_32);
            buffer(i_row2, i_col2) = state.matrix()(i_row2, i_col2);
            buffer(i_row3, i_col2) = dot(matpair_10_11, rhopair_12_32);

            buffer(i_row0, i_col3) = state.matrix()(i_row0, i_col3);
            buffer(i_row1, i_col3) = dot(matpair_00_01, rhopair_13_33);
            buffer(i_row2, i_col3) = state.matrix()(i_row2, i_col3);
            buffer(i_row3, i_col3) = dot(matpair_10_11, rhopair_13_33);
        }
    }
}


void apply_cu_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
)
{
    const auto matpair_00_10 = ElementPair {.x0=mat.elem00, .x1=mat.elem10};
    const auto matpair_01_11 = ElementPair {.x0=mat.elem01, .x1=mat.elem11};

    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_row0, i_row2, i_row1, i_row3] = pair_iterator_outer.next_unset_and_set();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair_inner {pair.i_lower}; i_pair_inner < pair.i_upper; ++i_pair_inner) {
            const auto [i_col0, i_col2, i_col1, i_col3] = pair_iterator_inner.next_unset_and_set();

            const auto rhopair_01_03 = ElementPair {.x0=buffer(i_row0, i_col1), .x1=buffer(i_row0, i_col3)};
            const auto rhopair_11_13 = ElementPair {.x0=buffer(i_row1, i_col1), .x1=buffer(i_row1, i_col3)};
            const auto rhopair_21_23 = ElementPair {.x0=buffer(i_row2, i_col1), .x1=buffer(i_row2, i_col3)};
            const auto rhopair_31_33 = ElementPair {.x0=buffer(i_row3, i_col1), .x1=buffer(i_row3, i_col3)};

            state.matrix()(i_row0, i_col0) = buffer(i_row0, i_col0);
            state.matrix()(i_row1, i_col0) = buffer(i_row1, i_col0);
            state.matrix()(i_row2, i_col0) = buffer(i_row2, i_col0);
            state.matrix()(i_row3, i_col0) = buffer(i_row3, i_col0);

            state.matrix()(i_row0, i_col1) = dot(matpair_00_10, rhopair_01_03);
            state.matrix()(i_row1, i_col1) = dot(matpair_00_10, rhopair_11_13);
            state.matrix()(i_row2, i_col1) = dot(matpair_00_10, rhopair_21_23);
            state.matrix()(i_row3, i_col1) = dot(matpair_00_10, rhopair_31_33);

            state.matrix()(i_row0, i_col2) = buffer(i_row0, i_col2);
            state.matrix()(i_row1, i_col2) = buffer(i_row1, i_col2);
            state.matrix()(i_row2, i_col2) = buffer(i_row2, i_col2);
            state.matrix()(i_row3, i_col2) = buffer(i_row3, i_col2);

            state.matrix()(i_row0, i_col3) = dot(matpair_01_11, rhopair_01_03);
            state.matrix()(i_row1, i_col3) = dot(matpair_01_11, rhopair_11_13);
            state.matrix()(i_row2, i_col3) = dot(matpair_01_11, rhopair_21_23);
            state.matrix()(i_row3, i_col3) = dot(matpair_01_11, rhopair_31_33);
        }
    }
}

}  // namespace ket::internal

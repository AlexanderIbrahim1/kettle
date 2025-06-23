#include <cmath>
#include <complex>

#include "kettle/common/matrix2x2.hpp"
// #include "kettle/gates/primitive_gate.hpp"

#include "kettle_internal/simulation/operations_density_matrix.hpp"


namespace ket::internal
{

auto dot(const ElementPair& left, const ElementPair& right) -> std::complex<double>
{
    return (left.x0 * right.x0) + (left.x1 * right.x1);
}

// void apply_h_gate(ket::Statevector& state, std::size_t i0, std::size_t i1)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto real_add = M_SQRT1_2 * (state0.real() + state1.real());
//     const auto imag_add = M_SQRT1_2 * (state0.imag() + state1.imag());
//     const auto real_sub = M_SQRT1_2 * (state0.real() - state1.real());
//     const auto imag_sub = M_SQRT1_2 * (state0.imag() - state1.imag());
// 
//     state[i0] = std::complex<double> {real_add, imag_add};
//     state[i1] = std::complex<double> {real_sub, imag_sub};
// }
// 
// void apply_x_gate(ket::Statevector& state, std::size_t i0, std::size_t i1)
// {
//     std::swap(state[i0], state[i1]);
// }
// 
// void apply_y_gate(ket::Statevector& state, std::size_t i0, std::size_t i1)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto real0 = state1.imag();
//     const auto imag0 = -state1.real();
//     const auto real1 = -state0.imag();
//     const auto imag1 = state0.real();
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_z_gate(ket::Statevector& state, std::size_t i1)
// {
//     state[i1] *= -1.0;
// }
// 
// void apply_s_gate(ket::Statevector& state, std::size_t i1)
// {
//     const auto state1 = state[i1];
//     state[i1] = {-state1.imag(), state1.real()};
// }
// 
// void apply_sdag_gate(ket::Statevector& state, std::size_t i1)
// {
//     const auto state1 = state[i1];
//     state[i1] = {state1.imag(), -state1.real()};
// }
// 
// void apply_t_gate(ket::Statevector& state, std::size_t i1)
// {
//     const auto state1 = state[i1];
// 
//     const auto real1 = M_SQRT1_2 * (state1.real() - state1.imag());
//     const auto imag1 = M_SQRT1_2 * (state1.real() + state1.imag());
// 
//     state[i1] = {real1, imag1};
// }
// 
// void apply_tdag_gate(ket::Statevector& state, std::size_t i1)
// {
//     const auto state1 = state[i1];
// 
//     const auto real1 = M_SQRT1_2 * (state1.real() + state1.imag());
//     const auto imag1 = - M_SQRT1_2 * (state1.real() - state1.imag());
// 
//     state[i1] = {real1, imag1};
// }
// 
// void apply_sx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto real0 = 0.5 * (  state0.real() - state0.imag() + state1.real() + state1.imag());
//     const auto imag0 = 0.5 * (  state0.real() + state0.imag() - state1.real() + state1.imag());
//     const auto real1 = 0.5 * (  state0.real() + state0.imag() + state1.real() - state1.imag());
//     const auto imag1 = 0.5 * (- state0.real() + state0.imag() + state1.real() + state1.imag());
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_sxdag_gate(ket::Statevector& state, std::size_t i0, std::size_t i1)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto real0 = 0.5 * (  state0.real() + state0.imag() + state1.real() - state1.imag());
//     const auto imag0 = 0.5 * (- state0.real() + state0.imag() + state1.real() + state1.imag());
//     const auto real1 = 0.5 * (  state0.real() - state0.imag() + state1.real() + state1.imag());
//     const auto imag1 = 0.5 * (  state0.real() + state0.imag() - state1.real() + state1.imag());
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_rx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto cost = std::cos(theta / 2.0);
//     const auto sint = std::sin(theta / 2.0);
// 
//     const auto real0 = (state0.real() * cost) + (state1.imag() * sint);
//     const auto imag0 = (state0.imag() * cost) - (state1.real() * sint);
//     const auto real1 = (state1.real() * cost) + (state0.imag() * sint);
//     const auto imag1 = (state1.imag() * cost) - (state0.real() * sint);
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_ry_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto cost = std::cos(theta / 2.0);
//     const auto sint = std::sin(theta / 2.0);
// 
//     const auto real0 = (state0.real() * cost) - (state1.real() * sint);
//     const auto imag0 = (state0.imag() * cost) - (state1.imag() * sint);
//     const auto real1 = (state1.real() * cost) + (state0.real() * sint);
//     const auto imag1 = (state1.imag() * cost) + (state0.imag() * sint);
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_rz_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto cost = std::cos(theta / 2.0);
//     const auto sint = std::sin(theta / 2.0);
// 
//     const auto real0 = (state0.real() * cost) + (state0.imag() * sint);
//     const auto imag0 = (state0.imag() * cost) - (state0.real() * sint);
//     const auto real1 = (state1.real() * cost) - (state1.imag() * sint);
//     const auto imag1 = (state1.imag() * cost) + (state1.real() * sint);
// 
//     state[i0] = std::complex<double> {real0, imag0};
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_p_gate(ket::Statevector& state, std::size_t i1, double theta)
// {
//     const auto& state1 = state[i1];
// 
//     const auto cost = std::cos(theta);
//     const auto sint = std::sin(theta);
// 
//     const auto real1 = (state1.real() * cost) - (state1.imag() * sint);
//     const auto imag1 = (state1.imag() * cost) + (state1.real() * sint);
// 
//     state[i1] = std::complex<double> {real1, imag1};
// }
// 
// void apply_u_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, const ket::Matrix2X2& mat)
// {
//     const auto& state0 = state[i0];
//     const auto& state1 = state[i1];
// 
//     const auto new_state0 = state0 * mat.elem00 + state1 * mat.elem01;
//     const auto new_state1 = state0 * mat.elem10 + state1 * mat.elem11;
// 
//     state[i0] = new_state0;
//     state[i1] = new_state1;
// }


void apply_u_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat,
    Eigen::Index i_row0,
    Eigen::Index i_row1
)
{
    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        const auto [i_col0, i_col1] = pair_iterator.next();

        const auto rho00 = state.matrix()(i_row0, i_col0);
        const auto rho10 = state.matrix()(i_row1, i_col0);
        const auto rho01 = state.matrix()(i_row0, i_col1);
        const auto rho11 = state.matrix()(i_row1, i_col1);

        buffer(i_row0, i_col0) = (rho00 * mat.elem00) + (rho10 * mat.elem01);
        buffer(i_row1, i_col0) = (rho00 * mat.elem10) + (rho10 * mat.elem11);
        buffer(i_row0, i_col1) = (rho01 * mat.elem00) + (rho11 * mat.elem01);
        buffer(i_row1, i_col1) = (rho01 * mat.elem10) + (rho11 * mat.elem11);
    }
}


void apply_u_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat_adj,
    Eigen::Index i_col0,
    Eigen::Index i_col1
)
{
    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        const auto [i_row0, i_row1] = pair_iterator.next();

        const auto buf00 = buffer(i_row0, i_col0);
        const auto buf10 = buffer(i_row1, i_col0);
        const auto buf01 = buffer(i_row0, i_col1);
        const auto buf11 = buffer(i_row1, i_col1);

        state.matrix()(i_row0, i_col0) = (buf00 * mat_adj.elem00) + (buf01 * mat_adj.elem10);
        state.matrix()(i_row1, i_col0) = (buf10 * mat_adj.elem00) + (buf11 * mat_adj.elem10);
        state.matrix()(i_row0, i_col1) = (buf00 * mat_adj.elem01) + (buf01 * mat_adj.elem11);
        state.matrix()(i_row1, i_col1) = (buf10 * mat_adj.elem01) + (buf11 * mat_adj.elem11);
    }
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

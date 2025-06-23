#include <cmath>
#include <complex>

#include "kettle/common/matrix2x2.hpp"
// #include "kettle/gates/primitive_gate.hpp"

#include "kettle_internal/simulation/operations_density_matrix.hpp"


namespace ket::internal
{

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
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat,
    Eigen::Index i_row
)
{
    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        const auto [index_c0_t0, index_c0_t1, index_c1_t0, index_c1_t1] = pair_iterator.next_unset_and_set();

        const auto rho_elem0 = state.matrix()(i_row, index_c1_t0);
        const auto rho_elem1 = state.matrix()(i_row, index_c1_t1);

        buffer(i_row, index_c0_t0) = state.matrix()(i_row, index_c0_t0);
        buffer(i_row, index_c0_t1) = state.matrix()(i_row, index_c0_t1);
        buffer(i_row, index_c1_t0) = (mat.elem00 * rho_elem0) + (mat.elem01 * rho_elem1);
        buffer(i_row, index_c1_t1) = (mat.elem10 * rho_elem0) + (mat.elem11 * rho_elem1);
    }
}


void apply_cu_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat_adj,
    Eigen::Index i_col
)
{
    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        const auto [index_c0_t0, index_c0_t1, index_c1_t0, index_c1_t1] = pair_iterator.next_unset_and_set();

        const auto buf_elem0 = buffer(index_c1_t0, i_col);
        const auto buf_elem1 = buffer(index_c1_t1, i_col);

        state.matrix()(index_c0_t0, i_col) = buffer(index_c0_t0, i_col);
        state.matrix()(index_c0_t1, i_col) = buffer(index_c0_t1, i_col);
        state.matrix()(index_c1_t0, i_col) = (mat_adj.elem00 * buf_elem0) + (mat_adj.elem10 * buf_elem1);
        state.matrix()(index_c1_t1, i_col) = (mat_adj.elem01 * buf_elem0) + (mat_adj.elem11 * buf_elem1);
    }
}



}  // namespace ket::internal

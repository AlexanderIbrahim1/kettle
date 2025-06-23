#pragma once

#include <Eigen/Dense>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"

/*
    This header file contains the common operations performed on two states in the
    Statevector object.
*/

namespace ket::internal
{

/*
    Helper struct for the static_assert(), to see what ket::Gate instance is passed that causes
    the template instantiation to fail.
*/
template <ket::Gate GateType>
struct dm_gate_always_false : std::false_type
{};

struct ElementPair
{
    std::complex<double> x0;
    std::complex<double> x1;
};

auto dot(const ElementPair& left, const ElementPair& right) -> std::complex<double>;

template <ket::Gate GateType>
void apply_1t_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::Index i_row
)
{
    using G = ket::Gate;

    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        [[maybe_unused]]
        const auto [i0, i1] = pair_iterator.next();

        if constexpr (GateType == G::H) {
            const auto amplitude0 = state.matrix()(i_row, i0);
            const auto amplitude1 = state.matrix()(i_row, i1);

            buffer(i_row, i0) = M_SQRT1_2 * (amplitude0 + amplitude1);
            buffer(i_row, i1) = M_SQRT1_2 * (amplitude0 - amplitude1);
        }
        else if constexpr (GateType == G::X) {
            buffer(i_row, i0) = state.matrix()(i_row, i1);
            buffer(i_row, i1) = state.matrix()(i_row, i0);
        }
        else if constexpr (GateType == G::Y) {
            const auto amplitude0 = state.matrix()(i_row, i0);
            const auto amplitude1 = state.matrix()(i_row, i1);

            buffer(i_row, i0) = {amplitude1.imag(), -amplitude1.real()};
            buffer(i_row, i1) = {-amplitude0.imag(), amplitude0.real()};
        }
        else if constexpr (GateType == G::Z) {
            buffer(i_row, i1) = -1.0 * state.matrix()(i_row, i1);
        }
        else if constexpr (GateType == G::S) {
            const auto amplitude1 = state.matrix()(i_row, i1);
            buffer(i_row, i1) = {-amplitude1.imag(), amplitude1.real()};
        }
        else if constexpr (GateType == Gate::SDAG) {
            const auto amplitude1 = state.matrix()(i_row, i1);
            buffer(i_row, i1) = {amplitude1.imag(), -amplitude1.real()};
        }
        else if constexpr (GateType == Gate::T) {
            const auto amplitude1 = state.matrix()(i_row, i1);

            const auto real1 = M_SQRT1_2 * (amplitude1.real() - amplitude1.imag());
            const auto imag1 = M_SQRT1_2 * (amplitude1.real() + amplitude1.imag());

            buffer(i_row, i1) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::TDAG) {
            const auto amplitude1 = state.matrix()(i_row, i1);

            const auto real1 = M_SQRT1_2 * (amplitude1.real() + amplitude1.imag());
            const auto imag1 = - M_SQRT1_2 * (amplitude1.real() - amplitude1.imag());

            buffer(i_row, i1) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::SX) {
            const auto amplitude0 = state.matrix()(i_row, i0);
            const auto amplitude1 = state.matrix()(i_row, i1);

            const auto real0 = 0.5 * (  amplitude0.real() - amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto imag0 = 0.5 * (  amplitude0.real() + amplitude0.imag() - amplitude1.real() + amplitude1.imag());
            const auto real1 = 0.5 * (  amplitude0.real() + amplitude0.imag() + amplitude1.real() - amplitude1.imag());
            const auto imag1 = 0.5 * (- amplitude0.real() + amplitude0.imag() + amplitude1.real() + amplitude1.imag());

            buffer(i_row, i0) = {real0, imag0};
            buffer(i_row, i1) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::SXDAG) {
            const auto amplitude0 = state.matrix()(i_row, i0);
            const auto amplitude1 = state.matrix()(i_row, i1);

            const auto real0 = 0.5 * (  amplitude0.real() + amplitude0.imag() + amplitude1.real() - amplitude1.imag());
            const auto imag0 = 0.5 * (- amplitude0.real() + amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto real1 = 0.5 * (  amplitude0.real() - amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto imag1 = 0.5 * (  amplitude0.real() + amplitude0.imag() - amplitude1.real() + amplitude1.imag());

            buffer(i_row, i0) = {real0, imag0};
            buffer(i_row, i1) = {real1, imag1};
        }
        else {
            static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T gate for density matrix simulation of first multiplication.");
        }
    }
}

template <ket::Gate GateType>
void apply_1t_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::Index i_col
)
{
    using G = ket::Gate;

    pair_iterator.set_state(pair.i_lower);
    for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
        [[maybe_unused]]
        const auto [i0, i1] = pair_iterator.next();

        if constexpr (GateType == G::H) {
            const auto amplitude0 = buffer(i0, i_col);
            const auto amplitude1 = buffer(i1, i_col);

            // H-gate is Hermitian, no need to change assignment
            state.matrix()(i0, i_col) = M_SQRT1_2 * (amplitude0 + amplitude1);
            state.matrix()(i1, i_col) = M_SQRT1_2 * (amplitude0 - amplitude1);
        }
        else if constexpr (GateType == G::X) {
            // X-gate is Hermitian, no need to change assignment
            state.matrix()(i0, i_col) = buffer(i1, i_col);
            state.matrix()(i1, i_col) = buffer(i0, i_col);
        }
        else if constexpr (GateType == G::Y) {
            const auto amplitude0 = buffer(i0, i_col);
            const auto amplitude1 = buffer(i1, i_col);

            // Y-gate is Hermitian, no need to change assignment
            buffer(i0, i_col) = {amplitude1.imag(), -amplitude1.real()};
            buffer(i1, i_col) = {-amplitude0.imag(), amplitude0.real()};
        }
        else if constexpr (GateType == G::Z) {
            // Z-gate is Hermitian, no need to change assignment
            buffer(i1, i_col) = -1.0 * state.matrix()(i1, i_col);
        }
        else if constexpr (GateType == G::S) {
            // swapped the actions of S and SDAG
            const auto amplitude1 = buffer(i1, i_col);
            buffer(i1, i_col) = {amplitude1.imag(), -amplitude1.real()};
        }
        else if constexpr (GateType == Gate::SDAG) {
            // swapped the actions of S and SDAG
            const auto amplitude1 = buffer(i1, i_col);
            buffer(i1, i_col) = {-amplitude1.imag(), amplitude1.real()};
        }
        else if constexpr (GateType == Gate::T) {
            // swapped the actions of T and TDAG
            const auto amplitude1 = buffer(i1, i_col);

            const auto real1 = M_SQRT1_2 * (amplitude1.real() + amplitude1.imag());
            const auto imag1 = - M_SQRT1_2 * (amplitude1.real() - amplitude1.imag());

            buffer(i1, i_col) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::TDAG) {
            // swapped the actions of T and TDAG
            const auto amplitude1 = buffer(i1, i_col);

            const auto real1 = M_SQRT1_2 * (amplitude1.real() - amplitude1.imag());
            const auto imag1 = M_SQRT1_2 * (amplitude1.real() + amplitude1.imag());

            buffer(i1, i_col) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::SX) {
            const auto amplitude0 = buffer(i0, i_col);
            const auto amplitude1 = buffer(i1, i_col);

            const auto real0 = 0.5 * (  amplitude0.real() + amplitude0.imag() + amplitude1.real() - amplitude1.imag());
            const auto imag0 = 0.5 * (- amplitude0.real() + amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto real1 = 0.5 * (  amplitude0.real() - amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto imag1 = 0.5 * (  amplitude0.real() + amplitude0.imag() - amplitude1.real() + amplitude1.imag());

            buffer(i0, i_col) = {real0, imag0};
            buffer(i1, i_col) = {real1, imag1};
        }
        else if constexpr (GateType == Gate::SXDAG) {
            const auto amplitude0 = buffer(i0, i_col);
            const auto amplitude1 = buffer(i1, i_col);

            const auto real0 = 0.5 * (  amplitude0.real() - amplitude0.imag() + amplitude1.real() + amplitude1.imag());
            const auto imag0 = 0.5 * (  amplitude0.real() + amplitude0.imag() - amplitude1.real() + amplitude1.imag());
            const auto real1 = 0.5 * (  amplitude0.real() + amplitude0.imag() + amplitude1.real() - amplitude1.imag());
            const auto imag1 = 0.5 * (- amplitude0.real() + amplitude0.imag() + amplitude1.real() + amplitude1.imag());

            buffer(i0, i_col) = {real0, imag0};
            buffer(i1, i_col) = {real1, imag1};
        }
        else {
            static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T gate for density matrix simulation of second multiplication.");
        }
    }
}


void apply_u_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat,
    Eigen::Index i_row0,
    Eigen::Index i_row1
);

void apply_u_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat_adj,
    Eigen::Index i_col0,
    Eigen::Index i_col1
);

void apply_cu_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
);

void apply_cu_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
);

// void apply_h_gate(ket::DensityMatrix& state, std::size_t i0, std::size_t i1);
// 
// void apply_x_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);
// 
// void apply_y_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);
// 
// void apply_z_gate(ket::Statevector& state, std::size_t i1);
// 
// void apply_s_gate(ket::Statevector& state, std::size_t i1);
// 
// void apply_sdag_gate(ket::Statevector& state, std::size_t i1);
// 
// void apply_t_gate(ket::Statevector& state, std::size_t i1);
// 
// void apply_tdag_gate(ket::Statevector& state, std::size_t i1);
// 
// void apply_sx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);
// 
// void apply_sxdag_gate(ket::Statevector& state, std::size_t i0, std::size_t i1);
// 
// void apply_rx_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);
// 
// void apply_ry_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);
// 
// void apply_rz_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, double theta);
// 
// void apply_p_gate(ket::Statevector& state, std::size_t i1, double theta);
// 
// void apply_u_gate(ket::Statevector& state, std::size_t i0, std::size_t i1, const ket::Matrix2X2& mat);
// 
}  // namespace ket::internal

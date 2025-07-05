#pragma once

#include <Eigen/Dense>
#include <cmath>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
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
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair
)
{
    constexpr auto plus_half = std::complex<double> {0.5, 0.5};
    constexpr auto minus_half = std::complex<double> {0.5, -0.5};
    constexpr auto plus_rt2 = std::complex<double> {M_SQRT1_2, M_SQRT1_2};
    constexpr auto minus_rt2 = std::complex<double> {M_SQRT1_2, -M_SQRT1_2};

    using G = ket::Gate;

    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_row0, i_row1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_col0, i_col1] = pair_iterator_inner.next();

            const auto rho00 = state.matrix()(i_row0, i_col0);
            const auto rho10 = state.matrix()(i_row1, i_col0);
            const auto rho01 = state.matrix()(i_row0, i_col1);
            const auto rho11 = state.matrix()(i_row1, i_col1);

            if constexpr (GateType == G::H) {
                buffer(i_row0, i_col0) = M_SQRT1_2 * (rho00 + rho10);
                buffer(i_row1, i_col0) = M_SQRT1_2 * (rho00 - rho10);
                buffer(i_row0, i_col1) = M_SQRT1_2 * (rho01 + rho11);
                buffer(i_row1, i_col1) = M_SQRT1_2 * (rho01 - rho11);
            }
            else if constexpr (GateType == G::X) {
                buffer(i_row0, i_col0) = rho10;
                buffer(i_row1, i_col0) = rho00;
                buffer(i_row0, i_col1) = rho11;
                buffer(i_row1, i_col1) = rho01;
            }
            else if constexpr (GateType == G::Y) {
                buffer(i_row0, i_col0) = { rho10.imag(), -rho10.real()};
                buffer(i_row1, i_col0) = {-rho00.imag(),  rho00.real()};
                buffer(i_row0, i_col1) = { rho11.imag(), -rho11.real()};
                buffer(i_row1, i_col1) = {-rho01.imag(),  rho01.real()};
            }
            else if constexpr (GateType == G::Z) {
                buffer(i_row0, i_col0) =  rho00;
                buffer(i_row1, i_col0) = -rho10;
                buffer(i_row0, i_col1) =  rho01;
                buffer(i_row1, i_col1) = -rho11;
            }
            else if constexpr (GateType == G::S) {
                buffer(i_row0, i_col0) = rho00;
                buffer(i_row1, i_col0) = {-rho10.imag(),  rho10.real()};
                buffer(i_row0, i_col1) = rho01;
                buffer(i_row1, i_col1) = {-rho11.imag(),  rho11.real()};
            }
            else if constexpr (GateType == Gate::SDAG) {
                buffer(i_row0, i_col0) = rho00;
                buffer(i_row1, i_col0) = { rho10.imag(), -rho10.real()};
                buffer(i_row0, i_col1) = rho01;
                buffer(i_row1, i_col1) = { rho11.imag(), -rho11.real()};
            }
            else if constexpr (GateType == Gate::T) {
                buffer(i_row0, i_col0) = rho00;
                buffer(i_row1, i_col0) = rho10 * plus_rt2;
                buffer(i_row0, i_col1) = rho01;
                buffer(i_row1, i_col1) = rho11 * plus_rt2;
            }
            else if constexpr (GateType == Gate::TDAG) {
                buffer(i_row0, i_col0) = rho00;
                buffer(i_row1, i_col0) = rho10 * minus_rt2;
                buffer(i_row0, i_col1) = rho01;
                buffer(i_row1, i_col1) = rho11 * minus_rt2;
            }
            else if constexpr (GateType == Gate::SX) {
                buffer(i_row0, i_col0) = (rho00 * plus_half) + (rho10 * minus_half);
                buffer(i_row1, i_col0) = (rho00 * minus_half) + (rho10 * plus_half);
                buffer(i_row0, i_col1) = (rho01 * plus_half) + (rho11 * minus_half);
                buffer(i_row1, i_col1) = (rho01 * minus_half) + (rho11 * plus_half);
            }
            else if constexpr (GateType == Gate::SXDAG) {
                buffer(i_row0, i_col0) = (rho00 * minus_half) + (rho10 * plus_half);
                buffer(i_row1, i_col0) = (rho00 * plus_half) + (rho10 * minus_half);
                buffer(i_row0, i_col1) = (rho01 * minus_half) + (rho11 * plus_half);
                buffer(i_row1, i_col1) = (rho01 * plus_half) + (rho11 * minus_half);
            }
            else {
                static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T gate for density matrix simulation of first multiplication.");
            }
        }
    }
}


template <ket::Gate GateType>
void apply_1t_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair
)
{
    constexpr auto plus_half = std::complex<double> {0.5, 0.5};
    constexpr auto minus_half = std::complex<double> {0.5, -0.5};
    constexpr auto plus_rt2 = std::complex<double> {M_SQRT1_2, M_SQRT1_2};
    constexpr auto minus_rt2 = std::complex<double> {M_SQRT1_2, -M_SQRT1_2};

    using G = ket::Gate;

    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_col0, i_col1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_row0, i_row1] = pair_iterator_inner.next();

            const auto buf00 = buffer(i_row0, i_col0);
            const auto buf10 = buffer(i_row1, i_col0);
            const auto buf01 = buffer(i_row0, i_col1);
            const auto buf11 = buffer(i_row1, i_col1);

            if constexpr (GateType == G::H) {
                state.matrix()(i_row0, i_col0) = M_SQRT1_2 * (buf00 + buf01);
                state.matrix()(i_row1, i_col0) = M_SQRT1_2 * (buf10 + buf11);
                state.matrix()(i_row0, i_col1) = M_SQRT1_2 * (buf00 - buf01);
                state.matrix()(i_row1, i_col1) = M_SQRT1_2 * (buf10 - buf11);
            }
            else if constexpr (GateType == G::X) {
                state.matrix()(i_row0, i_col0) = buf01;
                state.matrix()(i_row1, i_col0) = buf11;
                state.matrix()(i_row0, i_col1) = buf00;
                state.matrix()(i_row1, i_col1) = buf10;
            }
            else if constexpr (GateType == G::Y) {
                state.matrix()(i_row0, i_col0) = {-buf01.imag(),  buf01.real()};
                state.matrix()(i_row1, i_col0) = {-buf11.imag(),  buf11.real()};
                state.matrix()(i_row0, i_col1) = { buf00.imag(), -buf00.real()};
                state.matrix()(i_row1, i_col1) = { buf10.imag(), -buf10.real()};
            }
            else if constexpr (GateType == G::Z) {
                state.matrix()(i_row0, i_col0) =  buf00;
                state.matrix()(i_row1, i_col0) =  buf10;
                state.matrix()(i_row0, i_col1) = -buf01;
                state.matrix()(i_row1, i_col1) = -buf11;
            }
            else if constexpr (GateType == G::S) {
                state.matrix()(i_row0, i_col0) = buf00;
                state.matrix()(i_row1, i_col0) = buf10;
                state.matrix()(i_row0, i_col1) = { buf01.imag(), -buf01.real()};
                state.matrix()(i_row1, i_col1) = { buf11.imag(), -buf11.real()};
            }
            else if constexpr (GateType == Gate::SDAG) {
                state.matrix()(i_row0, i_col0) = buf00;
                state.matrix()(i_row1, i_col0) = buf10;
                state.matrix()(i_row0, i_col1) = {-buf01.imag(),  buf01.real()};
                state.matrix()(i_row1, i_col1) = {-buf11.imag(),  buf11.real()};
            }
            else if constexpr (GateType == Gate::T) {
                state.matrix()(i_row0, i_col0) = buf00;
                state.matrix()(i_row1, i_col0) = buf10;
                state.matrix()(i_row0, i_col1) = buf01 * minus_rt2;
                state.matrix()(i_row1, i_col1) = buf11 * minus_rt2;
            }
            else if constexpr (GateType == Gate::TDAG) {
                state.matrix()(i_row0, i_col0) = buf00;
                state.matrix()(i_row1, i_col0) = buf10;
                state.matrix()(i_row0, i_col1) = buf01 * plus_rt2;
                state.matrix()(i_row1, i_col1) = buf11 * plus_rt2;
            }
            else if constexpr (GateType == Gate::SX) {
                state.matrix()(i_row0, i_col0) = (buf00 * minus_half) + (buf01 * plus_half);
                state.matrix()(i_row1, i_col0) = (buf10 * minus_half) + (buf11 * plus_half);
                state.matrix()(i_row0, i_col1) = (buf00 * plus_half) + (buf01 * minus_half);
                state.matrix()(i_row1, i_col1) = (buf10 * plus_half) + (buf11 * minus_half);
            }
            else if constexpr (GateType == Gate::SXDAG) {
                state.matrix()(i_row0, i_col0) = (buf00 * plus_half) + (buf01 * minus_half);
                state.matrix()(i_row1, i_col0) = (buf10 * plus_half) + (buf11 * minus_half);
                state.matrix()(i_row0, i_col1) = (buf00 * minus_half) + (buf01 * plus_half);
                state.matrix()(i_row1, i_col1) = (buf10 * minus_half) + (buf11 * plus_half);
            }
            else {
                static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T gate for density matrix simulation of second multiplication.");
            }
        }
    }
}


template <ket::Gate GateType>
void apply_1t1a_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle
)
{
    using G = ket::Gate;

    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_row0, i_row1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_col0, i_col1] = pair_iterator_inner.next();

            const auto rho00 = state.matrix()(i_row0, i_col0);
            const auto rho10 = state.matrix()(i_row1, i_col0);
            const auto rho01 = state.matrix()(i_row0, i_col1);
            const auto rho11 = state.matrix()(i_row1, i_col1);

            if constexpr (GateType == G::RX) {
                const auto cost = std::cos(angle / 2.0);
                const auto neg_i_sint = std::complex<double> {0.0, -std::sin(angle / 2.0)};

                buffer(i_row0, i_col0) = (rho00 * cost) + (rho10 * neg_i_sint);
                buffer(i_row1, i_col0) = (rho00 * neg_i_sint) + (rho10 * cost);
                buffer(i_row0, i_col1) = (rho01 * cost) + (rho11 * neg_i_sint);
                buffer(i_row1, i_col1) = (rho01 * neg_i_sint) + (rho11 * cost);
            }
            else if constexpr (GateType == G::RY) {
                const auto cost = std::cos(angle / 2.0);
                const auto sint = std::sin(angle / 2.0);

                buffer(i_row0, i_col0) = (rho00 * cost) - (rho10 * sint);
                buffer(i_row1, i_col0) = (rho00 * sint) + (rho10 * cost);
                buffer(i_row0, i_col1) = (rho01 * cost) - (rho11 * sint);
                buffer(i_row1, i_col1) = (rho01 * sint) + (rho11 * cost);
            }
            else if constexpr (GateType == G::RZ) {
                const auto cost = std::cos(angle / 2.0);
                const auto sint = std::sin(angle / 2.0);
                const auto plus = std::complex<double> {cost, sint};
                const auto minus = std::complex<double> {cost, -sint};

                buffer(i_row0, i_col0) = rho00 * minus;
                buffer(i_row1, i_col0) = rho10 * plus;
                buffer(i_row0, i_col1) = rho01 * minus;
                buffer(i_row1, i_col1) = rho11 * plus;
            }
            else if constexpr (GateType == G::P) {
                const auto cost = std::cos(angle);
                const auto sint = std::sin(angle);
                const auto plus = std::complex<double> {cost, sint};

                buffer(i_row0, i_col0) = rho00;
                buffer(i_row1, i_col0) = rho10 * plus;
                buffer(i_row0, i_col1) = rho01;
                buffer(i_row1, i_col1) = rho11 * plus;
            }
            else {
                static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T1A gate for density matrix simulation of first multiplication.");
            }
        }
    }
}


template <ket::Gate GateType>
void apply_1t1a_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle
)
{
    using G = ket::Gate;

    pair_iterator_outer.set_state(0);
    for (auto i_pair_outer {pair.i_lower}; i_pair_outer < pair.i_upper; ++i_pair_outer) {
        const auto [i_col0, i_col1] = pair_iterator_outer.next();

        pair_iterator_inner.set_state(pair.i_lower);
        for (auto i_pair {pair.i_lower}; i_pair < pair.i_upper; ++i_pair) {
            const auto [i_row0, i_row1] = pair_iterator_inner.next();

            const auto buf00 = buffer(i_row0, i_col0);
            const auto buf10 = buffer(i_row1, i_col0);
            const auto buf01 = buffer(i_row0, i_col1);
            const auto buf11 = buffer(i_row1, i_col1);

            if constexpr (GateType == G::RX) {
                const auto cost = std::cos(angle / 2.0);
                const auto i_sint = std::complex<double> {0.0, std::sin(angle / 2.0)};

                state.matrix()(i_row0, i_col0) = (buf00 * cost) + (buf01 * i_sint);
                state.matrix()(i_row1, i_col0) = (buf10 * cost) + (buf11 * i_sint);
                state.matrix()(i_row0, i_col1) = (buf00 * i_sint) + (buf01 * cost);
                state.matrix()(i_row1, i_col1) = (buf10 * i_sint) + (buf11 * cost);
            }
            else if constexpr (GateType == G::RY) {
                const auto cost = std::cos(angle / 2.0);
                const auto sint = std::sin(angle / 2.0);

                state.matrix()(i_row0, i_col0) = (buf00 * cost) - (buf01 * sint);
                state.matrix()(i_row1, i_col0) = (buf10 * cost) - (buf11 * sint);
                state.matrix()(i_row0, i_col1) = (buf00 * sint) + (buf01 * cost);
                state.matrix()(i_row1, i_col1) = (buf10 * sint) + (buf11 * cost);
            }
            else if constexpr (GateType == G::RZ) {
                const auto cost = std::cos(angle / 2.0);
                const auto sint = std::sin(angle / 2.0);
                const auto plus = std::complex<double> {cost, sint};
                const auto minus = std::complex<double> {cost, -sint};

                state.matrix()(i_row0, i_col0) = (buf00 * plus);
                state.matrix()(i_row1, i_col0) = (buf10 * plus);
                state.matrix()(i_row0, i_col1) = (buf01 * minus);
                state.matrix()(i_row1, i_col1) = (buf11 * minus);
            }
            else if constexpr (GateType == G::P) {
                const auto cost = std::cos(angle);
                const auto sint = std::sin(angle);
                const auto minus = std::complex<double> {cost, -sint};

                state.matrix()(i_row0, i_col0) = buf00;
                state.matrix()(i_row1, i_col0) = buf10;
                state.matrix()(i_row0, i_col1) = buf01 * minus;
                state.matrix()(i_row1, i_col1) = buf11 * minus;
            }
            else {
                static_assert(dm_gate_always_false<GateType>::value, "Invalid 1T1A gate for density matrix simulation of second multiplication.");
            }
        }
    }
}

void apply_u_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
);

void apply_u_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat_adj
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

template <ket::Gate GateType>
void apply_1c1t_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair
)
{
    using G = ket::Gate;

    // TODO: because we know the matrices beforehand, we can cut down on the number of calculations
    // needed for specific gates; but this requires a lot of repetition and a bunch of handwritten
    // calculations, and is just a performance enhancement, so it isn't the biggest priority right now
    if constexpr (GateType == G::CH) {
        const auto mat = ket::h_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CX) {
        const auto mat = ket::x_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CY) {
        const auto mat = ket::y_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CZ) {
        const auto mat = ket::z_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CS) {
        const auto mat = ket::s_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSDAG) {
        const auto mat = ket::sdag_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CT) {
        const auto mat = ket::t_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CTDAG) {
        const auto mat = ket::tdag_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSX) {
        const auto mat = ket::sx_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSXDAG) {
        const auto mat = ket::sxdag_gate();
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else {
        static_assert(dm_gate_always_false<GateType>::value, "Invalid 1C1T gate for density matrix simulation of first multiplication.");
    }
}

template <ket::Gate GateType>
void apply_1c1t_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair
)
{
    using G = ket::Gate;

    // TODO: because we know the matrices beforehand, we can cut down on the number of calculations
    // needed for specific gates; but this requires a lot of repetition and a bunch of handwritten
    // calculations, and is just a performance enhancement, so it isn't the biggest priority right now
    if constexpr (GateType == G::CH) {
        const auto mat = ket::h_gate();
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CX) {
        const auto mat = ket::x_gate();
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CY) {
        const auto mat = ket::y_gate();
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CZ) {
        const auto mat = ket::z_gate();
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CS) {
        const auto mat = ket::conjugate_transpose(ket::s_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSDAG) {
        const auto mat = ket::conjugate_transpose(ket::sdag_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CT) {
        const auto mat = ket::conjugate_transpose(ket::t_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CTDAG) {
        const auto mat = ket::conjugate_transpose(ket::tdag_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSX) {
        const auto mat = ket::conjugate_transpose(ket::sx_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CSXDAG) {
        const auto mat = ket::conjugate_transpose(ket::sxdag_gate());
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else {
        static_assert(dm_gate_always_false<GateType>::value, "Invalid 1C1T gate for density matrix simulation of second multiplication.");
    }
}


template <ket::Gate GateType>
void apply_1c1t1a_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle
)
{
    using G = ket::Gate;

    // TODO: because we know the matrices beforehand, we can cut down on the number of calculations
    // needed for specific gates; but this requires a lot of repetition and a bunch of handwritten
    // calculations, and is just a performance enhancement, so it isn't the biggest priority right now
    if constexpr (GateType == G::CRX) {
        const auto mat = ket::rx_gate(angle);
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CRY) {
        const auto mat = ket::ry_gate(angle);
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CRZ) {
        const auto mat = ket::rz_gate(angle);
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CP) {
        const auto mat = ket::p_gate(angle);
        apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else {
        static_assert(dm_gate_always_false<GateType>::value, "Invalid 1C1T1A gate for density matrix simulation of first multiplication.");
    }
}

template <ket::Gate GateType>
void apply_1c1t1a_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle
)
{
    using G = ket::Gate;

    // TODO: because we know the matrices beforehand, we can cut down on the number of calculations
    // needed for specific gates; but this requires a lot of repetition and a bunch of handwritten
    // calculations, and is just a performance enhancement, so it isn't the biggest priority right now
    if constexpr (GateType == G::CRX) {
        const auto mat = ket::conjugate_transpose(ket::rx_gate(angle));
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CRY) {
        const auto mat = ket::conjugate_transpose(ket::ry_gate(angle));
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CRZ) {
        const auto mat = ket::conjugate_transpose(ket::rz_gate(angle));
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else if constexpr (GateType == G::CP) {
        const auto mat = ket::conjugate_transpose(ket::p_gate(angle));
        apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
    }
    else {
        static_assert(dm_gate_always_false<GateType>::value, "Invalid 1C1T1A gate for density matrix simulation of second multiplication.");
    }
}

}  // namespace ket::internal

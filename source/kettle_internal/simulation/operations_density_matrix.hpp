#pragma once

#include <Eigen/Dense>
#include <cmath>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/density_matrix.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/parameter/parameter_expression_internal.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/simulate_utils.hpp"

/*
    This header file contains the common operations performed on two states in the
    Statevector object.
*/

/*
    TODO: most of these simulation functions can be specialized because we know the gate type
    beforehand and we can cut down on the number of calculations per transformation by choosing
    the specific form of the transformation at compile-time
    
    We already do this with many other simulation functions, where the overall body of the
    simulation function is smaller and the repetition isn't as horrid
    
    However, the controlled gate transformations for the density matrices take a lot of code
    and require A LOT of repetition to specialize for each gate type, so we aren't doing them;
    they are just a performance enchangement, which isn't the biggest priority right now
*/

namespace ket::internal
{

constexpr static auto MEASURING_THREAD_ID = int {0};

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

void apply_left_one_qubit_matrix_(
    const Eigen::MatrixXcd& original_state,
    Eigen::MatrixXcd& output_buffer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    SingleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    const ket::Matrix2X2& mat
);

void apply_right_one_qubit_matrix_(
    const Eigen::MatrixXcd& left_product,
    Eigen::MatrixXcd& output_buffer,
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

void apply_1c1t_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    ket::Gate gate
);

void apply_1c1t_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    ket::Gate gate
);

void apply_1c1t1a_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle,
    ket::Gate gate
);

void apply_1c1t1a_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle,
    ket::Gate gate
);

void simulate_u_gate_(
    Eigen::MatrixXcd& state,
    Eigen::MatrixXcd& buffer,
    Eigen::Index target_index,
    Eigen::Index n_qubits,
    const ket::Matrix2X2& mat,
    const FlatIndexPair<Eigen::Index>& pair
);

template <ket::Gate GateType>
void simulate_one_target_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto target_index = static_cast<Eigen::Index>(create::unpack_single_qubit_gate_index(info));
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());
    auto pair_iterator_outer = SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    apply_1t_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    apply_1t_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair);
}

template <ket::Gate GateType>
void simulate_one_target_one_angle_gate_(
    const ket::param::internal::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    namespace kpi = ket::param::internal;

    const auto [target_index_st, theta] = kpi::unpack_target_and_angle(parameter_values_map, info);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = SingleQubitGatePairGenerator {target_index, n_qubits};
    auto pair_iterator_inner = SingleQubitGatePairGenerator {target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    apply_1t1a_gate_first_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    apply_1t1a_gate_second_<GateType>(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta);
}


void simulate_cu_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
);


template <ket::Gate GateType>
void simulate_one_control_one_target_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    const auto [control_index_st, target_index_st] = create::unpack_double_qubit_gate_indices(info);
    const auto control_index = static_cast<Eigen::Index>(control_index_st);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    auto pair_iterator_inner = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    apply_1c1t_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, info.gate);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    apply_1c1t_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, info.gate);
}


template <ket::Gate GateType>
void simulate_one_control_one_target_one_angle_gate_(
    const ket::param::internal::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const FlatIndexPair<Eigen::Index>& pair,
    Eigen::MatrixXcd& buffer
)
{
    namespace kpi = ket::param::internal;

    const auto [control_index_st, target_index_st, theta] = kpi::unpack_control_target_and_angle(parameter_values_map, info);
    const auto control_index = static_cast<Eigen::Index>(control_index_st);
    const auto target_index = static_cast<Eigen::Index>(target_index_st);
    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    auto pair_iterator_outer = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};
    auto pair_iterator_inner = DoubleQubitGatePairGenerator {control_index, target_index, n_qubits};

    // perform the multiplication of U * rho;
    // fill the buffer
    apply_1c1t1a_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta, info.gate);

    // perform the multiplication of (U * rho) * U^t
    // write the result to the density matrix itself
    apply_1c1t1a_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, theta, info.gate);
}

void simulate_gate_info_(
    const ket::param::internal::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const FlatIndexPair<Eigen::Index>& single_pair,
    const FlatIndexPair<Eigen::Index>& double_pair,
    const ket::GateInfo& gate_info,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& c_register,
    Eigen::MatrixXcd& buffer
);

}  // namespace ket::internal

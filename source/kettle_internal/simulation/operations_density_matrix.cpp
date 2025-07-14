#include <cmath>
#include <complex>

#include "kettle/common/matrix2x2.hpp"

#include "kettle_internal/gates/primitive_gate_map.hpp"
#include "kettle_internal/simulation/operations_density_matrix.hpp"
#include "kettle_internal/simulation/measure_density_matrix.hpp"

namespace kpi = ket::param::internal;


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

void apply_1c1t_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    ket::Gate gate
)
{
    // TODO: maybe specialize in the future like all other simulation functions
    const auto mat_func = GATE_TO_MATRIX_FUNCTION_NO_PARAM.at(gate);
    const auto mat = mat_func();
    apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
}

void apply_1c1t_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    ket::Gate gate
)
{
    // TODO: maybe specialize in the future like all other simulation functions
    const auto mat_func = GATE_TO_MATRIX_FUNCTION_NO_PARAM.at(gate);
    const auto mat = ket::conjugate_transpose(mat_func());
    apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
}

void apply_1c1t1a_gate_first_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle,
    ket::Gate gate
)
{
    // TODO: maybe specialize in the future like all other simulation functions
    const auto mat_func = GATE_TO_MATRIX_FUNCTION_PARAM.at(gate);
    const auto mat = mat_func(angle);
    apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
}

void apply_1c1t1a_gate_second_(
    ket::DensityMatrix& state,
    Eigen::MatrixXcd& buffer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_outer,
    DoubleQubitGatePairGenerator<Eigen::Index>& pair_iterator_inner,
    const FlatIndexPair<Eigen::Index>& pair,
    double angle,
    ket::Gate gate
)
{
    // TODO: maybe specialize in the future like all other simulation functions
    const auto mat_func = GATE_TO_MATRIX_FUNCTION_PARAM.at(gate);
    const auto mat = ket::conjugate_transpose(mat_func(angle));
    apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);
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


void simulate_cu_gate_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    const ket::Matrix2X2& mat,
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

    // perform the multiplication of U * rho; fill the buffer
    apply_cu_gate_first_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat);

    const auto mat_adj = ket::conjugate_transpose(mat);

    // perform the multiplication of (U * rho) * U^t; write the result to the density matrix itself
    apply_cu_gate_second_(state, buffer, pair_iterator_outer, pair_iterator_inner, pair, mat_adj);
}


void simulate_gate_info_(
    const kpi::MapVariant& parameter_values_map,
    ket::DensityMatrix& state,
    const FlatIndexPair<Eigen::Index>& single_pair,
    const FlatIndexPair<Eigen::Index>& double_pair,
    const ket::GateInfo& gate_info,
    int thread_id,
    std::optional<int> prng_seed,
    ket::ClassicalRegister& c_register,
    Eigen::MatrixXcd& buffer
)
{
    namespace cre = create;
    using G = ket::Gate;

    const auto n_qubits = static_cast<Eigen::Index>(state.n_qubits());

    switch (gate_info.gate) {
        case G::H : {
            simulate_one_target_gate_<G::H>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::X : {
            simulate_one_target_gate_<G::X>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::Y : {
            simulate_one_target_gate_<G::Y>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::Z : {
            simulate_one_target_gate_<G::Z>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::S : {
            simulate_one_target_gate_<G::S>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SDAG : {
            simulate_one_target_gate_<G::SDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::T : {
            simulate_one_target_gate_<G::T>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::TDAG : {
            simulate_one_target_gate_<G::TDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SX : {
            simulate_one_target_gate_<G::SX>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::SXDAG : {
            simulate_one_target_gate_<G::SXDAG>(state, gate_info, single_pair, buffer);
            break;
        }
        case G::RX : {
            simulate_one_target_one_angle_gate_<G::RX>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::RY : {
            simulate_one_target_one_angle_gate_<G::RY>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::RZ : {
            simulate_one_target_one_angle_gate_<G::RZ>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::P : {
            simulate_one_target_one_angle_gate_<G::P>(parameter_values_map, state, gate_info, single_pair, buffer);
            break;
        }
        case G::CH : {
            simulate_one_control_one_target_gate_<G::CH>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CX : {
            simulate_one_control_one_target_gate_<G::CX>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CY : {
            simulate_one_control_one_target_gate_<G::CY>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CZ : {
            simulate_one_control_one_target_gate_<G::CZ>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CS : {
            simulate_one_control_one_target_gate_<G::CS>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSDAG : {
            simulate_one_control_one_target_gate_<G::CSDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CT : {
            simulate_one_control_one_target_gate_<G::CT>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CTDAG : {
            simulate_one_control_one_target_gate_<G::CTDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSX : {
            simulate_one_control_one_target_gate_<G::CSX>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CSXDAG : {
            simulate_one_control_one_target_gate_<G::CSXDAG>(state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRX : {
            simulate_one_control_one_target_one_angle_gate_<G::CRX>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRY : {
            simulate_one_control_one_target_one_angle_gate_<G::CRY>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CRZ : {
            simulate_one_control_one_target_one_angle_gate_<G::CRZ>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::CP : {
            simulate_one_control_one_target_one_angle_gate_<G::CP>(parameter_values_map, state, gate_info, double_pair, buffer);
            break;
        }
        case G::U : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            const auto target_index = static_cast<Eigen::Index>(cre::unpack_single_qubit_gate_index(gate_info));
            simulate_u_gate_(state.matrix(), buffer, target_index, n_qubits, *unitary_ptr, single_pair);
            break;
        }
        case G::CU : {
            const auto& unitary_ptr = cre::unpack_unitary_matrix(gate_info);
            simulate_cu_gate_(state, gate_info, *unitary_ptr, double_pair, buffer);
            break;
        }
        case G::M : {
            // this operation is more complicated to make multithreaded because the threads have already been
            // spawned before entering the simulation loop; thus, it is easier to just make the measurement
            // a single-threaded operation
            if (thread_id == MEASURING_THREAD_ID) {
                [[maybe_unused]]
                const auto [ignore, bit_index] = cre::unpack_m_gate(gate_info);
                const auto measured = simulate_measurement_(state, gate_info, prng_seed);
                c_register.set(bit_index, measured);
            }
            break;
        }
        default : {
            break;
        }
    }
}


}  // namespace ket::internal

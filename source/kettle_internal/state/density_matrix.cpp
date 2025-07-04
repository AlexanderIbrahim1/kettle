#include <stdexcept>

#include <Eigen/Dense>

#include "kettle/state/density_matrix.hpp"
#include "kettle/state/statevector.hpp"

#include "kettle_internal/common/mathtools_internal.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"

namespace ki = ket::internal;

namespace
{

void check_is_hermitian_(const Eigen::MatrixXcd& matrix, double tolerance) {
    if (!matrix.isApprox(matrix.adjoint(), tolerance)) {
        throw std::runtime_error {"ERROR: provided matrix is not Hermitian.\n"};
    }
}

void check_is_finite_size_(const Eigen::MatrixXcd& matrix)
{
    if (matrix.size() == 0) {
        throw std::runtime_error {"ERROR: provided matrix has a size of 0 x 0.\n"};
    }
}

void check_is_square_matrix_(const Eigen::MatrixXcd& matrix)
{
    if (matrix.cols() != matrix.rows()) {
        throw std::runtime_error {"ERROR: provided matrix is not square.\n"};
    }
}

void check_has_trace_of_one_(const Eigen::MatrixXcd& matrix, double tolerance)
{
    const auto trace = matrix.trace();
    const auto unity = std::complex<double> {1.0, 0.0};

    if (std::norm(trace - unity) > tolerance) {
        throw std::runtime_error {"ERROR: provided matrix does not have a trace of 1.\n"};
    }
}

void check_is_positive_semi_definite_(const Eigen::MatrixXcd& matrix)
{
    // perform the LDL^T decomposition, which should be faster than finding all the eigenvalues
    // an earlier check should have determined if the matrix is Hermitian
    const auto ldlt = Eigen::LDLT<Eigen::MatrixXcd> {matrix};
    if (!ldlt.isPositive() || (ldlt.info() != Eigen::Success)) {
        throw std::runtime_error {"ERROR: provided matrix is not positive semidefinite.\n"};
    }
}

void check_side_length_is_power_of_2_(const Eigen::MatrixXcd& matrix)
{
    if (!ki::is_power_of_2(matrix.cols())) {
        auto err_msg = std::stringstream {};
        err_msg << "The provided coefficients must have a size equal to a power of 2.\n";
        err_msg << "Found size = " << matrix.cols();
        throw std::runtime_error {err_msg.str()};
    }
}

}  // namespace


namespace ket
{

DensityMatrix::DensityMatrix(Eigen::MatrixXcd matrix, double trace_tolerance, double hermitian_tolerance)
    : n_qubits_ {ki::log_2_int(matrix.cols())}
    , n_states_ {static_cast<std::size_t>(matrix.cols())}
    , matrix_ {std::move(matrix)}
{
    check_is_finite_size_(matrix_);
    check_is_square_matrix_(matrix_);
    check_has_trace_of_one_(matrix_, trace_tolerance);
    check_is_hermitian_(matrix_, hermitian_tolerance);
    check_is_positive_semi_definite_(matrix_);
    check_side_length_is_power_of_2_(matrix_);
}

DensityMatrix::DensityMatrix(Eigen::MatrixXcd matrix, [[maybe_unused]] const density_matrix_nocheck& key)
    : n_qubits_ {ki::log_2_int(matrix.cols())}
    , n_states_ {static_cast<std::size_t>(matrix.cols())}
    , matrix_ {std::move(matrix)}
{}

DensityMatrix::DensityMatrix(const std::string& bitstring, Endian input_endian)
    : n_qubits_ {bitstring.size()}
    , n_states_ {ki::pow_2_int(bitstring.size())}
{
    const auto index = static_cast<Eigen::Index>(bitstring_to_state_index(bitstring, input_endian));

    const auto size = static_cast<Eigen::Index>(n_states_);
    matrix_ = Eigen::MatrixXcd::Zero(size, size).eval();
    matrix_(index, index) = 1.0;
}

[[nodiscard]]
auto DensityMatrix::is_pure(double tolerance) const -> bool
{
    const auto unity = std::complex<double> {1.0, 0.0};
    return std::norm(trace_of_square() - unity) <= tolerance;
}

[[nodiscard]]
auto DensityMatrix::trace() const -> std::complex<double>
{
    return matrix_.trace();
}

[[nodiscard]]
auto DensityMatrix::trace_of_square() const -> std::complex<double>
{
    const auto squared = matrix_ * matrix_;
    return squared.trace();
}

auto statevector_to_density_matrix(const Statevector& statevector) -> DensityMatrix
{
    const auto n_states = statevector.n_states();

    auto dens_mat = Eigen::MatrixXcd(n_states, n_states);

    for (std::size_t i0 {0}; i0 < n_states; ++i0) {
        const auto idx0 = static_cast<Eigen::Index>(i0);
        for (std::size_t i1 {0}; i1 < n_states; ++i1) {
            const auto idx1 = static_cast<Eigen::Index>(i1);
            dens_mat(idx0, idx1) = statevector[i0] * std::conj(statevector[i1]);
        }
    }

    return DensityMatrix {std::move(dens_mat), density_matrix_nocheck {}};
}

auto tensor_product(const DensityMatrix& left, const DensityMatrix& right) -> DensityMatrix
{
    const auto a_rows = left.matrix().rows();
    const auto a_cols = left.matrix().cols();
    const auto b_rows = right.matrix().rows();
    const auto b_cols = right.matrix().cols();

    auto result = Eigen::MatrixXcd{a_rows * b_rows, a_cols * b_cols};

    for (Eigen::Index i_col_left = 0; i_col_left < a_cols; ++i_col_left) {
        for (Eigen::Index i_row_left = 0; i_row_left < a_rows; ++i_row_left) {
            const auto left_val = left.matrix()(i_row_left, i_col_left);
            for (Eigen::Index i_col_right = 0; i_col_right < b_cols; ++i_col_right) {
                for (Eigen::Index i_row_right = 0; i_row_right < b_rows; ++i_row_right) {
                    const auto right_val = right.matrix()(i_row_right, i_col_right);

                    const auto i_result_row = (i_row_right * a_rows) + i_row_left;
                    const auto i_result_col = (i_col_right * a_cols) + i_col_left;

                    result(i_result_row, i_result_col) = left_val * right_val;
                }
            }
        }
    }

    return DensityMatrix {result};
}

auto partial_trace(const DensityMatrix& density_matrix, std::vector<std::size_t> qubit_indices) -> DensityMatrix
{
    // NOTE: the qubit indices *could* be passed in as `Eigen::Index` to avoid the casting, but since qubit
    // indices are passed as type `std::size_t` everywhere else, we'll just deal with it to avoid annoying
    // the user; not a big deal anyways

    const auto is_out_of_range = [n_qubits=density_matrix.n_qubits()](auto index) { return index >= n_qubits; };
    if (std::ranges::any_of(qubit_indices, is_out_of_range)) {
        throw std::runtime_error {"ERROR: Cannot take partial trace; found qubit index out of bounds\n"};
    }

    const auto rev_sort_predicate = [](auto x, auto y) { return x > y; };
    std::ranges::sort(qubit_indices, rev_sort_predicate);

    auto current = density_matrix.matrix();
    const auto n_qubits = static_cast<Eigen::Index>(density_matrix.n_qubits());

    // TODO: this can be parallelized
    for (std::size_t i {0}; i < qubit_indices.size(); ++i) {
        const auto i_qubit = static_cast<Eigen::Index>(qubit_indices[i]);

        const auto n_qubits_current = n_qubits - static_cast<Eigen::Index>(i);
        auto col_pair_iter = ki::SingleQubitGatePairGenerator {i_qubit, n_qubits_current};
        auto row_pair_iter = ki::SingleQubitGatePairGenerator {i_qubit, n_qubits_current};

        const auto n_qubits_next = n_qubits_current - 1;
        const auto new_size = ki::pow_2_int(n_qubits_next);
        auto reduced = Eigen::MatrixXcd {new_size, new_size};

        col_pair_iter.set_state(0);
        for (Eigen::Index i_col {0}; i_col < new_size; ++i_col) {
            const auto [i_col0, i_col1] = col_pair_iter.next();
            row_pair_iter.set_state(0);
            for (Eigen::Index i_row {0}; i_row < new_size; ++i_row) {
                const auto [i_row0, i_row1] = row_pair_iter.next();

                reduced(i_row, i_col) = current(i_row0, i_col0) + current(i_row1, i_col1);
            }
        }

        current = reduced;
    }

    return DensityMatrix {current};
}

}  // namespace ket

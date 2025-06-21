#include <stdexcept>

#include <Eigen/Dense>

#include "kettle/state/density_matrix.hpp"
#include "kettle/state/statevector.hpp"

#include "kettle_internal/common/mathtools_internal.hpp"

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

}  // namespace ket

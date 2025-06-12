#pragma once

#include <stdexcept>

#include <Eigen/Dense>

#include "kettle/common/tolerance.hpp"

// TODO: to implement;
//   - multiplying by a scalar
//     - NOTE: maybe some of these conditions are too strict?
//     - sometimes intermediate calculations require multiplication by a scalar, but this won't conserve the invariants
//     - one solution is to create specific functions for all operations,
//       and make sure that the functions themselves cause the invariants to be satisfied
//       - for example, a function for probabilistic mixtures, another for doing something -> renormalizing
//     - another is to just get rid of the invariant checking


namespace ket
{

inline void check_is_hermitian_(const Eigen::MatrixXcd& matrix, double tolerance = MATRIX_HERMITIAN_TOLERANCE) {
    if ((matrix - matrix.adjoint()).norm() > tolerance) {
        throw std::runtime_error {"ERROR: provided matrix is not Hermitian.\n"};
    }
}

inline void check_is_finite_size_(const Eigen::MatrixXcd& matrix)
{
    if (matrix.size() == 0) {
        throw std::runtime_error {"ERROR: provided matrix has a size of 0 x 0.\n"};
    }
}

inline void check_is_square_matrix_(const Eigen::MatrixXcd& matrix)
{
    if (matrix.cols() != matrix.rows()) {
        throw std::runtime_error {"ERROR: provided matrix is not square.\n"};
    }
}

inline void check_has_trace_of_one_(const Eigen::MatrixXcd& matrix, double tolerance = DENSITY_MATRIX_TRACE_TOLERANCE)
{
    const auto trace = matrix.trace();
    const auto unity = std::complex<double> {1.0, 0.0};

    if (std::norm(trace - unity) > tolerance) {
        throw std::runtime_error {"ERROR: provided matrix has a size of 0 x 0.\n"};
    }
}

inline void check_is_positive_semi_definite_(const Eigen::MatrixXcd& matrix)
{
    const auto ldlt = Eigen::LDLT<Eigen::MatrixXcd> {matrix};
    if (!ldlt.isPositive() || (ldlt.info() != Eigen::Success)) {
        throw std::runtime_error {"ERROR: provided matrix is not positive semidefinite\n."};
    }
}

class DensityMatrix
{
public:
    explicit DensityMatrix(
        Eigen::MatrixXcd matrix,
        double trace_tolerance = DENSITY_MATRIX_TRACE_TOLERANCE,
        double hermitian_tolerance = MATRIX_HERMITIAN_TOLERANCE
    )
        : matrix_ {std::move(matrix)}
    {
        check_is_finite_size_(matrix_);
        check_is_square_matrix_(matrix_);
        check_has_trace_of_one_(matrix_, trace_tolerance);
        check_is_hermitian_(matrix_, hermitian_tolerance);
        check_is_positive_semi_definite_(matrix_);
    }

    [[nodiscard]]
    auto is_pure(double tolerance = DENSITY_MATRIX_TRACE_TOLERANCE) const -> bool
    {
        const auto square = matrix_ * matrix_;

        const auto trace = square.trace();
        const auto unity = std::complex<double> {1.0, 0.0};

        return std::norm(trace - unity) <= tolerance;
    }

    [[nodiscard]]
    constexpr auto matrix() const -> const Eigen::MatrixXcd&
    {
        return matrix_;
    }

    [[nodiscard]]
    constexpr auto matrix() -> Eigen::MatrixXcd&
    {
        return matrix_;
    }

private:
    Eigen::MatrixXcd matrix_;
};

}  // namespace ket

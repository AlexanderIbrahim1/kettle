#pragma once

#include <Eigen/Dense>

#include "kettle/common/tolerance.hpp"
#include "kettle/state/statevector.hpp"

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

struct density_matrix_nocheck {};

class DensityMatrix
{
public:
    /*
        Construct the `DensityMatrix` type from an existing `Eigen::MatrixXcd` instance `matrix`.
        
        This constructor checks if the instance provided is a valid density matrix. It checks if
        the matrix is:
          - a square matrix with a side length equal to a positive power of 2
          - Hermitian (such that `|matrix - matrix.adjoint()| < hermitian_tolerance`)
          - of unit trace (such that `|Trace(matrix) - 1|^2 < trace_tolerance`)
          - positive semi-definite
    */
    explicit DensityMatrix(
        Eigen::MatrixXcd matrix,
        double trace_tolerance = DENSITY_MATRIX_TRACE_TOLERANCE,
        double hermitian_tolerance = MATRIX_HERMITIAN_TOLERANCE
    );

    /*
        Construct the `DensityMatrix` type from an existing `Eigen::MatrixXcd` instance `matrix`.

        All checks that determine if a density matrix is valid are all skipped.
    */
    explicit DensityMatrix(
        Eigen::MatrixXcd matrix,
        [[maybe_unused]] const density_matrix_nocheck& key
    );

    /*
        Construct the `DensityMatrix` type for a single computational basis state, represented by
        the bitstring `bistring`. The endianness of the input is provided by `input_endian`.

        The resulting density matrix will have a 0 for every entry, except for the diagonal element
        given by the index calculated from the bistring.
    */
    explicit DensityMatrix(
        const std::string& bitstring,
        Endian input_endian = Endian::LITTLE
    );

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

    [[nodiscard]]
    constexpr auto n_states() const noexcept -> std::size_t
    {
        return n_states_;
    }

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    /*
        Check if the trace of the square of the density matrix is equal to unity, such that
            `|Trace(matrix^2) - 1| <= tolerance`
    */
    [[nodiscard]]
    auto is_pure(double tolerance = DENSITY_MATRIX_TRACE_TOLERANCE) const -> bool;

    /*
        Get the trace of the matrix.
    */
    [[nodiscard]]
    auto trace() const -> std::complex<double>;

    /*
        Get the trace of the square of the matrix.
    */
    [[nodiscard]]
    auto trace_of_square() const -> std::complex<double>;

private:
    std::size_t n_qubits_;
    std::size_t n_states_;
    Eigen::MatrixXcd matrix_;
};

/*
    Convert the statevector to a density matrix.
*/
auto statevector_to_density_matrix(const Statevector& statevector) -> DensityMatrix;

}  // namespace ket

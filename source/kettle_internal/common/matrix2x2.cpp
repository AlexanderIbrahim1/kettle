#include <cmath>
#include <complex>

#include "kettle/common/mathtools.hpp"
#include "kettle/common/matrix2x2.hpp"

namespace
{

/*
    This function chooses the correct parameters needed to calculate the square root
    of the 2x2 matrix.

    There are four possible square roots of a given 2x2 matrix; the simplest choice,
    where the sign of `s` is positive, isn't always possible because it causes the
    final result to blow up.

    The matrix [-1, 0; 0 -1], or the negative of the identity, is a prime example.
*/
auto matrix_sqrt_parameters_(
    std::complex<double> trace,
    std::complex<double> determinant,
    double tolerance
) -> std::tuple<std::complex<double>, std::complex<double>>
{
    const auto s = std::sqrt(determinant);
    const auto t_arg = trace + 2.0 * s;

    if (std::norm(t_arg) > tolerance) {
        const auto t = std::sqrt(t_arg);
        return {s, t};
    } else {
        const auto t = std::sqrt(trace - 2.0 * s);
        return {-s, t};
    }
}

}  // namespace


namespace ket
{

auto Matrix2X2::operator*=(const Matrix2X2& other) noexcept -> Matrix2X2
{
    const auto new00 = elem00 * other.elem00 + elem01 * other.elem10;
    const auto new01 = elem00 * other.elem01 + elem01 * other.elem11;
    const auto new10 = elem10 * other.elem00 + elem11 * other.elem10;
    const auto new11 = elem10 * other.elem01 + elem11 * other.elem11;

    elem00 = new00;
    elem01 = new01;
    elem10 = new10;
    elem11 = new11;

    return *this;
}

template <typename Number>
auto Matrix2X2::operator*=(Number scalar) noexcept -> Matrix2X2
{
    elem00 *= scalar;
    elem01 *= scalar;
    elem10 *= scalar;
    elem11 *= scalar;

    return *this;
}
template auto Matrix2X2::operator*=<double>(double scalar) noexcept -> Matrix2X2;
template auto Matrix2X2::operator*=<std::complex<double>>(std::complex<double> scalar) noexcept -> Matrix2X2;

auto Matrix2X2::operator+=(const Matrix2X2& other) noexcept -> Matrix2X2
{
    const auto new00 = elem00 + other.elem00;
    const auto new01 = elem01 + other.elem01;
    const auto new10 = elem10 + other.elem10;
    const auto new11 = elem11 + other.elem11;

    elem00 = new00;
    elem01 = new01;
    elem10 = new10;
    elem11 = new11;

    return *this;
}

auto operator*(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2
{
    lhs *= rhs;
    return lhs;
}

template <typename Number>
auto operator*(Matrix2X2 lhs, Number scalar) noexcept -> Matrix2X2
{
    lhs *= scalar;
    return lhs;
}

template auto operator*<double>(Matrix2X2 lhs, double scalar) noexcept -> Matrix2X2;
template auto operator*<std::complex<double>>(Matrix2X2 lhs, std::complex<double> scalar) noexcept -> Matrix2X2;

template <typename Number>
auto operator*(Number scalar, Matrix2X2 rhs) noexcept -> Matrix2X2
{
    return operator*(rhs, scalar);
}
template auto operator*<double>(double scalar, Matrix2X2 lhs) noexcept -> Matrix2X2;
template auto operator*<std::complex<double>>(std::complex<double> scalar, Matrix2X2 lhs) noexcept -> Matrix2X2;

auto operator+(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2
{
    lhs += rhs;
    return lhs;
}

auto matrix_square_root(
    const Matrix2X2& mat,
    double matrix_sqrt_tolerance
) -> Matrix2X2
{
    // uses the following: https://en.wikipedia.org/wiki/Square_root_of_a_2_by_2_matrix#A_general_formula
    // we use the solution with the positive roots of s and t
    const auto trace = mat.elem00 + mat.elem11;
    const auto determinant = mat.elem00 * mat.elem11 - mat.elem01 * mat.elem10;

    const auto [s, t] = matrix_sqrt_parameters_(trace, determinant, matrix_sqrt_tolerance);

    const auto new00 = (mat.elem00 + s) / t;
    const auto new01 = mat.elem01 / t;
    const auto new10 = mat.elem10 / t;
    const auto new11 = (mat.elem11 + s) / t;

    return {.elem00=new00, .elem01=new01, .elem10=new10, .elem11=new11};
}

auto conjugate_transpose(const Matrix2X2& mat) -> Matrix2X2
{
    const auto new00 = std::conj(mat.elem00);
    const auto new01 = std::conj(mat.elem10);
    const auto new10 = std::conj(mat.elem01);
    const auto new11 = std::conj(mat.elem11);

    return {.elem00=new00, .elem01=new01, .elem10=new10, .elem11=new11};
}

auto determinant(const Matrix2X2& mat) -> std::complex<double>
{
    return mat.elem00 * mat.elem11 - mat.elem10 * mat.elem01;
}

auto trace(const Matrix2X2& mat) -> std::complex<double>
{
    return mat.elem00 + mat.elem11;
}

auto norm(const Matrix2X2& mat) -> double
{
    // NOTE: there are too few elements in a non-STL object to do anything with the STL iterators
    const auto norm00 = std::norm(mat.elem00);
    const auto norm01 = std::norm(mat.elem01);
    const auto norm10 = std::norm(mat.elem10);
    const auto norm11 = std::norm(mat.elem11);

    return std::sqrt(norm00 + norm01 + norm10 + norm11);
}

auto almost_eq(
    const Matrix2X2& left,
    const Matrix2X2& right,
    double tolerance_sq
) noexcept -> bool
{
    // clang-format off
    return \
        almost_eq(left.elem00, right.elem00, tolerance_sq) && \
        almost_eq(left.elem10, right.elem10, tolerance_sq) && \
        almost_eq(left.elem01, right.elem01, tolerance_sq) && \
        almost_eq(left.elem11, right.elem11, tolerance_sq);
    // clang-format on
}

}  // namespace ket

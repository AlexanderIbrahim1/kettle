#pragma once

#include <cmath>
#include <complex>

#include "kettle/common/mathtools.hpp"

namespace impl_ket
{

/*
    This function chooses the correct parameters needed to calculate the square root
    of the 2x2 matrix.

    There are four possible square roots of a given 2x2 matrix; the simplest choice,
    where the sign of `s` is positive, isn't always possible because it causes the
    final result to blow up.

    The matrix [-1, 0; 0 -1], or the negative of the identity, is a prime example.
*/
inline auto matrix_sqrt_parameters(
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

}  // namespace impl_ket

namespace ket
{

struct Matrix2X2
{
    std::complex<double> elem00;  // top-left
    std::complex<double> elem01;  // top-right
    std::complex<double> elem10;  // bottom-left
    std::complex<double> elem11;  // bottom-right

    constexpr auto operator*=(const Matrix2X2& other) noexcept -> Matrix2X2
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
    constexpr auto operator*=(Number scalar) noexcept -> Matrix2X2
    {
        elem00 *= scalar;
        elem01 *= scalar;
        elem10 *= scalar;
        elem11 *= scalar;

        return *this;
    }

    constexpr auto operator+=(const Matrix2X2& other) noexcept -> Matrix2X2
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
};

constexpr auto operator*(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2
{
    lhs *= rhs;
    return lhs;
}

template <typename Number>
constexpr auto operator*(Matrix2X2 lhs, Number scalar) noexcept -> Matrix2X2
{
    lhs *= scalar;
    return lhs;
}

template <typename Number>
constexpr auto operator*(Number scalar, Matrix2X2 rhs) noexcept -> Matrix2X2
{
    return operator*(rhs, scalar);
}

constexpr auto operator+(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2
{
    lhs += rhs;
    return lhs;
}

inline auto matrix_square_root(
    const Matrix2X2& mat,
    double matrix_sqrt_tolerance = impl_ket::MATRIX_2X2_SQRT_TOLERANCE
) -> Matrix2X2
{
    // uses the following: https://en.wikipedia.org/wiki/Square_root_of_a_2_by_2_matrix#A_general_formula
    // we use the solution with the positive roots of s and t
    const auto trace = mat.elem00 + mat.elem11;
    const auto determinant = mat.elem00 * mat.elem11 - mat.elem01 * mat.elem10;

    const auto [s, t] = impl_ket::matrix_sqrt_parameters(trace, determinant, matrix_sqrt_tolerance);

    const auto new00 = (mat.elem00 + s) / t;
    const auto new01 = mat.elem01 / t;
    const auto new10 = mat.elem10 / t;
    const auto new11 = (mat.elem11 + s) / t;

    return {new00, new01, new10, new11};
}

constexpr auto conjugate_transpose(const Matrix2X2& mat) -> Matrix2X2
{
    const auto new00 = std::conj(mat.elem00);
    const auto new01 = std::conj(mat.elem10);
    const auto new10 = std::conj(mat.elem01);
    const auto new11 = std::conj(mat.elem11);

    return {new00, new01, new10, new11};
}

constexpr auto determinant(const Matrix2X2& mat) -> std::complex<double>
{
    return mat.elem00 * mat.elem11 - mat.elem10 * mat.elem01;
}

constexpr auto trace(const Matrix2X2& mat) -> std::complex<double>
{
    return mat.elem00 + mat.elem11;
}

constexpr auto almost_eq(
    const Matrix2X2& left,
    const Matrix2X2& right,
    double tolerance_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
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

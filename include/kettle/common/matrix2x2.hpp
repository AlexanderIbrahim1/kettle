#pragma once

#include <cmath>
#include <complex>

#include "kettle/common/mathtools.hpp"


namespace ket
{

struct Matrix2X2
{
    std::complex<double> elem00;  // top-left
    std::complex<double> elem01;  // top-right
    std::complex<double> elem10;  // bottom-left
    std::complex<double> elem11;  // bottom-right

    auto operator+=(const Matrix2X2& other) noexcept -> Matrix2X2;

    auto operator*=(const Matrix2X2& other) noexcept -> Matrix2X2;

    template <typename Number>
    auto operator*=(Number scalar) noexcept -> Matrix2X2;
};

auto operator*(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2;

template <typename Number>
auto operator*(Matrix2X2 lhs, Number scalar) noexcept -> Matrix2X2;

template <typename Number>
auto operator*(Number scalar, Matrix2X2 rhs) noexcept -> Matrix2X2;

auto operator+(Matrix2X2 lhs, const Matrix2X2& rhs) noexcept -> Matrix2X2;

auto matrix_square_root(
    const Matrix2X2& mat,
    double matrix_sqrt_tolerance = ket::internal::MATRIX_2X2_SQRT_TOLERANCE
) -> Matrix2X2;

auto conjugate_transpose(const Matrix2X2& mat) -> Matrix2X2;

auto determinant(const Matrix2X2& mat) -> std::complex<double>;

auto trace(const Matrix2X2& mat) -> std::complex<double>;

auto almost_eq(
    const Matrix2X2& left,
    const Matrix2X2& right,
    double tolerance_sq = ket::internal::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool;

}  // namespace ket

#pragma once

namespace impl_mqis
{

constexpr auto COMPLEX_ALMOST_EQ_TOLERANCE_EQ = double {1.0e-8};

}  // namespace impl_mqis

namespace mqis
{

/*
    We use a struct of two doubles to represent the complex number rather than `std::complex`; at the
    time of implementing this I am not 100% sure if this is the best idea, but it looks like doing
    this makes it easier to translate their functions to my own.
*/
struct Complex
{
    double real;
    double imag;
};

constexpr auto almost_eq(
    const Complex& left,
    const Complex& right,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_EQ
) noexcept -> bool
{
    const auto diff_real = left.real - right.real;
    const auto diff_imag = left.imag - right.imag;
    const auto diff_sq = diff_real * diff_real + diff_imag * diff_imag;

    return diff_sq < tolerance_sq;
}

}  // namespace mqis

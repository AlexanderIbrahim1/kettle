#pragma once

#include <bit>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <utility>

namespace mqis
{

constexpr static auto NORMALIZATION_TOLERANCE = double{1.0e-6};

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

constexpr auto norm_squared(const Complex& number) -> double
{
    return number.real * number.real + number.imag * number.imag;
}

constexpr auto pow_2_int(std::size_t exponent) -> std::size_t
{
    // there are slightly faster ways to do this, but w/e
    // - if the exponent is too large, the simulation won't run anyways
    auto output = std::size_t {1};
    for (std::size_t i {0}; i < exponent; ++i) {
        output *= 2;
    }

    return output;
}

constexpr auto is_power_of_2(std::size_t value) -> bool
{
    const auto is_positive = value > 0;
    const auto has_one_bit_set = (value & (value - 1)) == 0;

    return is_positive && has_one_bit_set;
}

class QuantumState
{
public:
    /*
        The default constructor sets the initial state to the |0000...0> state; this means the entire
        weight is on the 0th element.
    */
    explicit QuantumState(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
        , n_states_ {pow_2_int(n_qubits)}
        , coefficients_(n_states_, {0, 0})
    {
        // we can ignore the global phase factor, so the entire weight is on the real component
        coefficients_[0] = {1, 0};
    }

    explicit QuantumState(std::vector<Complex> coefficients)
        : n_qubits_ {0}
        , n_states_ {coefficients.size()}
        , coefficients_ {std::move(coefficients)}
    {
        check_power_of_2_();
        check_normalization_of_coefficients_();

        // the size of the coefficients vector is 2^{number of 0's in bit rep of the size}
        n_qubits_ = std::countr_zero(coefficients_.size());
    }

private:
    std::size_t n_qubits_;
    std::size_t n_states_;
    std::vector<Complex> coefficients_;

    void check_power_of_2_() const
    {
        if (!is_power_of_2(coefficients_.size())) {
            auto err_msg = std::stringstream {};
            err_msg << "The provided coefficients must have a size equal to a power of 2.\n";
            err_msg << "Found size = " << coefficients_.size();
            throw std::runtime_error {err_msg.str()};
        }
    }

    void check_normalization_of_coefficients_() const
    {
        const auto sum_of_squared_norms = std::accumulate(
            coefficients_.begin(),
            coefficients_.end(),
            0.0,
            [](const Complex& elem) { return norm_squared(elem); }
        );

        const auto expected = 1.0;
        const auto is_normalized = std::fabs(sum_of_squared_norms - expected) < NORMALIZATION_TOLERANCE;

        if (!is_normalized) {
            auto err_msg = std::stringstream {};
            err_msg << "The provided coefficients are not properly normalized.\n";
            err_msg << "Found sum of squared norms : ";
            err_msg << std::fixed << std::setprecision(14) << sum_of_squared_norms;
            throw std::runtime_error {err_msg.str()};
        }
    }
};

}  // namespace mqis

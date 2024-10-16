#pragma once

#include <bit>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "mini-qiskit/common/mathtools.hpp"

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

class QuantumState
{
public:
    /*
        The default constructor sets the initial state to the |0000...0> state; this means the entire
        weight is on the 0th element.
    */
    explicit QuantumState(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
        , n_states_ {impl_mqis::pow_2_int(n_qubits)}
        , coefficients_(n_states_, {0, 0})
    {
        // we can ignore the global phase factor, so the entire weight is on the real component
        check_at_least_one_qubit_();
        coefficients_[0] = {1, 0};
    }

    explicit QuantumState(std::vector<Complex> coefficients)
        : n_qubits_ {0}
        , n_states_ {coefficients.size()}
        , coefficients_ {std::move(coefficients)}
    {
        check_power_of_2_with_at_least_one_qubit_();
        check_normalization_of_coefficients_();

        // the size of the coefficients vector is 2^{number of 0's in bit rep of the size}
        n_qubits_ = static_cast<std::size_t>(std::countr_zero(coefficients_.size()));
    }

    constexpr auto operator[](std::size_t index) const noexcept -> const Complex&
    {
        return coefficients_[index];
    }

    constexpr auto operator[](std::size_t index) noexcept -> Complex&
    {
        return coefficients_[index];
    }

    constexpr auto at(std::size_t index) const -> const Complex&
    {
        check_index_(index);
        return coefficients_[index];
    }

    constexpr auto at(std::size_t index) -> Complex&
    {
        check_index_(index);
        return coefficients_[index];
    }

    constexpr auto n_states() const noexcept -> std::size_t
    {
        return n_states_;
    }

    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

private:
    std::size_t n_qubits_;
    std::size_t n_states_;
    std::vector<Complex> coefficients_;

    void check_power_of_2_with_at_least_one_qubit_() const
    {
        if (coefficients_.size() < 2) {
            throw std::runtime_error {
                "There must be at least 2 coefficients, representing the states for one qubit.\n"};
        }

        if (!impl_mqis::is_power_of_2(coefficients_.size())) {
            auto err_msg = std::stringstream {};
            err_msg << "The provided coefficients must have a size equal to a power of 2.\n";
            err_msg << "Found size = " << coefficients_.size();
            throw std::runtime_error {err_msg.str()};
        }
    }

    void check_normalization_of_coefficients_() const
    {
        auto sum_of_squared_norms = double {0.0};
        for (const auto& elem : coefficients_) {
            sum_of_squared_norms += impl_mqis::norm_squared(elem.real, elem.imag);
        }

        const auto expected = 1.0;
        const auto is_normalized = std::fabs(sum_of_squared_norms - expected) < impl_mqis::NORMALIZATION_TOLERANCE;

        if (!is_normalized) {
            auto err_msg = std::stringstream {};
            err_msg << "The provided coefficients are not properly normalized.\n";
            err_msg << "Found sum of squared norms : ";
            err_msg << std::fixed << std::setprecision(14) << sum_of_squared_norms;
            throw std::runtime_error {err_msg.str()};
        }
    }

    constexpr void check_index_(std::size_t index) const
    {
        if (index >= n_states_) {
            throw std::runtime_error {"Out-of-bounds access for the quantum state.\n"};
        }
    }

    constexpr void check_at_least_one_qubit_() const
    {
        if (n_qubits_ == 0) {
            throw std::runtime_error {"There must be at least 1 qubit in the QuantumState.\n"};
        }
    }
};

auto state_as_dynamic_bitset(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>
{
    const auto n_states = impl_mqis::pow_2_int(n_qubits);
    if (i_state >= n_states) {
        throw std::runtime_error {"The index for the requested state is greater than the number of possible states."};
    }

    auto dyn_bitset = std::vector<std::uint8_t> (n_qubits, 0);
    for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
        const auto bit_position = impl_mqis::pow_2_int(i_qubit);

        // the computational states are ordered in a little-endian manner
        dyn_bitset[n_qubits - i_qubit - 1] = bit_position & i_state;
    }

    return dyn_bitset;
}

}  // namespace mqis

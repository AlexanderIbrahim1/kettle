#pragma once

#include <algorithm>
#include <bit>
#include <bitset>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "mini-qiskit/common/complex.hpp"
#include "mini-qiskit/common/mathtools.hpp"

namespace impl_mqis
{

template <bool LittleEndian>
constexpr auto state_as_dynamic_bitset_helper_(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>
{
    const auto n_states = impl_mqis::pow_2_int(n_qubits);
    if (i_state >= n_states) {
        throw std::runtime_error {"The index for the requested state is greater than the number of possible states."};
    }

    auto dyn_bitset = std::vector<std::uint8_t>(n_qubits, 0);
    for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
        const auto bit_position = impl_mqis::pow_2_int(i_qubit);

        // internally, the computational states are ordered in a little-endian manner
        // - most people expect a big-endian representation
        if constexpr (LittleEndian) {
            dyn_bitset[i_qubit] = static_cast<std::uint8_t>((bit_position & i_state) != 0);
        }
        else {
            dyn_bitset[n_qubits - i_qubit - 1] = static_cast<std::uint8_t>((bit_position & i_state) != 0);
        }
    }

    return dyn_bitset;
}

auto dynamic_bitset_to_bitstring_(const std::vector<std::uint8_t>& bits) -> std::string
{
    auto bitstring = std::string {};
    bitstring.reserve(bits.size());
    for (auto bit : bits) {
        if (bit == 0) {
            bitstring.push_back('0');
        }
        else {
            bitstring.push_back('1');
        }
    }

    return bitstring;
}

}  // namespace impl_mqis

namespace mqis
{

enum class QuantumStateEndian
{
    LITTLE,
    BIG
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
        , coefficients_(n_states_, {0.0, 0.0})
    {
        // we can ignore the global phase factor, so the entire weight is on the real component
        check_at_least_one_qubit_();
        coefficients_[0] = {1, 0};
    }

    QuantumState(std::vector<Complex> coefficients, QuantumStateEndian input_endian = QuantumStateEndian::LITTLE)
        : n_qubits_ {0}
        , n_states_ {coefficients.size()}
        , coefficients_ {std::move(coefficients)}
    {
        check_power_of_2_with_at_least_one_qubit_();
        check_normalization_of_coefficients_();

        // the size of the coefficients vector is 2^{number of 0's in bit rep of the size}
        n_qubits_ = static_cast<std::size_t>(std::countr_zero(coefficients_.size()));

        // the user passed the vector of coefficients in big endian format, but the
        // internal mapping of the indices to the states is in little endian format,
        // so we need to perform a conversion
        if (input_endian == QuantumStateEndian::BIG) {
            perform_endian_flip_on_coefficients_();
        }
    }

    QuantumState(const std::string& computational_state, QuantumStateEndian input_endian = QuantumStateEndian::LITTLE)
        : n_qubits_ {computational_state.size()}
        , n_states_ {impl_mqis::pow_2_int(computational_state.size())}
        , coefficients_ {n_states_, {0.0, 0.0}}
    {
        const auto is_binary = [](char c) { return c == '0' || c == '1'; };
        if (!std::all_of(computational_state.begin(), computational_state.end(), is_binary)) {
            throw std::runtime_error {"Can only build QuantumState from string with all '0's and/or '1's"};
        }

        const auto index = impl_mqis::qubit_string_to_state_index(computational_state);

        if (input_endian == QuantumStateEndian::BIG) {
            coefficients_[index] = {1.0, 0.0};
        }
        else {
            const auto little_endian_index = impl_mqis::endian_flip(index, n_qubits_);
            coefficients_[little_endian_index] = {1.0, 0.0};
        }
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

    constexpr void perform_endian_flip_on_coefficients_() noexcept
    {
        for (std::size_t i {0}; i < n_states_; ++i) {
            const auto i_flip = impl_mqis::endian_flip(i, n_qubits_);
            if (i < i_flip) {
                std::swap(coefficients_[i], coefficients_[i_flip]);
            }
        }
    }
};

constexpr auto state_as_dynamic_bitset_little_endian(std::size_t i_state, std::size_t n_qubits)
    -> std::vector<std::uint8_t>
{
    return impl_mqis::state_as_dynamic_bitset_helper_<true>(i_state, n_qubits);
}

constexpr auto state_as_dynamic_bitset_big_endian(std::size_t i_state, std::size_t n_qubits)
    -> std::vector<std::uint8_t>
{
    return impl_mqis::state_as_dynamic_bitset_helper_<false>(i_state, n_qubits);
}

// the internal mapping in the quantum state is little endian, so this should be the default function
constexpr auto state_as_dynamic_bitset(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>
{
    return state_as_dynamic_bitset_little_endian(i_state, n_qubits);
}

inline auto state_as_bitstring_little_endian(std::size_t i_state, std::size_t n_qubits) -> std::string
{
    const auto bits = state_as_dynamic_bitset_little_endian(i_state, n_qubits);
    return impl_mqis::dynamic_bitset_to_bitstring_(bits);
}

inline auto state_as_bitstring_big_endian(std::size_t i_state, std::size_t n_qubits) -> std::string
{
    const auto bits = state_as_dynamic_bitset_big_endian(i_state, n_qubits);
    return impl_mqis::dynamic_bitset_to_bitstring_(bits);
}

// the internal mapping in the quantum state is little endian, so this should be the default function
inline auto state_as_bitstring(std::size_t i_state, std::size_t n_qubits) -> std::string
{
    return state_as_bitstring_little_endian(i_state, n_qubits);
}

constexpr auto almost_eq(
    const QuantumState& left,
    const QuantumState& right,
    double tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_EQ
) noexcept -> bool
{
    if (left.n_qubits() != right.n_qubits()) {
        return false;
    }

    for (std::size_t i {0}; i < left.n_states(); ++i) {
        if (!almost_eq(left[i], right[i], tolerance_sq)) {
            return false;
        }
    }

    return true;
}

}  // namespace mqis

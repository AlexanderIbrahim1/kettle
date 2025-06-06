#pragma once

#include <complex>
#include <initializer_list>
#include <vector>

#include "kettle/common/tolerance.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/state/state.hpp"

/*
    This file contains the `PauliOperator` class for 
*/


namespace ket
{

struct WeightedPauliString
{
    std::complex<double> coefficient;
    SparsePauliString pauli_string;
};

class PauliOperator
{
public:
    explicit PauliOperator(std::size_t n_qubits);

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    PauliOperator(std::vector<WeightedPauliString> weighted_pauli_strings);

    PauliOperator(const std::initializer_list<WeightedPauliString>& weighted_pauli_strings);

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return weighted_pauli_strings_.size();
    }

    [[nodiscard]]
    constexpr auto weighted_pauli_strings() const noexcept -> const std::vector<WeightedPauliString>&
    {
        return weighted_pauli_strings_;
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const WeightedPauliString&
    {
        return weighted_pauli_strings_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> WeightedPauliString&
    {
        return weighted_pauli_strings_[index];
    }

    void add(WeightedPauliString value);

    void add(std::complex<double> coefficient, SparsePauliString pauli_string);

    void remove(std::size_t index);

private:
    std::size_t n_qubits_;
    std::vector<WeightedPauliString> weighted_pauli_strings_;
};

auto almost_eq(
    const PauliOperator& left_op,
    const PauliOperator& right_op,
    double coeff_tolerance = COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) -> bool;

auto expectation_value(const PauliOperator& pauli_op, const QuantumState& state) -> std::complex<double>;

auto expectation_value(const SparsePauliString& sparse_pauli_string, const QuantumState& state) -> std::complex<double>;

}  // namespace ket

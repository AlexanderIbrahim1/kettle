#pragma once

#include <complex>
#include <vector>

#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/simulation/simulate_pauli.hpp"
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
    explicit PauliOperator(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
    {}

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

    void add(WeightedPauliString value)
    {
        weighted_pauli_strings_.emplace_back(std::move(value));
    }

    void add(std::complex<double> coefficient, SparsePauliString pauli_string)
    {
        weighted_pauli_strings_.emplace_back(coefficient, std::move(pauli_string));
    }

    void remove(std::size_t index)
    {
        if (index >= weighted_pauli_strings_.size()) {
            throw std::runtime_error {"ERROR: attempted to remove out of bounds element from PauliOperator.\n"};
        }

        const auto position = static_cast<std::ptrdiff_t>(index);
        weighted_pauli_strings_.erase(std::next(weighted_pauli_strings_.begin(), position));
    }

private:
    std::size_t n_qubits_;
    std::vector<WeightedPauliString> weighted_pauli_strings_;
};


inline auto expectation_value(const PauliOperator& pauli_op, const QuantumState& state) -> std::complex<double>
{
    auto expval = std::complex<double> {};

    for (const auto& [coeff, sparse_pauli_string] : pauli_op.weighted_pauli_strings()) {
        auto ket = state;
        simulate(sparse_pauli_string, ket);

        const auto inner_prod = inner_product(state, ket);
        const auto phase = PAULI_PHASE_MAP.at(sparse_pauli_string.phase());

        expval += (coeff * phase * inner_prod);
    }

    return expval;
}


}  // namespace ket

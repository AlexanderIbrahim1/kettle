#include <algorithm>
#include <complex>
#include <initializer_list>
#include <vector>

#include "kettle/operator/pauli/pauli_operator.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/simulation/simulate_pauli.hpp"
#include "kettle/state/statevector.hpp"

#include "kettle_internal/operator/pauli/pauli_common.hpp"

/*
    This file contains the `PauliOperator` class for 
*/

namespace ket
{

PauliOperator::PauliOperator(std::size_t n_qubits)
    : n_qubits_ {n_qubits}
{}

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
PauliOperator::PauliOperator(std::vector<WeightedPauliString> weighted_pauli_strings)
    : n_qubits_ {0}
    , weighted_pauli_strings_ {std::move(weighted_pauli_strings)}
{
    const auto& strings_ = weighted_pauli_strings_;

    if (strings_.size() == 0) {
        throw std::runtime_error {
            "ERROR: construction of `PauliOperator` with only WeightedPauliString instances requires\n"
            "a non-empty vector.\n"
        };
    }

    const auto has_nonequal_n_qubits = [](const auto& left, const auto& right) -> bool {
        return left.pauli_string.n_qubits() != right.pauli_string.n_qubits();
    };

    if (std::ranges::adjacent_find(strings_, has_nonequal_n_qubits) != strings_.end()) {
        throw std::runtime_error {
            "ERROR: construction of `PauliOperator` with WeightedPauliString instances requires\n"
            "all pauli strings to have the same number of qubits.\n"
        };
    }

    n_qubits_ = strings_[0].pauli_string.n_qubits();
}

PauliOperator::PauliOperator(const std::initializer_list<WeightedPauliString>& weighted_pauli_strings)
    : PauliOperator {std::vector<WeightedPauliString> {weighted_pauli_strings}}
{}

void PauliOperator::add(WeightedPauliString value)
{
    weighted_pauli_strings_.emplace_back(std::move(value));
}

void PauliOperator::add(std::complex<double> coefficient, SparsePauliString pauli_string)
{
    weighted_pauli_strings_.emplace_back(coefficient, std::move(pauli_string));
}

void PauliOperator::remove(std::size_t index)
{
    if (index >= weighted_pauli_strings_.size()) {
        throw std::runtime_error {"ERROR: attempted to remove out of bounds element from PauliOperator.\n"};
    }

    const auto position = static_cast<std::ptrdiff_t>(index);
    weighted_pauli_strings_.erase(std::next(weighted_pauli_strings_.begin(), position));
}


auto expectation_value(const PauliOperator& pauli_op, const Statevector& state) -> std::complex<double>
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


auto expectation_value(const SparsePauliString& sparse_pauli_string, const Statevector& state) -> std::complex<double>
{
    auto ket = state;
    simulate(sparse_pauli_string, ket);

    const auto inner_prod = inner_product(state, ket);
    const auto phase = PAULI_PHASE_MAP.at(sparse_pauli_string.phase());

    const auto expval = phase * inner_prod;

    return expval;
}

auto almost_eq(
    const PauliOperator& left_op,
    const PauliOperator& right_op,
    double coeff_tolerance
) -> bool
{
    return ket::internal::almost_eq_pauli_helper_(left_op, right_op, coeff_tolerance);
}

}  // namespace ket

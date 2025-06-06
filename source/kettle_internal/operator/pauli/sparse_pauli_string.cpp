#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <optional>
#include <vector>

#include "kettle/operator/pauli/sparse_pauli_string.hpp"

namespace
{

using PauliIndexedTerms = std::vector<std::pair<std::size_t, ket::PauliTerm>>;

auto sorted_non_identity_paulis_(const PauliIndexedTerms& pauli_indexed_terms) -> PauliIndexedTerms
{
    auto output = PauliIndexedTerms {};
    for (const auto& elem : pauli_indexed_terms) {
        if (elem.second != ket::PauliTerm::I) {
            output.emplace_back(elem);
        }
    }

    std::ranges::sort(output, [](const auto& left, const auto& right) { return left.first < right.first; });

    return output;
}

}  // namespace

namespace ket
{

SparsePauliString::SparsePauliString(std::size_t n_qubits, PauliPhase phase)
    : n_qubits_ {n_qubits}
    , phase_ {phase}
{
    check_n_qubits_not_zero_();
}

SparsePauliString::SparsePauliString(
    std::vector<std::pair<std::size_t, PauliTerm>> pauli_indexed_terms,
    std::size_t n_qubits,
    PauliPhase phase
)
    : n_qubits_ {n_qubits}
    , phase_ {phase}
    , pauli_indexed_terms_ {std::move(pauli_indexed_terms)}
{
    check_n_qubits_not_zero_();
}

SparsePauliString::SparsePauliString(
    const std::vector<PauliTerm>& pauli_terms,
    PauliPhase phase
)
    : n_qubits_ {pauli_terms.size()}
    , phase_ {phase}
{
    check_n_qubits_not_zero_();

    for (std::size_t i {0}; i < pauli_terms.size(); ++i) {
        pauli_indexed_terms_.emplace_back(i, pauli_terms[i]);
    }
}

SparsePauliString::SparsePauliString(
    const std::initializer_list<PauliTerm>& pauli_terms,
    PauliPhase phase
)
    : SparsePauliString {std::vector<PauliTerm> {pauli_terms}, phase}
{}

void SparsePauliString::set_phase(PauliPhase phase) noexcept
{
    phase_ = phase;
}

[[nodiscard]]
auto SparsePauliString::at(std::size_t qubit_index) const -> PauliTerm
{
    const auto vector_index = vector_index_(qubit_index);

    if (vector_index) {
        return pauli_indexed_terms_[vector_index.value()].second;
    }

    throw std::runtime_error {"ERROR: no Pauli term found for provided qubit index.\n"};
}

void SparsePauliString::add(std::size_t qubit_index, PauliTerm term)
{
    check_index_in_qubit_range_(qubit_index);

    if (vector_index_(qubit_index)) {
        throw std::runtime_error {"ERROR: Pauli term is already present in the string\n"};
    }

    pauli_indexed_terms_.emplace_back(qubit_index, term);
}

void SparsePauliString::overwrite(std::size_t qubit_index, PauliTerm term)
{
    check_index_in_qubit_range_(qubit_index);

    const auto existing_index = vector_index_(qubit_index);

    if (existing_index) {
        pauli_indexed_terms_[existing_index.value()].second = term;
    } else {
        pauli_indexed_terms_.emplace_back(qubit_index, term);
    }
}

void SparsePauliString::remove(std::size_t qubit_index)
{
    const auto vector_index = vector_index_(qubit_index);

    if (vector_index) {
        const auto position = static_cast<std::ptrdiff_t>(vector_index.value());
        pauli_indexed_terms_.erase(std::next(pauli_indexed_terms_.begin(), position));
    }
}

[[nodiscard]]
auto SparsePauliString::contains_index(std::size_t qubit_index) const noexcept -> bool
{
    const auto contains = [qubit_index](const auto& pair) { return pair.first == qubit_index; };
    return std::ranges::any_of(pauli_indexed_terms_, contains);
}

[[nodiscard]]
auto SparsePauliString::vector_index_(std::size_t qubit_index) const -> std::optional<std::size_t>
{
    if (qubit_index >= n_qubits_) {
        return std::nullopt;
    }

    for (std::size_t i {0}; i < pauli_indexed_terms_.size(); ++i) {
        const auto& [existing_index, term] = pauli_indexed_terms_[i];

        if (existing_index == qubit_index) {
            return i;
        }
    }

    return std::nullopt;
}

void SparsePauliString::check_index_in_qubit_range_(std::size_t index) const
{
    if (index >= n_qubits_) {
        throw std::runtime_error {
            "ERROR: cannot perform operation on SparsePauliString with index beyond qubit range.\n"
        };
    }
}

void SparsePauliString::check_n_qubits_not_zero_() const
{
    if (n_qubits_ == 0) {
        throw std::runtime_error {"ERROR: SparsePauliString cannot be constructed with 0 qubits.\n"};
    }
}

auto SparsePauliString::equal_up_to_phase(const SparsePauliString& other) const -> bool
{
    if (n_qubits_ != other.n_qubits_) {
        return false;
    }

    // NOTE: this object can store identity operators, which don't do anything and do not affect
    // whether one SparsePauliString is equal to another, but it does change the size; thus we
    // cannot use the size as a comparison tool

    const auto left = sorted_non_identity_paulis_(pauli_indexed_terms_);
    const auto right = sorted_non_identity_paulis_(other.pauli_indexed_terms_);

    return left == right;
}

auto operator==(const SparsePauliString& left, const SparsePauliString& right) -> bool
{
    if (left.phase_ != right.phase_) {
        return false;
    }

    return left.equal_up_to_phase(right);
}

}  // namespace ket

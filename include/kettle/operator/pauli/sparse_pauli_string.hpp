#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <optional>
#include <vector>


namespace ket
{

enum class PauliTerm : std::uint8_t
{
    I,
    X,
    Y,
    Z,
};

enum class PauliPhase : std::uint8_t
{
    PLUS_ONE,
    PLUS_EYE,
    MINUS_ONE,
    MINUS_EYE,
};

/*
    The `SparsePauliString` class holds a container of qubit indices and non-identity Pauli terms.
*/
class SparsePauliString
{
public:
    explicit SparsePauliString(std::size_t n_qubits)
        : phase_ {PauliPhase::PLUS_ONE}
        , n_qubits_ {n_qubits}
    {
        if (n_qubits_ == 0) {
            throw std::runtime_error {"ERROR: SparsePauliString cannot be constructed with 0 qubits.\n"};
        }
    }

    void set_phase(PauliPhase phase) noexcept
    {
        phase_ = phase;
    }

    [[nodiscard]]
    constexpr auto phase() const noexcept -> PauliPhase
    {
        return phase_;
    }

    [[nodiscard]]
    constexpr auto terms() const noexcept -> const std::vector<std::pair<std::size_t, PauliTerm>>&
    {
        return pauli_terms_;
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return pauli_terms_.size();
    }

    [[nodiscard]]
    auto at(std::size_t qubit_index) const -> PauliTerm
    {
        const auto vector_index = contains_index(qubit_index);

        if (vector_index) {
            return pauli_terms_[vector_index.value()].second;
        }

        throw std::runtime_error {"ERROR: no Pauli term found for provided qubit index.\n"};
    }

    void add(std::size_t qubit_index, PauliTerm term)
    {
        check_index_in_qubit_range_(qubit_index);

        if (contains_index(qubit_index)) {
            throw std::runtime_error {"ERROR: Pauli term is already present in the string\n"};
        }

        pauli_terms_.emplace_back(qubit_index, term);
    }

    void overwrite(std::size_t qubit_index, PauliTerm term)
    {
        check_index_in_qubit_range_(qubit_index);

        const auto existing_index = contains_index(qubit_index);

        if (existing_index) {
            pauli_terms_[existing_index.value()].second = term;
        } else {
            pauli_terms_.emplace_back(qubit_index, term);
        }
    }

    void remove(std::size_t qubit_index)
    {
        const auto vector_index = contains_index(qubit_index);

        if (vector_index) {
            const auto position = static_cast<std::ptrdiff_t>(vector_index.value());
            pauli_terms_.erase(std::next(pauli_terms_.begin(), position));
        }
    }

    [[nodiscard]]
    auto contains_index(std::size_t qubit_index) const -> std::optional<std::size_t>
    {
        if (qubit_index >= n_qubits_) {
            return std::nullopt;
        }

        for (std::size_t i {0}; i < pauli_terms_.size(); ++i) {
            const auto& [existing_index, term] = pauli_terms_[i];

            if (existing_index == qubit_index) {
                return i;
            }
        }

        return std::nullopt;
    }

private:
    PauliPhase phase_ {};
    std::size_t n_qubits_;
    std::vector<std::pair<std::size_t, PauliTerm>> pauli_terms_;

    void check_index_in_qubit_range_(std::size_t index) const
    {
        if (index >= n_qubits_) {
            throw std::runtime_error {
                "ERROR: cannot perform operation on SparsePauliString with index beyond qubit range.\n"
            };
        }
    }

    // NOTE: why do we use a `std::vector` of pairs instead of a map?
    //   - first, because the Pauli string is sparse, we expect the container to hold very
    //     few of them; so the time complexity of a search is dominated by the prefactor,
    //     which makes a linear search through a contiguous vector much faster
    //   - during a simulation, we need to loop over all the pairs anyways, and the order
    //     doesn't matter; so a `std::vector` is faster for this anyways
};

}  // namespace ket

#pragma once

#include <complex>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
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

// NOLINTNEXTLINE(cert-err58-cpp)
const auto PAULI_PHASE_MAP = std::unordered_map<PauliPhase, std::complex<double>> {
    {PauliPhase::PLUS_ONE, {1.0, 0.0}},
    {PauliPhase::PLUS_EYE, {0.0, 1.0}},
    {PauliPhase::MINUS_ONE, {-1.0, 0.0}},
    {PauliPhase::MINUS_EYE, {0.0, -1.0}},
};

/*
    The `SparsePauliString` class holds a container of qubit indices and non-identity Pauli terms.
*/
class SparsePauliString
{
public:
    explicit SparsePauliString(std::size_t n_qubits);

    /*
        Set the phase of the `SparsePauliString`.
    */
    void set_phase(PauliPhase phase) noexcept;

    [[nodiscard]]
    constexpr auto phase() const noexcept -> PauliPhase
    {
        return phase_;
    }

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
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

    /*
        Returns the `PauliTerm` instance applied to the qubit at `qubit_index`;
        throws a `std::runtime_error` if no `PauliTerm` is applied at the qubit.
    */
    [[nodiscard]]
    auto at(std::size_t qubit_index) const -> PauliTerm;

    /*
        Add a `PauliTerm` operator `term` to be applied to the qubit at `qubit_index`;
        throws a `std::runtime_error` if an existing `PauliTerm` is already applied to the qubit.
    */
    void add(std::size_t qubit_index, PauliTerm term);

    /*
        Add a `PauliTerm` operator `term` to be applied to the qubit at `qubit_index`;
        if an existing `PauliTerm` is already applied to the qubit, it will be overwritten
        with the provided `term`.
    */
    void overwrite(std::size_t qubit_index, PauliTerm term);

    /*
        Removes the `PauliTerm` operator being applied to the qubit at `qubit_index`;
        does nothing if there is no `PauliTerm` operator at the qubit.
    */
    void remove(std::size_t qubit_index);

    /*
        Checks if a `PauliTerm` operator is being applied to the qubit at `qubit_index`.
    */
    [[nodiscard]]
    auto contains_index(std::size_t qubit_index) const noexcept -> bool;

private:
    PauliPhase phase_ {};
    std::size_t n_qubits_;
    std::vector<std::pair<std::size_t, PauliTerm>> pauli_terms_;

    void check_index_in_qubit_range_(std::size_t index) const;

    /*
        Checks if a `PauliTerm` operator is being applied to the qubit at `qubit_index`;
        if yes, the index the `PauliTerm` instance in the internal container is returned,
        and if no, then `std::nullopt` is returned.
    */
    [[nodiscard]]
    auto vector_index_(std::size_t qubit_index) const -> std::optional<std::size_t>;

    // NOTE: why do we use a `std::vector` of pairs instead of a map?
    //   - first, because the Pauli string is sparse, we expect the container to hold very
    //     few of them; so the time complexity of a search is dominated by the prefactor,
    //     which makes a linear search through a contiguous vector much faster
    //   - during a simulation, we need to loop over all the pairs anyways, and the order
    //     doesn't matter; so a `std::vector` is faster for this anyways
};

}  // namespace ket

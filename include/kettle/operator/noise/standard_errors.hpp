#pragma once

#include <ranges>
#include <vector>

#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"

/*
    This header contains the functions for implementing the following error types:
      - symmetric depolarizing error (as a Pauli Channel)
      - amplitude error (as a Kraus channel)
      - phase damping error (as a Kraus channel)
      - phase-amplitude damping error (as a Kraus channel)
      - thermal relaxation error (as a Kraus channel)
      - reset error (as a Mixed Circuit channel)
      - coherent unitary error (as a Mixed Circuit channel)
    
    Some error types aren't implemented as functions; you should call the constructors directly:
      - general pauli errors (as a PauliChannel)
      - mixed unitary error (as a MixedCircuitChannel)
*/

namespace ket::internal
{

/*
    Custom type that loops over all possible Cartesian product combinations; this is because the
    current codebase is sticking with C++20, and `std::ranges::views::cartesian_product()` is not
    available until C++23 :(
*/
class CartesianTicker
{
public:
    CartesianTicker(std::size_t n_elements, std::size_t period)
        : period_ {period}
        , ticker_(n_elements, 0)
    {}

    [[nodiscard]]
    constexpr auto size() const -> std::size_t
    {
        // TODO: replace with internal power function when we split the header and source
        return static_cast<std::size_t>(std::pow(period_, ticker_.size()));
    }

    [[nodiscard]]
    constexpr auto ticker() const -> const std::vector<std::size_t>&
    {
        return ticker_;
    }

    constexpr void increment()
    {
        namespace sv = std::views;

        for (std::size_t i : sv::iota(0UL, ticker_.size()) | sv::reverse) {

            ticker_[i] += 1;

            if (ticker_[i] == period_) {
                ticker_[i] = 0;
                continue;
            }
        
            break;
        }
    }

private:
    std::size_t period_;
    std::vector<std::size_t> ticker_;
};

}  // namespace ket::internal

namespace ket
{

/*
    The symmetric depolarizing error channel applied to a single qubit.

    For this definition:
      - p = 0 gives a noiseless channel
      - p = 3/4 gives a full depolarized channel, and the output will be proportional to the identity matrix
      - p = 1 gives the uniform Pauli error channel, where X, Y, and Z are applied equally to the 1-qubit density matrix
*/
template <QubitIndices Container = QubitIndicesIList>
auto symmetric_depolarizing_error_channel(
    double parameter,
    std::size_t n_qubits,
    const Container& indices
) -> ket::PauliChannel
{
    using PT = ket::PauliTerm;

    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    // TODO: replace this with the internal function
    const auto size = static_cast<std::size_t>(std::distance(indices.begin(), indices.end()));

    if (size == 0) {
        throw std::runtime_error {"ERROR: cannot create depolarizing noise channel acting on 0 qubits.\n"};
    }

    // TODO: check if any indices go past the number of qubits

    const auto n_total_pauli_terms = (1UL << (2UL * size));
    const auto n_noisy_pauli_terms = n_total_pauli_terms - 1UL;
    const auto noiseless_coeff = 1.0 - parameter;
    const auto noisy_coeff = parameter / static_cast<double>(n_noisy_pauli_terms);

    auto sparse_pauli_strings = std::vector<ket::ProbabilisticPauliString> {};
    sparse_pauli_strings.reserve(n_total_pauli_terms);

    const auto n_pauli_kinds = std::size_t {4};
    const auto n_indices = indices.size();
    auto ticker = ket::internal::CartesianTicker {n_indices, n_pauli_kinds};
    const auto all_pauli_kinds = std::vector<PT> {PT::I, PT::X, PT::Y, PT::Z};

    // the first term must always be the noiseless term
    sparse_pauli_strings.emplace_back(noiseless_coeff, ket::SparsePauliString {n_qubits});

    for (std::size_t j {0}; j < n_noisy_pauli_terms; ++j) {
        auto string = ket::SparsePauliString {n_qubits};

        ticker.increment();
        const auto& pauli_kind_indices = ticker.ticker();
        for (std::size_t i {0}; i < n_indices; ++i) {
            const auto pauli_kind = all_pauli_kinds[pauli_kind_indices[i]];

            // TODO: replace this operator[] access with the internal function, because it might be
            // an initializer list, and that *doesn't* implement operator[]
            const auto qubit_index = *(indices.begin() + i);

            if (pauli_kind != PT::I) {
                string.add(qubit_index, pauli_kind);
            }
        }
        sparse_pauli_strings.emplace_back(noisy_coeff, std::move(string));
    }

    return ket::PauliChannel {std::move(sparse_pauli_strings)};
}

}  // namespace ket
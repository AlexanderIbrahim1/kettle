#include <vector>

#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/operator/noise/standard_errors.hpp"

#include "kettle_internal/common/utils_internal.hpp"
#include "kettle_internal/operator/channels/channel_helper.hpp"

namespace ki = ket::internal;

namespace ket
{

template <QubitIndices Container>
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

    const auto size = ki::get_container_size(indices);

    if (size == 0) {
        throw std::runtime_error {"ERROR: cannot create depolarizing noise channel acting on 0 qubits.\n"};
    }

    if (std::ranges::any_of(indices, [&](auto index) { return index >= n_qubits; })) {
        throw std::runtime_error {"ERROR: cannot apply depolarizing error to qubit with index outside of `n_qubits`\n"};
    }

    const auto n_total_pauli_terms = (1UL << (2UL * size));
    const auto n_noisy_pauli_terms = n_total_pauli_terms - 1UL;
    const auto noiseless_coeff = 1.0 - parameter;
    const auto noisy_coeff = parameter / static_cast<double>(n_noisy_pauli_terms);

    auto sparse_pauli_strings = std::vector<ket::ProbabilisticPauliString> {};
    sparse_pauli_strings.reserve(n_total_pauli_terms);

    const auto n_pauli_kinds = std::size_t {4};
    const auto n_indices = indices.size();
    auto ticker = ki::CartesianTicker {n_indices, n_pauli_kinds};
    const auto all_pauli_kinds = std::vector<PT> {PT::I, PT::X, PT::Y, PT::Z};

    // the first term must always be the noiseless term
    sparse_pauli_strings.emplace_back(noiseless_coeff, ket::SparsePauliString {n_qubits});

    for (std::size_t j {0}; j < n_noisy_pauli_terms; ++j) {
        auto string = ket::SparsePauliString {n_qubits};

        ticker.increment();
        const auto& pauli_kind_indices = ticker.ticker();
        for (std::size_t i {0}; i < n_indices; ++i) {
            const auto pauli_kind = all_pauli_kinds[pauli_kind_indices[i]];

            if (pauli_kind != PT::I) {
                const auto qubit_index = ki::get_container_index(indices, i);
                string.add(qubit_index, pauli_kind);
            }
        }
        sparse_pauli_strings.emplace_back(noisy_coeff, std::move(string));
    }

    return ket::PauliChannel {std::move(sparse_pauli_strings)};
}
template auto symmetric_depolarizing_error_channel(
    double parameter,
    std::size_t n_qubits,
    const QubitIndicesIList& indices
) -> ket::PauliChannel;
template auto symmetric_depolarizing_error_channel(
    double parameter,
    std::size_t n_qubits,
    const QubitIndicesVector& indices
) -> ket::PauliChannel;

}  // namespace ket
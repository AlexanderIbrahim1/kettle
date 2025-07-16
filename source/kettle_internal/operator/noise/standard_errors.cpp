#include <format>
#include <string_view>
#include <vector>

#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/operator/noise/standard_errors.hpp"

#include "kettle_internal/common/utils_internal.hpp"
#include "kettle_internal/operator/channels/channel_helper.hpp"

namespace ki = ket::internal;

namespace
{

auto check_in_0_1_(double value, std::string_view parameter, std::string_view channel)
{
    if (value < 0.0 || value > 1.0) {
        const auto err_msg = std::format(
            "ERROR: the '{}' parameter for the '{}' channel must be in [0.0, 1.0]\n", parameter, channel
        );

        throw std::runtime_error {err_msg};
    }
}

/*
    A thermal relaxation process requires that `T2 <= 2 * T1`.
*/
void check_relaxation_times_valid_(
    const ket::RelaxationTime t1,
    const ket::RelaxationTime t2
)
{
    // NOTE: T1 and T2 are very common variables in QC literature, so it's probably better to leave these
    // as is instead of giving them full names; also to prevent confusion with the "gate_time" variable,
    // which is also a time but not a constant.

    // if T1 is infinite, it doesn't matter if T2 is finite or infinite;
    // - if T2 is also infinite, then there is no error at all (and the channel does nothing)
    // - if T2 is finite, the constraint is satisfied
    if (t1.is_infinite()) {
        return;
    }

    // NOTE: must be in this order, because calling `time()` throws if first statement is false
    if ((t2.is_infinite() && !t1.is_infinite()) || (t2.time() > 2.0 * t1.time())) {
        throw std::runtime_error {"ERROR: a thermal relaxation process requires that `T2 <= 2 * T1`.\n"};
    }
}

}  // namespace

namespace ket
{

template <QubitIndices Container>
auto symmetric_depolarizing_error_channel(
    double depolarizing_parameter,
    std::size_t n_qubits,
    const Container& indices
) -> ket::PauliChannel
{
    using PT = ket::PauliTerm;

    check_in_0_1_(depolarizing_parameter, "depolarizing_parameter", "symmetric_depolarizing_error_channel");

    const auto size = ki::get_container_size(indices);

    if (size == 0) {
        throw std::runtime_error {"ERROR: cannot create depolarizing noise channel acting on 0 qubits.\n"};
    }

    if (std::ranges::any_of(indices, [&](auto index) { return index >= n_qubits; })) {
        throw std::runtime_error {"ERROR: cannot apply depolarizing error to qubit with index outside of `n_qubits`\n"};
    }

    const auto n_total_pauli_terms = (1UL << (2UL * size));
    const auto n_noisy_pauli_terms = n_total_pauli_terms - 1UL;
    const auto noiseless_coeff = 1.0 - depolarizing_parameter;
    const auto noisy_coeff = depolarizing_parameter / static_cast<double>(n_noisy_pauli_terms);

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


auto one_qubit_phase_amplitude_damping_error_channel(
    const PhaseAmplitudeDampingParameters& parameters,
    std::size_t target_index,
    double tolerance
) -> ket::OneQubitKrausChannel
{
    const auto* func_name = "one_qubit_phase_amplitude_damping_error_channel";

    check_in_0_1_(parameters.amplitude, "amplitude", func_name);
    check_in_0_1_(parameters.phase, "phase", func_name);
    check_in_0_1_(parameters.excited_population, "excited_population", func_name);
    check_in_0_1_(parameters.amplitude + parameters.phase, "amplitude + phase", func_name);

    const auto pop_damp0 = std::sqrt(1.0 - parameters.excited_population);
    const auto pop_damp1 = std::sqrt(parameters.excited_population);

    const auto param_both = std::sqrt(1.0 - parameters.amplitude - parameters.phase);
    const auto param_ampp = std::sqrt(parameters.amplitude);
    const auto param_phas = std::sqrt(parameters.phase);

    auto kraus_matrices = std::vector<ket::Matrix2X2> {};
    kraus_matrices.emplace_back(pop_damp0 * Matrix2X2 {1.0, 0.0, 0.0, param_both}); // NOLINT
    kraus_matrices.emplace_back(pop_damp0 * Matrix2X2 {0.0, param_ampp, 0.0, 0.0}); // NOLINT
    kraus_matrices.emplace_back(pop_damp0 * Matrix2X2 {0.0, 0.0, 0.0, param_phas}); // NOLINT
    kraus_matrices.emplace_back(pop_damp1 * Matrix2X2 {param_both, 0.0, 0.0, 1.0}); // NOLINT
    kraus_matrices.emplace_back(pop_damp1 * Matrix2X2 {0.0, 0.0, param_ampp, 0.0}); // NOLINT
    kraus_matrices.emplace_back(pop_damp1 * Matrix2X2 {param_phas, 0.0, 0.0, 0.0}); // NOLINT

    const auto norm_too_small = [&](const auto& matrix) { return ket::norm(matrix) < tolerance; };
    std::erase_if(kraus_matrices, norm_too_small);

    // TODO: use a version of this that doesn't bother to check the conditions
    return ket::OneQubitKrausChannel {std::move(kraus_matrices), target_index, tolerance};
}

/*
    The one-qubit thermal relaxation error channel, applied to a single qubit.
      - `tolerance`: omit Kraus matrices from the channel if their Frobenius norm is less than this

    TODO: remove the magic number for the tolerance
*/
auto one_qubit_thermal_relaxation_error_channel(
    const ThermalRelaxationParameters& parameters,
    std::size_t target_index,
    double tolerance
) -> ket::OneQubitKrausChannel
{
    const auto* func_name = "one_qubit_phase_amplitude_damping_error_channel";

    check_in_0_1_(parameters.excited_population, "excited_population", func_name);
    check_relaxation_times_valid_(parameters.t1, parameters.t2);

    if (parameters.gate_time < 0.0) {
        throw std::runtime_error {"ERROR: the gate time for an operation cannot be negative.\n"};
    }

    const auto& [t1, t2, gate_time, pop1] = parameters;

    const auto p_reset = [&]() {
        if (t1.is_infinite()) {
            return 0.0;
        } else {
            return 1.0 - std::exp(- gate_time / t1.time());
        }
    }();

    const auto exp_t2 = [&]() {
        if (t2.is_infinite()) {
            return 0.0;
        } else {
            return std::exp(- gate_time / t2.time() );
        }
    }();

    const auto pop0 = 1.0 - pop1;

    // manually calculate the Kraus decomposition; the parameters here don't really have meaningful names
    const auto a = 1.0 - (pop1 * p_reset);
    const auto b = 1.0 - (pop0 * p_reset);
    const auto alpha = (a + b) / 2.0;
    const auto beta = (a - b) / 2.0;
    const auto root = std::sqrt((beta * beta) + (exp_t2 * exp_t2));

    const auto evalue0 = alpha + root;
    const auto evalue1 = pop1 * p_reset;
    const auto evalue2 = pop0 * p_reset;
    const auto evalue3 = alpha - root;

    auto kraus_matrices = std::vector<ket::Matrix2X2> {};
    kraus_matrices.emplace_back(1.0, 0.0, 0.0, (evalue0 - a) / exp_t2);
    kraus_matrices.emplace_back(0.0, 0.0, 1.0, 0.0);
    kraus_matrices.emplace_back(0.0, 1.0, 0.0, 0.0);
    kraus_matrices.emplace_back(1.0, 0.0, 0.0, (evalue3 - a) / exp_t2);

    // normalize and multiply by eigenvalues at the same time
    kraus_matrices[0] *= (std::sqrt(evalue0) / ket::norm(kraus_matrices[0]));
    kraus_matrices[1] *= std::sqrt(evalue1);
    kraus_matrices[2] *= std::sqrt(evalue2);
    kraus_matrices[3] *= (std::sqrt(evalue3) / ket::norm(kraus_matrices[3]));

    const auto norm_too_small = [&](const auto& matrix) { return ket::norm(matrix) < tolerance; };
    std::erase_if(kraus_matrices, norm_too_small);

    // TODO: use a version of this that doesn't bother to check the conditions
    return ket::OneQubitKrausChannel {std::move(kraus_matrices), target_index, tolerance};
}

}  // namespace ket
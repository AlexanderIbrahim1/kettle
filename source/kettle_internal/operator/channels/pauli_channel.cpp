#include <initializer_list>
#include <vector>

#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/operator/pauli/sparse_pauli_string.hpp"

#include "kettle_internal/operator/pauli/pauli_common.hpp"
#include "kettle_internal/operator/channels/unitary_channel_helper.hpp"


namespace ket
{

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
PauliChannel::PauliChannel(
    std::vector<ProbabilisticPauliString> weighted_pauli_strings,
    double tolerance
)
    : n_qubits_ {0}
    , weighted_unitaries_ {std::move(weighted_pauli_strings)}
{
    namespace ki = ket::internal;
    const auto name = std::string {"PauliChannel"};

    const auto n_qubits_getter = [](const auto& elem) { return elem.pauli_string.n_qubits(); };
    const auto coefficient_getter = [](const auto& elem) { return elem.coefficient; };

    ki::check_nonempty_(weighted_unitaries_, name);
    ki::check_unitaries_have_same_n_qubits_(weighted_unitaries_, n_qubits_getter, name);
    ki::check_probabilities_add_up_to_1_(weighted_unitaries_, coefficient_getter, tolerance, name);

    n_qubits_ = weighted_unitaries_[0].pauli_string.n_qubits();

    ki::check_number_of_qubits_is_nonzero_(n_qubits_, name);
}


PauliChannel::PauliChannel(
    const std::initializer_list<ProbabilisticPauliString>& weighted_pauli_strings,
    double tolerance
)
    : PauliChannel {std::vector<ProbabilisticPauliString> {weighted_pauli_strings}, tolerance}
{}


auto almost_eq(
    const PauliChannel& left_op,
    const PauliChannel& right_op,
    double coeff_tolerance
) -> bool
{
    return ket::internal::almost_eq_pauli_helper_(left_op, right_op, coeff_tolerance);
}


auto depolarizing_noise_pauli_1qubit(double parameter) -> PauliChannel
{
    using PT = ket::PauliTerm;

    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = 1.0 - parameter;
    const auto coeff123 = parameter / 3.0;

    return PauliChannel {
        {.coefficient=coeff0,   .pauli_string={PT::I}},
        {.coefficient=coeff123, .pauli_string={PT::X}},
        {.coefficient=coeff123, .pauli_string={PT::Y}},
        {.coefficient=coeff123, .pauli_string={PT::Z}},
    };
}

}  // namespace ket

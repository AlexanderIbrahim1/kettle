#include <initializer_list>
#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/operator/channels/mixed_unitary_channel.hpp"

#include "kettle_internal/operator/channels/unitary_channel_helper.hpp"
#include "kettle_internal/operator/channels/almost_eq_helper.hpp"

namespace ki = ket::internal;

namespace
{

/*
    Each unitary in the MixedUnitaryChannel must:
      - have no measurement gates
      - have no classical control flow
      - no circuit loggers
*/

}  // namespace


namespace ket
{

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
MixedUnitaryChannel::MixedUnitaryChannel(
    std::vector<ProbabilisticUnitary> weighted_pauli_strings,
    double tolerance
)
    : n_qubits_ {0}
    , weighted_unitaries_ {std::move(weighted_pauli_strings)}
{
    namespace ki = ket::internal;
    const auto name = std::string {"MixedUnitaryChannel"};

    const auto n_qubits_getter = [](const auto& elem) { return elem.unitary.n_qubits(); };
    const auto coefficient_getter = [](const auto& elem) { return elem.coefficient; };

    ki::check_nonempty_(weighted_unitaries_, name);
    ki::check_unitaries_have_same_n_qubits_(weighted_unitaries_, n_qubits_getter, name);
    ki::check_probabilities_add_up_to_1_(weighted_unitaries_, coefficient_getter, tolerance, name);

    n_qubits_ = weighted_unitaries_[0].unitary.n_qubits();

    ki::check_number_of_qubits_is_nonzero_(n_qubits_, name);
}


MixedUnitaryChannel::MixedUnitaryChannel(
    const std::initializer_list<ProbabilisticUnitary>& weighted_pauli_strings,
    double tolerance
)
    : MixedUnitaryChannel {std::vector<ProbabilisticUnitary> {weighted_pauli_strings}, tolerance}
{}


auto almost_eq(
    const MixedUnitaryChannel& left_op,
    const MixedUnitaryChannel& right_op,
    double coeff_tolerance
) -> bool
{
    using PU = ProbabilisticUnitary;
    const auto almost_eq = [coeff_tolerance](const PU& left, const PU& right) {
        if (!ket::almost_eq(left.coefficient, right.coefficient, coeff_tolerance)) {
            return false;
        }

        return ket::almost_eq(left.unitary, right.unitary, coeff_tolerance);
    };

    return ki::almost_eq_helper_(left_op, right_op, almost_eq);
}

}  // namespace ket

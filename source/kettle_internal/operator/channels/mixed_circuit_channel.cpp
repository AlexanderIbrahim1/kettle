#include <initializer_list>
#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/operator/channels/mixed_circuit_channel.hpp"

#include "kettle_internal/operator/channels/unitary_channel_helper.hpp"
#include "kettle_internal/operator/channels/almost_eq_helper.hpp"

namespace ki = ket::internal;

namespace
{

/*
    Channels that are probabilistic linear combinations of quantum circuits can only have unitary gates
    and non-unitary gates (M, RESET). However, they cannot have classical control flow.
*/
auto check_only_gates_(const std::vector<ket::WeightedCircuit>& weighted_operators)
{
    for ([[maybe_unused]] const auto& [ignore, unitary] : weighted_operators) {
        for (const auto& circ_element : unitary) {
            if (!circ_element.is_gate() && !circ_element.is_circuit_logger()) {
                throw std::runtime_error{"ERROR: MixedCircuitChannel only allows gates and loggers as circuit elements.\n"};
            }
        }
    }
}

}  // namespace


namespace ket
{

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
MixedCircuitChannel::MixedCircuitChannel(
    std::vector<WeightedCircuit> weighted_operators,
    double tolerance
)
    : n_qubits_ {0}
    , weighted_operators_ {std::move(weighted_operators)}
{
    namespace ki = ket::internal;
    const auto name = std::string {"MixedCircuitChannel"};

    const auto n_qubits_getter = [](const auto& elem) { return elem.unitary.n_qubits(); };
    const auto coefficient_getter = [](const auto& elem) { return elem.coefficient; };

    ki::check_nonempty_(weighted_operators_, name);
    n_qubits_ = weighted_operators_[0].unitary.n_qubits();
    ki::check_number_of_qubits_is_nonzero_(n_qubits_, name);

    ki::check_unitaries_have_same_n_qubits_(weighted_operators_, n_qubits_getter, name);
    ki::check_probabilities_add_up_to_1_(weighted_operators_, coefficient_getter, tolerance, name);
    check_only_gates_(weighted_operators_);
}


MixedCircuitChannel::MixedCircuitChannel(
    const std::initializer_list<WeightedCircuit>& weighted_operators,
    double tolerance
)
    : MixedCircuitChannel {std::vector<WeightedCircuit> {weighted_operators}, tolerance}
{}


auto almost_eq(
    const MixedCircuitChannel& left_op,
    const MixedCircuitChannel& right_op,
    double coeff_tolerance
) -> bool
{
    using PU = WeightedCircuit;
    const auto almost_eq = [coeff_tolerance](const PU& left, const PU& right) {
        if (!ket::almost_eq(left.coefficient, right.coefficient, coeff_tolerance)) {
            return false;
        }

        return ket::almost_eq(left.unitary, right.unitary, coeff_tolerance);
    };

    return ki::almost_eq_helper_(left_op, right_op, almost_eq);
}

}  // namespace ket

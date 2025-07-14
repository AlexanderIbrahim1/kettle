#include <initializer_list>
#include <vector>

#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_operations/compare_circuits.hpp"
#include "kettle/common/mathtools.hpp"
#include "kettle/operator/channels/mixed_unitary_channel.hpp"

#include "kettle_internal/operator/channels/unitary_channel_helper.hpp"


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
    if (left_op.size() != right_op.size()) {
        return false;
    }

    const auto size = left_op.size();
    for (std::size_t i {0}; i < size; ++i) {
        const auto& left = left_op.at(i);
        const auto& right = right_op.at(i);

        if (!ket::almost_eq(left.coefficient, right.coefficient, coeff_tolerance)) {
            return false;
        }

        if (!ket::almost_eq(left.unitary, right.unitary, coeff_tolerance)) {
            return false;
        }
    }

    return true;
}

/*
    The MixedUnitaryChannel version for depolarizing noise.

    NOTE: this function will be replaced by the Kraus channel version; I'm just writing it right
    now so I have something to unit test
*/
auto depolarizing_noise_mixed_unitary_1qubit(double parameter) -> MixedUnitaryChannel
{
    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = 1.0 - parameter;
    const auto coeff123 = parameter / 3.0;

    auto circuit0 = ket::QuantumCircuit {1};

    auto circuit1 = ket::QuantumCircuit {1};
    circuit1.add_x_gate(0);

    auto circuit2 = ket::QuantumCircuit {1};
    circuit2.add_y_gate(0);

    auto circuit3 = ket::QuantumCircuit {1};
    circuit3.add_z_gate(0);

    return MixedUnitaryChannel {
        {.coefficient=coeff0,   .unitary=std::move(circuit0)},
        {.coefficient=coeff123, .unitary=std::move(circuit1)},
        {.coefficient=coeff123, .unitary=std::move(circuit2)},
        {.coefficient=coeff123, .unitary=std::move(circuit3)},
    };
}

}  // namespace ket

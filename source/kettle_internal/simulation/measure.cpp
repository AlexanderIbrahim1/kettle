#include <cstddef>

#include "kettle_internal/common/utils_internal.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/state.hpp"

#include "kettle_internal/gates/primitive_gate/gate_create.hpp"
#include "kettle_internal/simulation/gate_pair_generator.hpp"
#include "kettle_internal/simulation/measure.hpp"

namespace ket::internal
{

auto probabilities_of_collapsed_states_(
    ket::QuantumState& state,
    const ket::GateInfo& info
) -> std::tuple<double, double>
{
    const auto target_index = ket::internal::create::unpack_single_qubit_gate_index(info);

    auto pair_iterator = ket::internal::SingleQubitGatePairGenerator {target_index, state.n_qubits()};
    pair_iterator.set_state(0);

    auto prob_of_0_states = double {0.0};
    auto prob_of_1_states = double {0.0};

    for (std::size_t i {0}; i < pair_iterator.size(); ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        prob_of_0_states += std::norm(state[state0_index]);
        prob_of_1_states += std::norm(state[state1_index]);
    }

    return {prob_of_0_states, prob_of_1_states};
}

template <int StateToCollapse>
void collapse_and_renormalize_(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
)
{
    const auto target_index = ket::internal::create::unpack_single_qubit_gate_index(info);

    auto pair_iterator = ket::internal::SingleQubitGatePairGenerator {target_index, state.n_qubits()};
    pair_iterator.set_state(0);

    for (std::size_t i {0}; i < pair_iterator.size(); ++i) {
        const auto [state0_index, state1_index] = pair_iterator.next();

        if constexpr (StateToCollapse == 0) {
            state[state0_index] = {0.0, 0.0};
            state[state1_index] *= norm_of_surviving_state;
        }
        else if constexpr (StateToCollapse == 1) {
            state[state0_index] *= norm_of_surviving_state;
            state[state1_index] = {0.0, 0.0};
        }
        else {
            static_assert(ket::internal::always_false<void>::value, "Invalid integer provided for state collapse.");
        }
    }
}
template
void collapse_and_renormalize_<0>(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
);
template
void collapse_and_renormalize_<1>(
    ket::QuantumState& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
);

}  // namespace ket::internal

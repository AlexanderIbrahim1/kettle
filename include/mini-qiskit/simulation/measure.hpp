#pragma once

#include <cstddef>
#include <optional>
#include <random>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/common/prng.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/primitive_gate.hpp"
#include "mini-qiskit/state.hpp"
#include "mini-qiskit/simulation/gate_pair_generator.hpp"
#include "mini-qiskit/simulation/simulate_utils.hpp"

namespace impl_mqis
{

inline auto probabilities_of_collapsed_states_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits
) -> std::tuple<double, double>
{
    const auto target_index = unpack_single_qubit_gate_index(info);

    auto pair_iterator = SingleQubitGatePairGenerator {target_index, n_qubits};
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
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    double norm_of_surviving_state
)
{
    const auto target_index = unpack_single_qubit_gate_index(info);

    auto pair_iterator = SingleQubitGatePairGenerator {target_index, n_qubits};
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
            static_assert(always_false<void>::value, "Invalid integer provided for state collapse.");
        }
    }
}

/*
    Perform a measurement at the target qubit index, which collapses the state.

    For the time being, this is only done with a single-threaded implementation, because the
    threads for the multithreaded implementation are spawned before entering the simulation
    loop.
*/
template <DiscreteDistribution Distribution = std::discrete_distribution<int>>
void simulate_measurement_(
    mqis::QuantumState& state,
    const mqis::GateInfo& info,
    std::size_t n_qubits,
    std::optional<int> seed = std::nullopt
)
{
    const auto [prob_of_0_states, prob_of_1_states] = probabilities_of_collapsed_states_(state, info, n_qubits);

    auto prng = get_prng_(seed);
    auto coin_flipper = Distribution {{prob_of_0_states, prob_of_1_states}};

    if (coin_flipper(prng) == 0) {
        const auto norm = std::sqrt(1.0 / prob_of_0_states);
        collapse_and_renormalize_<1>(state, info, n_qubits, norm);
    }
    else {
        const auto norm = std::sqrt(1.0 / prob_of_1_states);
        collapse_and_renormalize_<0>(state, info, n_qubits, norm);
    }
}

}  // namespace impl_mqis

#pragma once

#include <optional>
#include <random>

#include "kettle_internal/common/prng.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/state/density_matrix.hpp"


namespace ket::internal
{

auto probabilities_of_collapsed_states_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info
) -> std::tuple<double, double>;

template <int StateToCollapse>
void collapse_and_renormalize_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    double norm_of_surviving_state
);

/*
    Perform a measurement at the target qubit index, which collapses the state.

    For the time being, this is only done with a single-threaded implementation, because the
    threads for the multithreaded implementation are spawned before entering the simulation
    loop.
*/
template <ket::internal::DiscreteDistribution Distribution = std::discrete_distribution<int>>
auto simulate_measurement_(
    ket::DensityMatrix& state,
    const ket::GateInfo& info,
    std::optional<int> seed = std::nullopt
) -> Distribution::result_type
{
    const auto [prob_of_0_states, prob_of_1_states] = probabilities_of_collapsed_states_(state, info);

    auto prng = ket::internal::get_prng_(seed);
    auto coin_flipper = Distribution {{prob_of_0_states, prob_of_1_states}};

    const auto collapsed_state = coin_flipper(prng);

    if (collapsed_state == 0) {
        const auto norm = std::sqrt(1.0 / prob_of_0_states);
        collapse_and_renormalize_<1>(state, info, norm);
    }
    else {
        const auto norm = std::sqrt(1.0 / prob_of_1_states);
        collapse_and_renormalize_<0>(state, info, norm);
    }

    return collapsed_state;
}

}  // namespace ket::internal

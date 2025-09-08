#pragma once

#include <cstdint>
#include <optional>

#include "kettle_internal/common/prng.hpp"

namespace ket::internal
{

enum class MeasurementOutcome : std::uint8_t
{
    FORCE_RESET_TO_0,
    FORCE_RESET_TO_1,
    MEASURE_BASED_ON_PROBABILITIES
};

template <ket::internal::DiscreteDistribution Distribution>
auto collapse_state(
    MeasurementOutcome measure,
    std::optional<int> seed,
    double prob_of_0_states,
    double prob_of_1_states
)
{
    if (measure == MeasurementOutcome::MEASURE_BASED_ON_PROBABILITIES) {
        auto prng = ket::internal::get_prng_(seed);
        auto coin_flipper = Distribution {{prob_of_0_states, prob_of_1_states}};

        return coin_flipper(prng);
    }
    else if (measure == MeasurementOutcome::FORCE_RESET_TO_0) {
        return 0;
    }
    else {
        return 1;
    }
}

}  // namespace ket::internal
#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>

#include "kettle/common/mathtools.hpp"

namespace impl_ket
{

/*
    The SingleQubitGatePairGenerator loops over all pairs of computational states which
    differ on bit `target_index`, and yields them using the `next()` member function.

    The number of yielded pairs is always 2^(n_qubits - 1).

    Separating the index looping from the simulation code makes it easier to test if the
    correct pairs of indices are being chosen.

    C++20 doesn't support generators :(

    I don't need the full iterator protocol for these objects, so I don't bother with it.
*/
class SingleQubitGatePairGenerator
{
public:
    constexpr SingleQubitGatePairGenerator(std::size_t target_index, std::size_t n_qubits)
        : i0_max_ {impl_ket::pow_2_int(target_index)}
        , i1_max_ {impl_ket::pow_2_int(n_qubits - target_index - 1)}
    {}

    constexpr void set_state(std::size_t i_state) noexcept
    {
        std::tie(i0_, i1_) = flat_index_to_grid_indices_2d(i_state, i1_max_);
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return i0_max_ * i1_max_;
    }

    constexpr auto next() noexcept -> std::tuple<std::size_t, std::size_t>
    {
        // indices corresponding to the computational basis states where the [i1]^th digit
        // are 0 and 1, respectively
        const auto state0_index = i0_ + (2 * i1_ * i0_max_);
        const auto state1_index = state0_index + i0_max_;

        ++i1_;
        if (i1_ == i1_max_) {
            ++i0_;
            i1_ = 0;
        }

        return {state0_index, state1_index};
    }

private:
    std::size_t i0_max_;
    std::size_t i1_max_;
    std::size_t i0_ {0};
    std::size_t i1_ {0};
};

/*
    The DoubleQubitGatePairIterator loops over all pairs of computational states where
      - in the first state, the qubits at (control_index, target_index) are (1, 0)
      - in the second state, the qubits at (control_index, target_index) are (1, 1)
    and yields them using the `next()` member function.

    The number of yielded pairs is always 2^(n_qubits - 2).

    Separating the index looping from the simulation code makes it easier to test if the
    correct pairs of indices are being chosen.

    C++20 doesn't support generators :(
*/
class DoubleQubitGatePairGenerator
{
public:
    DoubleQubitGatePairGenerator(std::size_t control_index, std::size_t target_index, std::size_t n_qubits)
        : lower_index_ {std::min({control_index, target_index})}
        , upper_index_ {std::max({control_index, target_index})}
        , lower_shift_ {impl_ket::pow_2_int(lower_index_ + 1)}
        , upper_shift_ {impl_ket::pow_2_int(upper_index_ + 1)}
        , control_shift_ {impl_ket::pow_2_int(control_index)}
        , target_shift_ {impl_ket::pow_2_int(target_index)}
        , i0_max_ {impl_ket::pow_2_int(lower_index_)}
        , i1_max_ {impl_ket::pow_2_int(upper_index_ - lower_index_ - 1)}
        , i2_max_ {impl_ket::pow_2_int(n_qubits - upper_index_ - 1)}
    {}

    constexpr void set_state(std::size_t i_state) noexcept
    {
        std::tie(i0_, i1_, i2_) = flat_index_to_grid_indices_3d(i_state, i1_max_, i2_max_);
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return i0_max_ * i1_max_ * i2_max_;
    }

    constexpr auto next() noexcept -> std::tuple<std::size_t, std::size_t>
    {
        const auto state0_index = i0_ + (i1_ * lower_shift_) + (i2_ * upper_shift_) + control_shift_;
        const auto state1_index = state0_index + target_shift_;

        ++i2_;
        if (i2_ == i2_max_) {
            ++i1_;
            i2_ = 0;

            if (i1_ == i1_max_) {
                ++i0_;
                i1_ = 0;
            }
        }

        return {state0_index, state1_index};
    }

private:
    std::size_t lower_index_;
    std::size_t upper_index_;
    std::size_t lower_shift_;
    std::size_t upper_shift_;
    std::size_t control_shift_;
    std::size_t target_shift_;
    std::size_t i0_max_;
    std::size_t i1_max_;
    std::size_t i2_max_;
    std::size_t i0_ {0};
    std::size_t i1_ {0};
    std::size_t i2_ {0};
};

}  // namespace impl_ket

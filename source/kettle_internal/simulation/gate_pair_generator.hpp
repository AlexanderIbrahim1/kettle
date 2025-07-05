#pragma once

#include <algorithm>
#include <tuple>

#include "kettle_internal/common/mathtools_internal.hpp"

namespace ket::internal
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
template <typename T>
class SingleQubitGatePairGenerator
{
public:
    SingleQubitGatePairGenerator(T target_index, T n_qubits)
        : i0_max_ {ket::internal::pow_2_int(target_index)}
        , i1_max_ {ket::internal::pow_2_int(n_qubits - target_index - 1)}
    {}

    void set_state(T i_state) noexcept
    {
        std::tie(i0_, i1_) = ket::internal::flat_index_to_grid_indices_2d(i_state, i1_max_);
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> T
    {
        return i0_max_ * i1_max_;
    }

    constexpr auto next() noexcept -> std::tuple<T, T>
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
    T i0_max_;
    T i1_max_;
    T i0_ {0};
    T i1_ {0};
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
template <typename T>
class DoubleQubitGatePairGenerator
{
public:
    DoubleQubitGatePairGenerator(T control_index, T target_index, T n_qubits)
        : lower_index_ {std::min({control_index, target_index})}
        , upper_index_ {std::max({control_index, target_index})}
        , lower_shift_ {ket::internal::pow_2_int(lower_index_ + 1)}
        , upper_shift_ {ket::internal::pow_2_int(upper_index_ + 1)}
        , control_shift_ {ket::internal::pow_2_int(control_index)}
        , target_shift_ {ket::internal::pow_2_int(target_index)}
        , i0_max_ {ket::internal::pow_2_int(lower_index_)}
        , i1_max_ {ket::internal::pow_2_int(upper_index_ - lower_index_ - 1)}
        , i2_max_ {ket::internal::pow_2_int(n_qubits - upper_index_ - 1)}
    {}

    void set_state(T i_state) noexcept
    {
        std::tie(i0_, i1_, i2_) = ket::internal::flat_index_to_grid_indices_3d(i_state, i1_max_, i2_max_);
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> T
    {
        return i0_max_ * i1_max_ * i2_max_;
    }

    constexpr auto next() noexcept -> std::tuple<T, T>
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

    constexpr auto next_unset_and_set() noexcept -> std::tuple<T, T, T, T>
    {
        const auto index_c0_t0 = i0_ + (i1_ * lower_shift_) + (i2_ * upper_shift_);
        const auto index_c0_t1 = index_c0_t0 + target_shift_;
        const auto index_c1_t0 = index_c0_t0 + control_shift_;
        const auto index_c1_t1 = index_c0_t1 + control_shift_;

        ++i2_;
        if (i2_ == i2_max_) {
            ++i1_;
            i2_ = 0;

            if (i1_ == i1_max_) {
                ++i0_;
                i1_ = 0;
            }
        }

        return {index_c0_t0, index_c0_t1, index_c1_t0, index_c1_t1};
    }

private:
    T lower_index_;
    T upper_index_;
    T lower_shift_;
    T upper_shift_;
    T control_shift_;
    T target_shift_;
    T i0_max_;
    T i1_max_;
    T i2_max_;
    T i0_ {0};
    T i1_ {0};
    T i2_ {0};
};

}  // namespace ket::internal

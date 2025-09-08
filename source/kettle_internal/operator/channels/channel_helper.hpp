#pragma once

#include <cmath>
#include <cstddef>
#include <ranges>
#include <vector>

namespace ket::internal
{

template <typename T, typename F>
auto almost_eq_helper_(
    const T& left_op,
    const T& right_op,
    F almost_eq_func
) -> bool
{
    if (left_op.size() != right_op.size()) {
        return false;
    }

    const auto size = left_op.size();
    for (std::size_t i {0}; i < size; ++i) {
        const auto& left = left_op.at(i);
        const auto& right = right_op.at(i);

        if (!almost_eq_func(left, right)) {
            return false;
        }
    }

    return true;
}

/*
    Custom type that loops over all possible Cartesian product combinations; this is because the
    current codebase is sticking with C++20, and `std::ranges::views::cartesian_product()` is not
    available until C++23 :(
*/
class CartesianTicker
{
public:
    CartesianTicker(std::size_t n_elements, std::size_t period)
        : period_ {period}
        , ticker_(n_elements, 0)
    {}

    [[nodiscard]]
    constexpr auto size() const -> std::size_t
    {
        // TODO: replace with internal power function when we split the header and source
        return static_cast<std::size_t>(std::pow(period_, ticker_.size()));
    }

    [[nodiscard]]
    constexpr auto ticker() const -> const std::vector<std::size_t>&
    {
        return ticker_;
    }

    constexpr void increment()
    {
        namespace sv = std::views;

        for (std::size_t i : sv::iota(0UL, ticker_.size()) | sv::reverse) {

            ticker_[i] += 1;

            if (ticker_[i] == period_) {
                ticker_[i] = 0;
                continue;
            }
        
            break;
        }
    }

private:
    std::size_t period_;
    std::vector<std::size_t> ticker_;
};


}  // namespace ket::internal

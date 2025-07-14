#pragma once

#include <cstddef>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/operator/channels/kraus_common.hpp"


namespace ket
{

class OneQubitKrausChannel
{
public:
    OneQubitKrausChannel(
        std::vector<Matrix2X2> kraus_matrices,
        std::size_t target_index,
        double tolerance = 1.0e-6
    );

    OneQubitKrausChannel(
        std::vector<Matrix2X2> kraus_matrices,
        std::size_t target_index,
        kraus_channel_nocheck tag
    );

    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t
    {
        return kraus_matrices_.size();
    }

    [[nodiscard]]
    constexpr auto matrices() const -> const std::vector<Matrix2X2>& {
        return kraus_matrices_;
    }

    [[nodiscard]]
    constexpr auto target_index() const -> std::size_t {
        return target_index_;
    }

    [[nodiscard]]
    auto at(std::size_t index) const -> const ket::Matrix2X2&
    {
        return kraus_matrices_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> ket::Matrix2X2&
    {
        return kraus_matrices_[index];
    }

private:
    std::vector<Matrix2X2> kraus_matrices_;
    std::size_t target_index_;
};

// TODO: replace magic number for tolerance
auto almost_eq(
    const OneQubitKrausChannel& left_op,
    const OneQubitKrausChannel& right_op,
    double coeff_tolerance = 1.0e-6
) -> bool;

}  // namespace ket

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
    constexpr auto matrices() const -> const std::vector<Matrix2X2>& {
        return kraus_matrices_;
    }

    [[nodiscard]]
    constexpr auto target_index() const -> std::size_t {
        return target_index_;
    }

private:
    std::vector<Matrix2X2> kraus_matrices_;
    std::size_t target_index_;
};

}  // namespace ket

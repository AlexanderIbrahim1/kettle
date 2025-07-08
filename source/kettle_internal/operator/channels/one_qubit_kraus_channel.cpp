#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/operator/channels/kraus_common.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"

namespace
{

auto is_valid_one_qubit_kraus_channel(
    const std::vector<ket::Matrix2X2>& matrices,
    double tolerance
) -> bool
{
    const auto product = [](const ket::Matrix2X2& current, const ket::Matrix2X2& mat) -> ket::Matrix2X2 {
        return current + (ket::conjugate_transpose(mat) * mat);
    };

    const auto total = std::accumulate(matrices.begin(), matrices.end(), ket::Matrix2X2 {}, product);

    return ket::almost_eq(total, ket::i_gate(), tolerance);
}

}  // namespace


namespace ket
{

/*
    A tag passed to the constructor of the `KrausChannel` object to skip checking
    whether or not the matrices form a valid channel.
*/

OneQubitKrausChannel::OneQubitKrausChannel(
    std::vector<Matrix2X2> kraus_matrices,
    std::size_t target_index,
    double tolerance
)
    : kraus_matrices_ {std::move(kraus_matrices)}
    , target_index_ {target_index}
{
    if (kraus_matrices_.empty()) {
        throw std::runtime_error {"ERROR: cannot create a Kraus channel with no Kraus matrices.\n"};
    }

    if (!is_valid_one_qubit_kraus_channel(kraus_matrices_, tolerance)) {
        throw std::runtime_error {"ERROR: sum of products of Kraus matrices do not give the identity matrix.\n"};
    }
}

OneQubitKrausChannel::OneQubitKrausChannel(
    std::vector<Matrix2X2> kraus_matrices,
    std::size_t target_index,
    [[maybe_unused]] kraus_channel_nocheck tag
)
    : kraus_matrices_ {std::move(kraus_matrices)}
    , target_index_ {target_index}
{
    if (kraus_matrices_.empty()) {
        throw std::runtime_error {"ERROR: cannot create a Kraus channel with no Kraus matrices.\n"};
    }
}

auto depolarizing_noise(double parameter, std::size_t target_index) -> OneQubitKrausChannel
{
    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = std::sqrt(1.0 - parameter);
    const auto coeff123 = std::sqrt(parameter / 3.0);
    const auto mat0 = coeff0 * ket::i_gate();
    const auto mat1 = coeff123 * ket::x_gate();
    const auto mat2 = coeff123 * ket::y_gate();
    const auto mat3 = coeff123 * ket::z_gate();

    return OneQubitKrausChannel {{mat0, mat1, mat2, mat3}, target_index};
}

}  // namespace ket

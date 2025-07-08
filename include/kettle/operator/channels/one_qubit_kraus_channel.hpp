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


/*
    The symmetric depolarizing error channel applied to a single qubit.

    Kraus channels are not unique, and there are multiple definitions in the literature.
    For this definition:
      - p = 0 gives a noiseless channel
      - p = 3/4 gives a full depolarized channel, and the output will be proportional to the identity matrix
      - p = 1 gives the uniform Pauli error channel, where X, Y, and Z are applied equally to the 1-qubit density matrix
    
    NOTE: replace this function with the Pauli gate implementation at some point in the future
      - because that one naturally extends to multiple qubits
*/
auto depolarizing_noise(double parameter, std::size_t target_index) -> OneQubitKrausChannel;

}  // namespace ket

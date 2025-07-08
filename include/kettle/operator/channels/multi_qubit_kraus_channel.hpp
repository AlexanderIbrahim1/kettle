#pragma once

#include <cstddef>
#include <vector>

#include <Eigen/Dense>

#include "kettle/operator/channels/kraus_common.hpp"


namespace ket
{

class MultiQubitKrausChannel
{
public:
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    MultiQubitKrausChannel(
        std::vector<Eigen::MatrixXcd> kraus_matrices,
        double tolerance = 1.0e-6
    );

    MultiQubitKrausChannel(
        std::vector<Eigen::MatrixXcd> kraus_matrices,
        kraus_channel_nocheck tag
    );

    [[nodiscard]]
    constexpr auto matrices() const -> const std::vector<Eigen::MatrixXcd>& {
        return kraus_matrices_;
    }

    [[nodiscard]]
    constexpr auto n_input_qubits() const -> std::size_t
    {
        return n_input_qubits_;
    }

    [[nodiscard]]
    constexpr auto n_output_qubits() const -> std::size_t
    {
        return n_output_qubits_;
    }

private:
    std::vector<Eigen::MatrixXcd> kraus_matrices_;
    std::size_t n_input_qubits_ {};
    std::size_t n_output_qubits_ {};
};

}  // namespace ket

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
    constexpr auto size() const noexcept -> std::size_t
    {
        return kraus_matrices_.size();
    }

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

    [[nodiscard]]
    auto at(std::size_t index) const -> const Eigen::MatrixXcd&
    {
        return kraus_matrices_[index];
    }

    [[nodiscard]]
    auto at(std::size_t index) -> Eigen::MatrixXcd&
    {
        return kraus_matrices_[index];
    }

private:
    std::vector<Eigen::MatrixXcd> kraus_matrices_;
    std::size_t n_input_qubits_ {};
    std::size_t n_output_qubits_ {};
};

// TODO: replace magic number
auto almost_eq(
    const MultiQubitKrausChannel& left_op,
    const MultiQubitKrausChannel& right_op,
    double coeff_tolerance = 1.0e-6
) -> bool;

}  // namespace ket

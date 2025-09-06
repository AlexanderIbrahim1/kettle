#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "kettle/operator/channels/kraus_common.hpp"
#include "kettle/state/qubit_state_conversion.hpp"


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

inline auto kraus_matrix_from_projectors(
    std::span<const std::string> bitstrings,
    std::span<const std::complex<double>> amplitudes,
    Endian input_endian = Endian::LITTLE
) -> Eigen::MatrixXcd
{
    if (amplitudes.empty()) {
        throw std::runtime_error {"ERROR: cannot construct Kraus matrix without amplitudes.\n"};
    }

    if (amplitudes.size() != bitstrings.size()) {
        throw std::runtime_error {"ERROR: number of amplitudes and states do not match.\n"};
    }

    const auto same_size_as_0 = [&](const auto& bstring) { return bstring.size() == bitstrings[0].size(); };
    if (!std::ranges::all_of(bitstrings, same_size_as_0)) {
        throw std::runtime_error {"ERROR: all bitstrings must be the same size\n"};
    }

    const auto n_states = static_cast<Eigen::Index>(1UL << bitstrings[0].size());
    auto kraus_matrix = Eigen::MatrixXcd::Zero(n_states, n_states).eval();

    const auto n_projectors = amplitudes.size();
    for (std::size_t i {0}; i < n_projectors; ++i) {
        const auto state_index_ = bitstring_to_state_index(bitstrings[i], input_endian);
        const auto state_index = static_cast<Eigen::Index>(state_index_);

        kraus_matrix(state_index, state_index) = amplitudes[i];
    }

    return kraus_matrix;
}

/*
    Overload that assumes that the amplitudes in front of the projection
    terms are all 1.
*/
inline auto kraus_matrix_from_projectors(
    std::span<const std::string> bitstrings,
    Endian input_endian = Endian::LITTLE
) -> Eigen::MatrixXcd
{
    auto amplitudes = std::vector<std::complex<double>>(bitstrings.size(), {1.0, 0.0});
    return kraus_matrix_from_projectors(bitstrings, amplitudes, input_endian);
}

}  // namespace ket

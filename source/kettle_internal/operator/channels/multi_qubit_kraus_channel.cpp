#include <numeric>
#include <stdexcept>
#include <vector>

#include <Eigen/Dense>

#include "kettle/operator/channels/kraus_common.hpp"
#include "kettle/operator/channels/multi_qubit_kraus_channel.hpp"
#include "kettle_internal/common/mathtools_internal.hpp"
#include "kettle_internal/common/utils_internal.hpp"
#include "kettle_internal/operator/channels/almost_eq_helper.hpp"

namespace ki = ket::internal;


namespace
{

auto is_valid_multi_qubit_kraus_channel(
    const std::vector<Eigen::MatrixXcd>& matrices,
    Eigen::Index n_input_qubits,
    double tolerance
) -> bool
{
    // NOTE: this function assumes that the sizes of the matrices have already been checked
    // and are all the same
    const auto product = [](const Eigen::MatrixXcd& current, const Eigen::MatrixXcd& mat) -> Eigen::MatrixXcd {
        return current + (mat.adjoint() * mat);
    };

    const auto n_states = ki::pow_2_int(n_input_qubits);

    auto initial = Eigen::MatrixXcd::Zero(n_states, n_states).eval();
    const auto total = std::accumulate(matrices.begin(), matrices.end(), initial, product);

    const auto expected = Eigen::MatrixXcd::Identity(n_states, n_states).eval();

    return total.isApprox(expected, tolerance);
}

auto are_all_matrices_same_size_(const std::vector<Eigen::MatrixXcd>& matrices) -> bool
{
    const auto get_size = [](const auto& matrix) {
        return std::pair {matrix.rows(), matrix.cols()};
    };

    return ki::all_same(matrices, get_size);
}

}  // namespace

namespace ket
{

// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
MultiQubitKrausChannel::MultiQubitKrausChannel(
    std::vector<Eigen::MatrixXcd> kraus_matrices,
    double tolerance
)
    : kraus_matrices_ {std::move(kraus_matrices)}
{
    if (kraus_matrices_.empty()) {
        throw std::runtime_error {"ERROR: cannot create a Kraus channel with no Kraus matrices.\n"};
    }

    if (!are_all_matrices_same_size_(kraus_matrices_)) {
        throw std::runtime_error {"All matrices in the MultiQubitKrausChannel must have the same size.\n"};
    }

    const auto& ref_matrix = kraus_matrices_[0];

    if (!ki::is_power_of_2(ref_matrix.rows())) {
        throw std::runtime_error {"ERROR: The number of rows for the Kraus channel is not a power of 2.\n"};
    }

    if (!ki::is_power_of_2(ref_matrix.cols())) {
        throw std::runtime_error {"ERROR: The number of columns for the Kraus channel is not a power of 2.\n"};
    }

    n_input_qubits_ = ki::log_2_int(ref_matrix.cols());
    n_output_qubits_ = ki::log_2_int(ref_matrix.rows());

    if (!is_valid_multi_qubit_kraus_channel(kraus_matrices_, static_cast<Eigen::Index>(n_input_qubits_), tolerance)) {
        throw std::runtime_error {"ERROR: sum of products of Kraus matrices do not give the identity matrix.\n"};
    }
}

MultiQubitKrausChannel::MultiQubitKrausChannel(
    std::vector<Eigen::MatrixXcd> kraus_matrices,
    [[maybe_unused]] kraus_channel_nocheck tag
)
    : kraus_matrices_ {std::move(kraus_matrices)}
    , n_input_qubits_ {ki::log_2_int(kraus_matrices_[0].cols())}
    , n_output_qubits_ {ki::log_2_int(kraus_matrices_[0].rows())}
{}

auto almost_eq(
    const MultiQubitKrausChannel& left_op,
    const MultiQubitKrausChannel& right_op,
    double coeff_tolerance
) -> bool
{
    const auto almost_eq = [coeff_tolerance](const Eigen::MatrixXcd& left, const Eigen::MatrixXcd& right) {
        return left.isApprox(right, coeff_tolerance);
    };

    return ki::almost_eq_helper_(left_op, right_op, almost_eq);
}

}  // namespace ket

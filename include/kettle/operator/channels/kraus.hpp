#pragma once

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <variant>
#include <vector>

#include <Eigen/Dense>

#include <kettle/common/matrix2x2.hpp>
#include "kettle/gates/common_u_gates.hpp"

/*
    Implement a KrausChannel object
      - it will hold a sequence of matrices
    
    Create a function that takes a KrausChannel and a density matrix, and evolve the density matrix

    NOTE: many Kraus channels only act on a single qubit, or maybe two qubits
      - so I should have a KrausMatrix object
        - it might be the equivalent of a CU gate
        - it might be a full matrix
*/


namespace ket
{

template <typename T>
struct kraus_matrix_type_always_false: std::false_type
{};

class OneQubitKrausMatrix
{
public:
    OneQubitKrausMatrix(const Matrix2X2& matrix, std::size_t target_index)
        : matrix_ {matrix}
        , target_index_ {target_index}
    {}

    [[nodiscard]]
    constexpr auto target_index() const -> std::size_t
    {
        return target_index_;
    }

    [[nodiscard]]
    constexpr auto matrix() const -> const Matrix2X2&
    {
        return matrix_;
    }

private:
    Matrix2X2 matrix_;
    std::size_t target_index_;
};

class ControlledQubitKrausMatrix
{
public:
    ControlledQubitKrausMatrix(Eigen::MatrixXcd matrix, std::size_t control_index, std::size_t target_index)
        : matrix_ {std::move(matrix)}
        , control_index_ {control_index}
        , target_index_ {target_index}
    {
        if (control_index_ == target_index_) {
            throw std::runtime_error {"ERROR: the control index and target index cannot match.\n"};
        }

        if (matrix_.cols() != 4 || matrix.rows() != 4) {
            throw std::runtime_error {"ERROR: the matrix for a ControlledQubitKrausMatrix must be a 4x4 matrix.\n"};
        }
    }

    [[nodiscard]]
    constexpr auto control_index() const -> std::size_t
    {
        return control_index_;
    }

    [[nodiscard]]
    constexpr auto target_index() const -> std::size_t
    {
        return target_index_;
    }

    [[nodiscard]]
    constexpr auto matrix() const -> const Eigen::MatrixXcd&
    {
        return matrix_;
    }

private:
    Eigen::MatrixXcd matrix_;
    std::size_t control_index_;
    std::size_t target_index_;
};

class FullKrausMatrix
{
public:
    explicit FullKrausMatrix(Eigen::MatrixXcd matrix)
        : matrix_ {std::move(matrix)}
    {
        // TODO: check if the matrix side lengths are 2^m x 2^n
        // TODO: I should take the functions in `density_matrix.cpp` and move them into a separate compilation unit
        // so I can reuse them here
    }

    [[nodiscard]]
    constexpr auto matrix() const -> const Eigen::MatrixXcd&
    {
        return matrix_;
    }

    [[nodiscard]]
    constexpr auto n_input_qubits() const -> const std::size_t&
    {
        return n_input_qubits_;
    }

    [[nodiscard]]
    constexpr auto n_output_qubits() const -> const std::size_t&
    {
        return n_output_qubits_;
    }

private:
    Eigen::MatrixXcd matrix_;
    std::size_t n_input_qubits_ {};
    std::size_t n_output_qubits_ {};
};

using KrausMatrixInfo = std::variant<OneQubitKrausMatrix, ControlledQubitKrausMatrix, FullKrausMatrix>;


/*
    A tag passed to the constructor of the `KrausChannel` object to skip checking
    whether or not the matrices form a valid channel.
*/
struct kraus_channel_nocheck {};


template <typename Element, typename Function>
auto all_same(const std::vector<Element>& elements, Function function) -> bool
{
    if (elements.empty()) {
        return true;
    }

    const auto value0 = function(elements[0]);
    const auto it1 = std::next(std::begin(elements));

    return std::all_of(it1, elements.end(), [&](const auto& elem) { return function(elem) == value0; });
}


inline auto is_valid_kraus_channel(
    const std::vector<OneQubitKrausMatrix>& matrices,
    double tolerance
) -> bool
{
    const auto product = [](const Matrix2X2& current, const OneQubitKrausMatrix& mat) -> Matrix2X2 {
        return current + (ket::conjugate_transpose(mat.matrix()) * mat.matrix());
    };

    const auto total = std::accumulate(matrices.begin(), matrices.end(), Matrix2X2 {}, product);

    return ket::almost_eq(total, ket::i_gate(), tolerance);
}

/*
    Current restrictions:
      - all KrausMatrixInfo instances must be of the same variant type
      - all OneQubitKrausMatrix instances must have the same target index
      - all ControlledQubitKrausMatrix instances must have the same control and target qubit indices
    
    Technically a Kraus channel can still be a Kraus channel without the above being true, but it
    greatly reduces the number of conditions and code to write.

    I can remove these restrictions at a later time.
*/
template <typename KrausMatrixType>
class KrausChannel
{
public:
    explicit KrausChannel(
        std::vector<KrausMatrixType> kraus_matrices,
        double tolerance = 1.0e-6
    )
        : kraus_matrices_ {std::move(kraus_matrices)}
    {
        if (kraus_matrices_.empty()) {
            throw std::runtime_error {"ERROR: cannot create a Kraus channel with no Kraus matrices.\n"};
        }

        if constexpr (std::is_same_v<KrausMatrixType, OneQubitKrausMatrix>) {
            const auto get_target_index = [&](const auto& elem) {
                return elem.target_index();
            };

            if (!all_same(kraus_matrices_, get_target_index)) {
                throw std::runtime_error {"ERROR: all OneQubitKrausMatrix instances must have the same target index.\n"};
            }
        }
        else if constexpr (std::is_same_v<KrausMatrixType, ControlledQubitKrausMatrix>) {
            const auto get_both_indices = [&](const auto& elem) {
                return std::pair {elem.control_index(), elem.target_index()};
            };

            if (!all_same(kraus_matrices_, get_both_indices )) {
                throw std::runtime_error {"ERROR: all ControlledQubitKrausMatrix instances must have matching indices.\n"};
            }
        }
        else if constexpr (std::is_same_v<KrausMatrixType, FullKrausMatrix>) {
            const auto get_both_indices = [&](const auto& elem) {
                return std::pair {elem.n_input_qubits(), elem.n_output_qubits()};
            };

            if (!all_same(kraus_matrices_, get_both_indices )) {
                throw std::runtime_error {"ERROR: all FullKrausMatrix instances must have matching input and output qubit sizes.\n"};
            }
        }
        else {
            static_assert(kraus_matrix_type_always_false<void>::value, "Invalid KrausMatrixType provided.");
        }

        // NOTE: focus on the 1-qubit Kraus matrices for now
        // TODO: implement this function for the other Kraus matrix types
        if (!is_valid_kraus_channel(kraus_matrices_, tolerance)) {
            throw std::runtime_error {"ERROR: sum of products of Kraus matrices do not give the identity matrix.\n"};
        }
    }

    [[nodiscard]]
    constexpr auto matrices() const -> const std::vector<KrausMatrixType>& {
        return kraus_matrices_;
    }

private:
    std::vector<KrausMatrixType> kraus_matrices_;
//         const auto get_variant_index = [&](const KrausMatrixInfo& info) { return info.index(); };
//         if (!all_same(kraus_matrices_, get_variant_index)) {
//             throw std::runtime_error {"ERROR: all Kraus matrices must have the same internal representation.\n"};
//         }
};

// TEMPORARY FUNCTION: I plan to use a QuantumNoise instance to wrap around the channels and error models
/*
    The symmetric depolarizing error channel applied to a single qubit.

    Kraus channels are not unique, and there are multiple definitions in the literature.
    For this definition:
      - p = 0 gives a noiseless channel
      - p = 3/4 gives a full depolarized channel, and the output will be proportional to the identity matrix
      - p = 1 gives the uniform Pauli error channel, where X, Y, and Z are applied equally to the 1-qubit density matrix
*/
inline auto depolarizing_noise(double parameter, std::size_t target_index) -> KrausChannel<OneQubitKrausMatrix>
{
    if (parameter < 0.0 || parameter > 1.0) {
        throw std::runtime_error {"ERROR: the depolarizing noise parameter must be in [0.0, 1.0].\n"};
    }

    const auto coeff0 = std::sqrt(1.0 - parameter);
    const auto coeff123 = std::sqrt(parameter / 3.0);
    const auto mat0 = OneQubitKrausMatrix {coeff0 * ket::i_gate(), target_index};
    const auto mat1 = OneQubitKrausMatrix {coeff123 * ket::x_gate(), target_index};
    const auto mat2 = OneQubitKrausMatrix {coeff123 * ket::y_gate(), target_index};
    const auto mat3 = OneQubitKrausMatrix {coeff123 * ket::z_gate(), target_index};

    return KrausChannel<OneQubitKrausMatrix> {{mat0, mat1, mat2, mat3}};
}

}  // namespace ket

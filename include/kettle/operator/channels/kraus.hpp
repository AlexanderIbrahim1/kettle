#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <variant>
#include <vector>

#include <Eigen/Dense>

#include <kettle/common/matrix2x2.hpp>

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

inline auto are_all_variants_the_same_(const std::vector<KrausMatrixInfo>& matrices) -> bool
{
    if (matrices.empty()) {
        return true;
    }

    const auto index0 = matrices[0].index();

    for (std::size_t i {1}; i < matrices.size(); ++i) {
        if (matrices[i].index() != index0) {
            return false;
        }
    }

    return true;
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
class KrausChannel
{
public:
    explicit KrausChannel(std::vector<KrausMatrixInfo> kraus_matrices)
        : kraus_matrices_ {std::move(kraus_matrices)}
    {
        if (kraus_matrices_.empty()) {
            throw std::runtime_error {"ERROR: cannot create a Kraus channel with no Kraus matrices.\n"};
        }

        const auto get_variant_index = [&](const KrausMatrixInfo& info) { return info.index(); };
        if (!all_same(kraus_matrices_, get_variant_index)) {
            throw std::runtime_error {"ERROR: all Kraus matrices must have the same internal representation.\n"};
        }

        if (std::holds_alternative<OneQubitKrausMatrix>(kraus_matrices_[0])) {
            const auto get_target_index = [&](const auto& elem) {
                return std::get<OneQubitKrausMatrix>(elem).target_index();
            };

            if (!all_same(kraus_matrices_, get_target_index)) {
                throw std::runtime_error {"ERROR: all OneQubitKrausMatrix instances must have the same target index.\n"};
            }
        }

        if (std::holds_alternative<ControlledQubitKrausMatrix>(kraus_matrices_[0])) {
            const auto get_both_indices = [&](const auto& elem) -> std::pair<std::size_t, std::size_t> {
                const auto& mat = std::get<ControlledQubitKrausMatrix>(elem);
                return {mat.control_index(), mat.target_index()};
            };

            if (!all_same(kraus_matrices_, get_both_indices )) {
                throw std::runtime_error {"ERROR: all ControlledQubitKrausMatrix instances must have matching indices.\n"};
            }
        }
    }

private:
    std::vector<KrausMatrixInfo> kraus_matrices_;
};

// TODO: add function that checks if a Kraus channel is valid

}  // namespace ket

#pragma once

#include <compare>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "kettle/circuit/classical_register.hpp"

namespace ket
{

enum class ControlFlowBooleanKind
{
    IF,
    IF_NOT
};

class ControlFlowPredicate
{
public:
    ControlFlowPredicate(
        std::vector<std::size_t> bit_indices_to_check,
        std::vector<int> expected_bits,
        ControlFlowBooleanKind control_kind
    )
        : bit_indices_to_check_ {std::move(bit_indices_to_check)}
        , expected_bits_ {std::move(expected_bits)}
        , control_kind_ {control_kind}
    {
        if (bit_indices_to_check_.size() != expected_bits_.size()) {
            auto err_msg = std::stringstream {};
            err_msg << "There is a mismatch in the number of bits to check, and the number of expected bits:\n";
            err_msg << "bit_indices_to_check.size() = " << bit_indices_to_check_.size() << '\n';
            err_msg << "expected_bits.size() = " << expected_bits_.size() << '\n';
            throw std::runtime_error {err_msg.str()};
        }

        if (bit_indices_to_check_.size() == 0) {
            throw std::runtime_error {"The control flow predicate must depend on at least one bit.\n"};
        }
    }

    auto operator()(const ClassicalRegister& creg) const -> bool
    {
        auto output = true;

        const auto n_bits_to_check = bit_indices_to_check_.size();

        // check if the bit values match their expected values, with short circuiting
        for (std::size_t i {0}; i < n_bits_to_check; ++i) {
            const auto bit_index = bit_indices_to_check_[i];
            const auto expected_bit = expected_bits_[i];

            if (!creg.is_measured(bit_index)) {
                auto err_msg = std::stringstream {};
                err_msg << "There is no measured bit at classical register " << bit_index << '\n';
                throw std::runtime_error {err_msg.str()};
            }

            if (creg.get(bit_index) != expected_bit) {
                output = false;
                break;
            }
        }

        // possibly flip the output, depending on what kind of control flow predicate is being used
        if (control_kind_ == ControlFlowBooleanKind::IF) {
            return output;
        } else {
            return !output;
        }
    }

    constexpr auto bit_indices_to_check() const -> const std::vector<std::size_t>&
    {
        return bit_indices_to_check_;
    }

    constexpr auto expected_bits() const -> const std::vector<int>&
    {
        return expected_bits_;
    }

    constexpr auto control_kind() const -> ControlFlowBooleanKind
    {
        return control_kind_;
    }

    friend constexpr auto operator<=>(const ControlFlowPredicate& left, const ControlFlowPredicate& right) noexcept = default;

private:
    std::vector<std::size_t> bit_indices_to_check_;
    std::vector<int> expected_bits_;
    ControlFlowBooleanKind control_kind_;
};

}  // namespace ket

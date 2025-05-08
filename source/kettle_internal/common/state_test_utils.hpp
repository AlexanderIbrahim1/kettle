#pragma once

#include <string>

#include "kettle/common/tolerance.hpp"
#include "kettle/state/state.hpp"


namespace ket::internal
{

enum PrintAlmostEq_ : std::uint8_t
{
    PRINT,
    NOPRINT
};

void print_state_(const ket::QuantumState& state);

auto ae_err_msg_diff_number_of_qubits_(std::size_t n_left_qubits, std::size_t n_right_qubits) -> std::string;

auto ae_err_msg_diff_states_(const ket::QuantumState& left, const ket::QuantumState& right) -> std::string;

auto almost_eq_with_print_(
    const ket::QuantumState& left,
    const ket::QuantumState& right,
    PrintAlmostEq_ print_state = PrintAlmostEq_::PRINT,
    double tolerance_sq = ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool;

}  // namespace ket::internal

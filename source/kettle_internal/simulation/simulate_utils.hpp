#pragma once

#include <cstddef>

/*
    This header file contains code to help perform the quantum circuit simulations,
    but aren't direct simulation code.
*/


namespace ket::internal
{

struct FlatIndexPair
{
    std::size_t i_lower;
    std::size_t i_upper;

    friend constexpr auto operator==(const FlatIndexPair& left, const FlatIndexPair& right) noexcept -> bool
    {
        return left.i_lower == right.i_lower && left.i_upper == right.i_upper;
    }
};

auto number_of_single_qubit_gate_pairs_(std::size_t n_qubits) -> std::size_t;

auto number_of_double_qubit_gate_pairs_(std::size_t n_qubits) -> std::size_t;

}  // namespace ket::internal

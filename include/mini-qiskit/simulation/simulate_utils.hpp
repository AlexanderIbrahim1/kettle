#pragma once

#include <compare>
#include <cstddef>
#include <stdexcept>

/*
    This header file contains code to help perform the quantum circuit simulations,
    but aren't direct simulation code.
*/


namespace impl_mqis
{

struct FlatIndexPair
{
    std::size_t i_lower;
    std::size_t i_upper;

    constexpr auto operator<=>(const FlatIndexPair&) const noexcept = default;
};

constexpr auto number_of_single_qubit_gate_pairs_(std::size_t n_qubits) -> std::size_t
{
    if (n_qubits == 0) {
        throw std::runtime_error {
            "UNREACHABLE: dev error, can't get number of single qubit gate pairs for 0 qubits\n"
        };
    }

    return static_cast<std::size_t>(1 << (n_qubits - 1));
}

constexpr auto number_of_double_qubit_gate_pairs_(std::size_t n_qubits) -> std::size_t
{
    // the case where `n_qubits == 1` does not make sense for double qubit gate simulations;
    //   - the calculation for the number of double gates is invalid
    // Luckily, this does not matter;
    //   - if there is only one qubit, then no double qubit gates should be simulated anyways
    //   - so we can set the upper qubit value to 0, so the loops never run

    if (n_qubits == 0) {
        throw std::runtime_error {
            "UNREACHABLE: dev error, can't get number of double qubit gate pairs for 0 qubits\n"
        };
    }

    if (n_qubits == 1) {
        return 0;
    } else {
        return 1ul << (n_qubits - 2);
    }
}


}  // namespace impl_mqis

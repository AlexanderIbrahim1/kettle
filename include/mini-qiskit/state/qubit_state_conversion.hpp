#pragma once

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "mini-qiskit/common/mathtools.hpp"
#include "mini-qiskit/state/bitstring_utils.hpp"
#include "mini-qiskit/state/endian.hpp"

namespace mqis
{

inline auto bitstring_to_state_index(
    const std::string& bitstring,
    mqis::QuantumStateEndian input_endian
) -> std::size_t
{
    impl_mqis::check_bitstring_is_valid_nonmarginal_(bitstring);

    constexpr auto n_bits = std::numeric_limits<std::size_t>::digits;
    const auto bits = std::bitset<n_bits> {bitstring};

    const auto big_endian_index = static_cast<std::size_t>(bits.to_ullong());

    if (input_endian == mqis::QuantumStateEndian::BIG) {
        return big_endian_index;
    }
    else if (input_endian == mqis::QuantumStateEndian::LITTLE) {
        const auto n_qubits = bitstring.size();
        const auto little_endian_index = impl_mqis::endian_flip_(big_endian_index, n_qubits);
        return little_endian_index;
    }
    else {
        throw std::runtime_error {"UNREACHABLE: invalid QuantumStateEndian provided\n"};
    }
}

inline auto state_index_to_dynamic_bitset(
    std::size_t i_state,
    std::size_t n_qubits,
    mqis::QuantumStateEndian input_endian
) -> std::vector<std::uint8_t>
{
    const auto n_states = impl_mqis::pow_2_int(n_qubits);
    if (i_state >= n_states) {
        throw std::runtime_error {"The state index exceeds the number of possible states."};
    }

    auto dyn_bitset = std::vector<std::uint8_t>(n_qubits, 0);
    for (std::size_t i_qubit {0}; i_qubit < n_qubits; ++i_qubit) {
        const auto bit_position = impl_mqis::pow_2_int(i_qubit);

        // I *could* improve performance by making this section inside the loop 'constexpr if', but
        // the performance in this area of the code isn't the bottleneck, and leaving it like this
        // makes it more flexible
        if (input_endian == mqis::QuantumStateEndian::LITTLE) {
            dyn_bitset[i_qubit] = static_cast<std::uint8_t>((bit_position & i_state) != 0);
        }
        else if (input_endian == mqis::QuantumStateEndian::BIG) {
            dyn_bitset[n_qubits - i_qubit - 1] = static_cast<std::uint8_t>((bit_position & i_state) != 0);
        }
        else {
            throw std::runtime_error {"UNREACHABLE: invalid QuantumStateEndian provided\n"};
        }
    }

    return dyn_bitset;
}

inline auto dynamic_bitset_to_bitstring(const std::vector<std::uint8_t>& bits) -> std::string
{
    auto bitstring = std::string {};
    bitstring.reserve(bits.size());

    for (auto bit : bits) {
        if (bit == 0) {
            bitstring.push_back('0');
        }
        else {
            bitstring.push_back('1');
        }
    }

    return bitstring;
}

inline auto state_index_to_bitstring(
    std::size_t i_state,
    std::size_t n_qubits,
    mqis::QuantumStateEndian input_endian
) -> std::string
{
    const auto dyn_bitset = state_index_to_dynamic_bitset(i_state, n_qubits, input_endian);
    return dynamic_bitset_to_bitstring(dyn_bitset);
}

inline auto dynamic_bitset_to_state_index(
    const std::vector<std::uint8_t>& dyn_bitset,
    mqis::QuantumStateEndian input_endian
) -> std::size_t
{
    const auto bitstring = dynamic_bitset_to_bitstring(dyn_bitset);
    return bitstring_to_state_index(bitstring, input_endian);
}

inline auto bitstring_to_dynamic_bitset(
    const std::string& bitstring
) -> std::vector<std::uint8_t>
{
    impl_mqis::check_bitstring_is_valid_nonmarginal_(bitstring);

    auto dyn_bitset = std::vector<std::uint8_t> {};
    dyn_bitset.reserve(bitstring.size());

    for (auto bitchar : bitstring) {
        if (bitchar == '0') {
            dyn_bitset.push_back(0);
        }
        else {
            dyn_bitset.push_back(1);
        }
    }

    return dyn_bitset;
}

inline auto bitstring_to_state_index_little_endian(const std::string& bitstring) -> std::size_t
{
    return bitstring_to_state_index(bitstring, mqis::QuantumStateEndian::LITTLE);
}

inline auto bitstring_to_state_index_big_endian(const std::string& bitstring) -> std::size_t
{
    return bitstring_to_state_index(bitstring, mqis::QuantumStateEndian::BIG);
}

inline auto state_index_to_bitstring_little_endian(std::size_t i_state, std::size_t n_qubits) -> std::string
{
    return state_index_to_bitstring(i_state, n_qubits, mqis::QuantumStateEndian::LITTLE);
}

inline auto state_index_to_bitstring_big_endian(std::size_t i_state, std::size_t n_qubits) -> std::string
{
    return state_index_to_bitstring(i_state, n_qubits, mqis::QuantumStateEndian::BIG);
}

inline auto state_index_to_dynamic_bitset_little_endian(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>
{
    return state_index_to_dynamic_bitset(i_state, n_qubits, mqis::QuantumStateEndian::LITTLE);
}

inline auto state_index_to_dynamic_bitset_big_endian(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>
{
    return state_index_to_dynamic_bitset(i_state, n_qubits, mqis::QuantumStateEndian::BIG);
}

inline auto dynamic_bitset_to_state_index_little_endian(const std::vector<std::uint8_t>& dyn_bitset) -> std::size_t
{
    return dynamic_bitset_to_state_index(dyn_bitset, mqis::QuantumStateEndian::LITTLE);
}

inline auto dynamic_bitset_to_state_index_big_endian(const std::vector<std::uint8_t>& dyn_bitset) -> std::size_t
{
    return dynamic_bitset_to_state_index(dyn_bitset, mqis::QuantumStateEndian::BIG);
}

}  // namespace impl_mqis

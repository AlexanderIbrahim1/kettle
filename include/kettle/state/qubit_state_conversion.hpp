#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "kettle/state/endian.hpp"


namespace ket
{

auto bitstring_to_state_index(
    const std::string& bitstring,
    ket::Endian input_endian
) -> std::size_t;

auto state_index_to_dynamic_bitset(
    std::size_t i_state,
    std::size_t n_qubits,
    ket::Endian input_endian
) -> std::vector<std::uint8_t>;

auto dynamic_bitset_to_bitstring(const std::vector<std::uint8_t>& bits) -> std::string;

auto state_index_to_bitstring(
    std::size_t i_state,
    std::size_t n_qubits,
    ket::Endian input_endian
) -> std::string;

auto dynamic_bitset_to_state_index(
    const std::vector<std::uint8_t>& dyn_bitset,
    ket::Endian input_endian
) -> std::size_t;

auto bitstring_to_dynamic_bitset(
    const std::string& bitstring
) -> std::vector<std::uint8_t>;

auto bitstring_to_state_index_little_endian(const std::string& bitstring) -> std::size_t;

auto bitstring_to_state_index_big_endian(const std::string& bitstring) -> std::size_t;

auto state_index_to_bitstring_little_endian(std::size_t i_state, std::size_t n_qubits) -> std::string;

auto state_index_to_bitstring_big_endian(std::size_t i_state, std::size_t n_qubits) -> std::string;

auto state_index_to_dynamic_bitset_little_endian(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>;

auto state_index_to_dynamic_bitset_big_endian(std::size_t i_state, std::size_t n_qubits) -> std::vector<std::uint8_t>;

auto dynamic_bitset_to_state_index_little_endian(const std::vector<std::uint8_t>& dyn_bitset) -> std::size_t;

auto dynamic_bitset_to_state_index_big_endian(const std::vector<std::uint8_t>& dyn_bitset) -> std::size_t;

auto binary_fraction_expansion(
    const std::string& bitstring,
    Endian endian = Endian::LITTLE
) -> double;

}  // namespace ket

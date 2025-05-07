#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "kettle/state/endian.hpp"

namespace ket::internal
{

enum class MarginalBitsSide : std::uint8_t
{
    LEFT,
    RIGHT
};

auto state_index_to_bitstring_marginal_(
    std::size_t i_state,
    const std::vector<std::uint8_t>& marginal_bitmask,
    ket::QuantumStateEndian input_endian
) -> std::string;

template <MarginalBitsSide Side>
auto are_all_marginal_bits_on_side_(const std::string& marginal_bitstring) -> bool;

}  // namespace ket::internal


namespace ket
{

auto rstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string;

auto lstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string;

}  // namespace ket

#pragma once

#include <cstddef>
#include <string>


namespace ket::internal
{

auto endian_flip_(std::size_t value, std::size_t n_relevant_bits) -> std::size_t;

auto is_valid_marginal_bitstring_(const std::string& bitstring) -> bool;

auto is_valid_nonmarginal_bitstring_(const std::string& bitstring) -> bool;

void check_bitstring_is_valid_nonmarginal_(const std::string& bitstring);

void check_bitstring_is_valid_marginal_(const std::string& bitstring);

}  // namespace ket::internal

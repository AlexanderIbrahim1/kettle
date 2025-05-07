#pragma once

#include <string>


namespace ket
{

auto rstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string;

auto lstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string;

}  // namespace ket

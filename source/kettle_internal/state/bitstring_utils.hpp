#pragma once

#include <string>


namespace ket::internal
{

auto is_valid_marginal_bitstring_(const std::string& bitstring) -> bool;

auto is_valid_nonmarginal_bitstring_(const std::string& bitstring) -> bool;

void check_bitstring_is_valid_nonmarginal_(const std::string& bitstring);

void check_bitstring_is_valid_marginal_(const std::string& bitstring);

}  // namespace ket::internal

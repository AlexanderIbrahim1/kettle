#include <algorithm>
#include <stdexcept>
#include <string>

#include "kettle_internal/common/utils_internal.hpp"
#include "kettle_internal/state/bitstring_utils.hpp"

namespace ket::internal
{

auto is_valid_marginal_bitstring_(const std::string& bitstring) -> bool
{
    const auto is_bitstring_char = [](char bitchar) {
        return bitchar == '0' || bitchar == '1' || bitchar == ket::internal::MARGINALIZED_QUBIT;
    };

    return std::all_of(bitstring.begin(), bitstring.end(), is_bitstring_char);
}

auto is_valid_nonmarginal_bitstring_(const std::string& bitstring) -> bool
{
    const auto is_nonmarginal_bitstring_char = [](char bitchar) {
        return bitchar == '0' || bitchar == '1';
    };

    return std::all_of(bitstring.begin(), bitstring.end(), is_nonmarginal_bitstring_char);
}

void check_bitstring_is_valid_nonmarginal_(const std::string& bitstring)
{
    if (!is_valid_nonmarginal_bitstring_(bitstring)) {
        throw std::runtime_error {"Received bitstring with inputs that aren't '0' or '1'\n"};
    }
}

void check_bitstring_is_valid_marginal_(const std::string& bitstring)
{
    if (!is_valid_marginal_bitstring_(bitstring)) {
        throw std::runtime_error {"Received bitstring with inputs that aren't '0', '1', or the marginal symbol.\n"};
    }
}

}  // namespace ket::internal

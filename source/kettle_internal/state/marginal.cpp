#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "kettle/state/endian.hpp"
#include "kettle/state/qubit_state_conversion.hpp"
#include "kettle/state/marginal.hpp"

#include "kettle_internal/common/utils_internal.hpp"

namespace ket::internal
{

auto state_index_to_bitstring_marginal_(
    std::size_t i_state,
    const std::vector<std::uint8_t>& marginal_bitmask,
    ket::QuantumStateEndian input_endian
) -> std::string
{
    const auto n_qubits = marginal_bitmask.size();
    const auto dyn_bitset = ket::state_index_to_dynamic_bitset(i_state, n_qubits, input_endian);

    auto bitstring = std::string {};
    bitstring.reserve(dyn_bitset.size());
    for (std::size_t i_bit {0}; i_bit < dyn_bitset.size(); ++i_bit) {
        if (marginal_bitmask[i_bit] == 1) {
            bitstring.push_back(ket::internal::MARGINALIZED_QUBIT);
        }
        else if (dyn_bitset[i_bit] == 0) {
            bitstring.push_back('0');
        }
        else {
            bitstring.push_back('1');
        }
    }

    return bitstring;
}

template <MarginalBitsSide Side>
auto are_all_marginal_bits_on_side_(const std::string& marginal_bitstring) -> bool
{
    if (marginal_bitstring.size() == 0) {
        return true;
    }

    auto flag_marginal_already_found = false;

    auto bitstring = marginal_bitstring;

    if constexpr (Side == MarginalBitsSide::LEFT) {
        std::ranges::reverse(bitstring);
    }

    for (auto bitchar : bitstring) {
        if (bitchar == ket::internal::MARGINALIZED_QUBIT) {
            flag_marginal_already_found = true;
            continue;
        }

        // at this point, a 0 or 1 has been found; if a marginal qubit was seen earlier, then we
        // know the bitstring is invalid
        if (flag_marginal_already_found) {
            return false;
        }
    }

    return true;
}
template auto are_all_marginal_bits_on_side_<MarginalBitsSide::LEFT>(const std::string& marginal_bitstring) -> bool;
template auto are_all_marginal_bits_on_side_<MarginalBitsSide::RIGHT>(const std::string& marginal_bitstring) -> bool;

}  // namespace ket::internal


namespace ket
{

auto rstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string
{
    namespace ki = ket::internal;

    using MBS = ki::MarginalBitsSide;
    if (!ki::are_all_marginal_bits_on_side_<MBS::RIGHT>(marginal_bitstring)) {
        auto err_msg = std::stringstream {};
        err_msg << "The bitstring '" << marginal_bitstring << "' cannot be rstripped of its marginal bits\n";
        throw std::runtime_error {err_msg.str()};
    }

    const auto it = std::ranges::find(marginal_bitstring, ket::internal::MARGINALIZED_QUBIT);

    return std::string {marginal_bitstring.begin(), it};
}

auto lstrip_marginal_bits(const std::string& marginal_bitstring) -> std::string
{
    namespace ki = ket::internal;

    using MBS = ki::MarginalBitsSide;
    if (!ki::are_all_marginal_bits_on_side_<MBS::LEFT>(marginal_bitstring)) {
        auto err_msg = std::stringstream {};
        err_msg << "The bitstring '" << marginal_bitstring << "' cannot be lstripped of its marginal bits\n";
        throw std::runtime_error {err_msg.str()};
    }

    const auto it = std::ranges::find_if_not(
        marginal_bitstring,
        [](auto bitchar) { return bitchar == ket::internal::MARGINALIZED_QUBIT; }
    );

    return std::string {it, marginal_bitstring.end()};
}

}  // namespace ket

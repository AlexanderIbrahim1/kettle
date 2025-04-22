#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>

/*
    The LinearBijectiveMap type is a thin wrapper around an array of key-value pairs, that performs
    an O(n) lookup for a given key.

    Although not asymptotically performant, this class has some advantages:
      - the number of items is typically <20, so it isn't too bad
      - I don't use it anywhere that performance is a big issue
      - I can avoid the hassle of creating a hash for whatever key I want to use
*/

namespace impl_ket
{

template <typename Key, typename Value, std::size_t Size>
struct LinearBijectiveMap
{
    std::array<std::pair<Key, Value>, Size> data_;

    [[nodiscard]]
    auto at(const Key& key) const -> Value
    {
        const auto is_item = [&key](const auto& item) { return item.first == key; };
        const auto it = std::ranges::find_if(data_, is_item);

        if (it != std::end(data_)) {
            return it->second;
        } else {
            throw std::range_error("Key not found in LinearBijectiveMap\n");
        }
    }

    [[nodiscard]]
    auto at_reverse(const Value& value) const -> Key
    {
        const auto is_item = [&value](const auto& item) { return item.second == value; };
        const auto it = std::ranges::find_if(data_, is_item);

        if (it != std::end(data_)) {
            return it->first;
        } else {
            throw std::range_error("Value not found in LinearBijectiveMap\n");
        }
    }
};

}  // namespace impl_ket

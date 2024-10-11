#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mini-qiskit/gate_pair_generator.hpp"

using BasisIndexMap = std::unordered_map<const char*, std::size_t>;
using IndexPair = std::pair<std::size_t, std::size_t>;

auto get_generated_index_pairs(mqis::SingleQubitGatePairGenerator& generator) -> std::vector<IndexPair>
{
    auto index_pairs = std::vector<IndexPair> {};
    for (std::size_t i {0}; i < generator.size(); ++i) {
        const auto [state0_index, state1_index] = generator.next();

        index_pairs.push_back({state0_index, state1_index});
    }

    return index_pairs;
}

TEST_CASE("SingleQubitGatePairGenerator with two qubits")
{
    const auto n_qubits = std::size_t {2};

    // with two qubits, the states are 00, 10, 01, 11
    const auto coeff_map = BasisIndexMap {
        {"00", 0},
        {"10", 1},
        {"01", 2},
        {"11", 3},
    };

    SECTION("loop over qubit at index 0")
    {
        auto generator = mqis::SingleQubitGatePairGenerator {0, n_qubits};

        REQUIRE(generator.size() == impl_mqis::pow_2_int(n_qubits - 1));

        auto expected = std::vector<IndexPair> {
            IndexPair {coeff_map.at("00"), coeff_map.at("10")},
             IndexPair {coeff_map.at("01"), coeff_map.at("11")}
        };

        auto actual = get_generated_index_pairs(generator);

        // there isn't really a guaranteed ordered, AFAIK
        std::sort(expected.begin(), expected.end());
        std::sort(actual.begin(), actual.end());

        REQUIRE(expected == actual);
    }

    SECTION("loop over qubit at index 1")
    {
        auto generator = mqis::SingleQubitGatePairGenerator {1, n_qubits};

        REQUIRE(generator.size() == impl_mqis::pow_2_int(n_qubits - 1));

        auto expected = std::vector<IndexPair> {
            IndexPair {coeff_map.at("00"), coeff_map.at("01")},
             IndexPair {coeff_map.at("10"), coeff_map.at("11")}
        };

        auto actual = get_generated_index_pairs(generator);

        // there isn't really a guaranteed ordered, AFAIK
        std::sort(expected.begin(), expected.end());
        std::sort(actual.begin(), actual.end());

        REQUIRE(expected == actual);
    }
}

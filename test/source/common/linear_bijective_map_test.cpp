#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "kettle_internal/common/linear_bijective_map.hpp"


TEST_CASE("LinearBijectiveMap")
{
    const auto map = ket::internal::LinearBijectiveMap<int, char, 10> {
        std::pair {1, 'a'},
        std::pair {2, 'b'},
        std::pair {5, 'c'}
    };

    SECTION("forward lookup")
    {
        REQUIRE(map.at(1) == 'a');
        REQUIRE(map.at(2) == 'b');
        REQUIRE(map.at(5) == 'c');
    }

    SECTION("reverse lookup")
    {
        REQUIRE(map.at_reverse('a') == 1);
        REQUIRE(map.at_reverse('b') == 2);
        REQUIRE(map.at_reverse('c') == 5);
    }

    SECTION("throws upon failed forward lookup")
    {
        REQUIRE_THROWS_AS(map.at(10), std::runtime_error);
    }

    SECTION("throws upon failed reverse lookup")
    {
        REQUIRE_THROWS_AS(map.at('d'), std::runtime_error);
    }
}

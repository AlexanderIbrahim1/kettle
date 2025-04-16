#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <mini-qiskit/common/arange.hpp>


TEST_CASE("arange() with one argument")
{
    SECTION("zero argument")
    {
        REQUIRE_THAT(mqis::arange(0), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("negative argument")
    {
        REQUIRE_THAT(mqis::arange(-2), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("positive argument : 1")
    {
        REQUIRE_THAT(mqis::arange(1), Catch::Matchers::Equals(std::vector<int> {0}));
    }

    SECTION("positive argument : 5")
    {
        REQUIRE_THAT(mqis::arange(5), Catch::Matchers::Equals(std::vector<int> {0, 1, 2, 3, 4}));
    }
}

TEST_CASE("arange() with two arguments")
{
    SECTION("right == left")
    {
        REQUIRE_THAT(mqis::arange(3, 3), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("right < left")
    {
        REQUIRE_THAT(mqis::arange(3, 1), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("right > left; both positive")
    {
        REQUIRE_THAT(mqis::arange(1, 5), Catch::Matchers::Equals(std::vector<int> {1, 2, 3, 4}));
    }

    SECTION("right > left; left negative, right positive")
    {
        REQUIRE_THAT(mqis::arange(-2, 3), Catch::Matchers::Equals(std::vector<int> {-2, -1, 0, 1, 2}));
    }

    SECTION("right > left; left negative, right negative")
    {
        REQUIRE_THAT(mqis::arange(-5, -1), Catch::Matchers::Equals(std::vector<int> {-5, -4, -3, -2}));
    }
}

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <kettle/common/arange.hpp>


TEST_CASE("arange() with one argument")
{
    SECTION("zero argument")
    {
        REQUIRE_THAT(ket::arange(0), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("negative argument")
    {
        REQUIRE_THAT(ket::arange(-2), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("positive argument : 1")
    {
        REQUIRE_THAT(ket::arange(1), Catch::Matchers::Equals(std::vector<int> {0}));
    }

    SECTION("positive argument : 5")
    {
        REQUIRE_THAT(ket::arange(5), Catch::Matchers::Equals(std::vector<int> {0, 1, 2, 3, 4}));
    }
}

TEST_CASE("arange() with two arguments")
{
    SECTION("right == left")
    {
        REQUIRE_THAT(ket::arange(3, 3), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("right < left")
    {
        REQUIRE_THAT(ket::arange(3, 1), Catch::Matchers::Equals(std::vector<int> {}));
    }

    SECTION("right > left; both positive")
    {
        REQUIRE_THAT(ket::arange(1, 5), Catch::Matchers::Equals(std::vector<int> {1, 2, 3, 4}));
    }

    SECTION("right > left; left negative, right positive")
    {
        REQUIRE_THAT(ket::arange(-2, 3), Catch::Matchers::Equals(std::vector<int> {-2, -1, 0, 1, 2}));
    }

    SECTION("right > left; left negative, right negative")
    {
        REQUIRE_THAT(ket::arange(-5, -1), Catch::Matchers::Equals(std::vector<int> {-5, -4, -3, -2}));
    }
}

TEST_CASE("arange() with three arguments")
{
    SECTION("with (signed) int")
    {
        struct TestCase
        {
            int left;
            int right;
            int step;
            std::vector<int> expected;
        };

        const auto testcase = GENERATE(
            TestCase(1, 2, 2, {1}),
            TestCase(1, 3, 2, {1}),
            TestCase(1, 4, 2, {1, 3}),
            TestCase(1, 5, 2, {1, 3}),
            TestCase(1, 6, 2, {1, 3, 5}),
            TestCase(1, 3, 1, {1, 2}),
            TestCase(1, 10, 4, {1, 5, 9}),
            TestCase(0, 10, 4, {0, 4, 8}),
            TestCase(-5, 5, 3, {-5, -2, 1, 4}),
            TestCase(5, 1, -2, {5, 3}),
            TestCase(5, -5, -2, {5, 3, 1, -1, -3}),
            TestCase(3, 3, 1, {}),
            TestCase(3, 3, 2, {}),
            TestCase(3, 3, -1, {}),
            TestCase(3, 3, -2, {}),
            TestCase(5, 1, 2, {}),
            TestCase(1, 5, -2, {})
        );

        const auto actual = ket::arange(testcase.left, testcase.right, testcase.step);

        REQUIRE_THAT(actual, Catch::Matchers::Equals(testcase.expected));
    }

    SECTION("with std::size_t")
    {
        struct TestCase
        {
            std::size_t left;
            std::size_t right;
            std::int64_t step;
            std::vector<std::size_t> expected;
        };

        const auto testcase = GENERATE(
            TestCase(1, 2, 2, {1}),
            TestCase(1, 3, 2, {1}),
            TestCase(1, 4, 2, {1, 3}),
            TestCase(1, 5, 2, {1, 3}),
            TestCase(1, 6, 2, {1, 3, 5}),
            TestCase(1, 3, 1, {1, 2}),
            TestCase(1, 10, 4, {1, 5, 9}),
            TestCase(0, 10, 4, {0, 4, 8}),
            TestCase(5, 1, -2, {5, 3}),
            TestCase(3, 3, 1, {}),
            TestCase(3, 3, 2, {}),
            TestCase(3, 3, -1, {}),
            TestCase(3, 3, -2, {}),
            TestCase(5, 1, 2, {}),
            TestCase(1, 5, -2, {})
        );

        const auto actual = ket::arange(testcase.left, testcase.right, testcase.step);

        REQUIRE_THAT(actual, Catch::Matchers::Equals(testcase.expected));
    }
}

TEST_CASE("revarange()")
{
    SECTION("one argument")
    {
        auto output = ket::revarange(5);
        REQUIRE_THAT(output, Catch::Matchers::Equals(std::vector<int> {4, 3, 2, 1, 0}));
    }

    SECTION("three arguments")
    {
        auto output = ket::revarange(0, 11, 2);
        REQUIRE_THAT(output, Catch::Matchers::Equals(std::vector<int> {10, 8, 6, 4, 2, 0}));
    }
}

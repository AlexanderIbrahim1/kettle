#include <catch2/catch_test_macros.hpp>

#include "mini-qiskit/mini-qiskit.hpp"

TEST_CASE("Basic test for mini-qiskit")
{
    const auto result = name();

    REQUIRE(result == "mini-qiskit");
}

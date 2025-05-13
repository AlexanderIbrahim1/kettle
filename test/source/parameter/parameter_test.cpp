#include <algorithm>
#include <array>
#include <cstdint>
#include <random>

#include <catch2/catch_test_macros.hpp>

#include "kettle/parameter/parameter.hpp"
#include "kettle_internal/parameter/parameter_internal.hpp"


namespace kp = ket::param;
namespace kpi = ket::param::internal;


template <std::uint8_t Output>
struct RiggedUniformIntDistribution
{
public:
    using result_type = std::uint8_t;

    RiggedUniformIntDistribution([[maybe_unused]] std::uint8_t left, [[maybe_unused]] std::uint8_t right)
    {}

    auto operator()([[maybe_unused]] std::mt19937& prng) -> std::uint8_t
    {
        return Output;
    }
};


TEST_CASE("create parameter")
{
    const auto id = kpi::create_parameter_id();
    const auto param = kp::Parameter {"theta", id};

    REQUIRE(param.name() == "theta");
    REQUIRE(param.id() == id);
}


TEST_CASE("create_parameter_id_helper()")
{
    SECTION("returns all 0")
    {
        const auto id = kpi::create_parameter_id_helper<RiggedUniformIntDistribution<0>>();

        auto expected = kp::ParameterID {};
        std::ranges::fill(expected, 0);

        REQUIRE(id == expected);
    }

    SECTION("returns all 10")
    {
        const auto id = kpi::create_parameter_id_helper<RiggedUniformIntDistribution<10>>();

        auto expected = kp::ParameterID {};
        std::ranges::fill(expected, 10);

        REQUIRE(id == expected);
    }
}

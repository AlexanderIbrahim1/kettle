#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <nlopt.hpp>


TEST_CASE("basic 2D nlopt example")
{
    const auto objective_function = [](
        const std::vector<double> &x,
        [[maybe_unused]] std::vector<double> &grad,
        [[maybe_unused]] void *data
    ) -> double
    {
        const auto dx = x[0] - 1.0;
        const auto dy = x[1] + 3.0;

        return (dx * dx) + (dy * dy);
    };

    auto opt = nlopt::opt {nlopt::LN_NELDERMEAD, 2};
    opt.set_min_objective(objective_function, nullptr);

    const auto test_check_tolerance = double {1.0e-6};
    const auto iter_tolerance = test_check_tolerance / 10.0;

    opt.set_xtol_rel(iter_tolerance);

    auto parameters = std::vector<double> {0.0, 0.0};
    double value_at_minimum;  // NOLINT

    [[maybe_unused]]
    const auto result = opt.optimize(parameters, value_at_minimum);

    REQUIRE_THAT(value_at_minimum, Catch::Matchers::WithinAbs(0.0, test_check_tolerance));
    REQUIRE_THAT(parameters[0], Catch::Matchers::WithinAbs(1.0, test_check_tolerance));
    REQUIRE_THAT(parameters[1], Catch::Matchers::WithinAbs(-3.0, test_check_tolerance));
}

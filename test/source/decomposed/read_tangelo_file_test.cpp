#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <mini-qiskit/primitive_gate.hpp>
#include <mini-qiskit/decomposed/read_tangelo_file.hpp>


TEST_CASE("read_tangelo_file()")
{
    SECTION("single h gate")
    {
        auto stream = std::stringstream {
            "H         target : [4]   \n"
        };

        const auto actual = mqis::read_tangelo_file(stream);

        REQUIRE(actual.size() == 1);
        REQUIRE(actual[0].gate == mqis::Gate::H);

        const auto target_qubit = impl_mqis::unpack_h_gate(actual[0]);
        REQUIRE(target_qubit == 4);
    }
}

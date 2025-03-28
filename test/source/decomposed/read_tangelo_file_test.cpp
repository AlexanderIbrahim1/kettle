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

        const auto actual = mqis::read_tangelo_circuit(9, stream, 0);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 1);
        REQUIRE(actual[0].gate == mqis::Gate::H);

        const auto target_qubit = impl_mqis::unpack_h_gate(actual[0]);
        REQUIRE(target_qubit == 4);
    }

    SECTION("multiple gates")
    {
        auto stream = std::stringstream {
            "Circuit object. Size 339200                            \n"
            "                                                       \n"
            "H         target : [4]                                 \n"
            "RX        target : [5]   parameter : 1.5707963267948966\n"
            "CNOT      target : [4]   control : [2]                 \n"
            "RZ        target : [5]   parameter : 12.533816585267923\n"
        };

        const auto actual = mqis::read_tangelo_circuit(9, stream, 2);

        REQUIRE(std::distance(actual.begin(), actual.end()) == 4);

        SECTION("H gate")
        {
            REQUIRE(actual[0].gate == mqis::Gate::H);
            const auto target_qubit = impl_mqis::unpack_h_gate(actual[0]);
            REQUIRE(target_qubit == 4);
        }

        SECTION("RX gate")
        {
            REQUIRE(actual[1].gate == mqis::Gate::RX);
            const auto [angle, target_qubit] = impl_mqis::unpack_rx_gate(actual[1]);
            REQUIRE(target_qubit == 5);
            REQUIRE_THAT(angle, Catch::Matchers::WithinRel(1.5707963267948966));
        }

        SECTION("CX gate")
        {
            REQUIRE(actual[2].gate == mqis::Gate::CX);
            const auto [control_qubit, target_qubit] = impl_mqis::unpack_cx_gate(actual[2]);
            REQUIRE(target_qubit == 4);
            REQUIRE(control_qubit == 2);
        }

        SECTION("RZ gate")
        {
            REQUIRE(actual[3].gate == mqis::Gate::RZ);
            const auto [angle, target_qubit] = impl_mqis::unpack_rx_gate(actual[3]);
            REQUIRE(target_qubit == 5);
            REQUIRE_THAT(angle, Catch::Matchers::WithinRel(12.533816585267923));
        }
    }
}

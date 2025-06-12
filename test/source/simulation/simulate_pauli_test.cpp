#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "kettle/operator/pauli/sparse_pauli_string.hpp"
#include "kettle/simulation/simulate_pauli.hpp"
#include "kettle/state/statevector.hpp"


TEST_CASE("simulate pauli string")
{
    SECTION("empty pauli string")
    {
        auto pauli_string = ket::SparsePauliString {3};

        auto statevector = ket::Statevector {"000"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"000"};
        REQUIRE(ket::almost_eq(statevector, expected));
    }
    
    SECTION("single X gate")
    {
        auto pauli_string = ket::SparsePauliString {3};
        pauli_string.add(0, ket::PauliTerm::X);

        auto statevector = ket::Statevector {"000"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"100"};
        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("two X gates")
    {
        auto pauli_string = ket::SparsePauliString {3};
        pauli_string.add(0, ket::PauliTerm::X);
        pauli_string.add(2, ket::PauliTerm::X);

        auto statevector = ket::Statevector {"000"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"101"};
        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("single Y gate")
    {
        auto pauli_string = ket::SparsePauliString {3};
        pauli_string.add(1, ket::PauliTerm::Y);

        auto statevector = ket::Statevector {"000"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"010"};
        expected.at("010") *= std::complex<double> {0.0, 1.0};
        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("Z gate on 010")
    {
        auto pauli_string = ket::SparsePauliString {3};
        pauli_string.add(1, ket::PauliTerm::Z);

        auto statevector = ket::Statevector {"010"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"010"};
        expected.at("010") *= -1.0;
        REQUIRE(ket::almost_eq(statevector, expected));
    }

    SECTION("X and Y gate")
    {
        auto pauli_string = ket::SparsePauliString {3};
        pauli_string.add(0, ket::PauliTerm::X);
        pauli_string.add(1, ket::PauliTerm::Y);
        pauli_string.add(2, ket::PauliTerm::Z);

        auto statevector = ket::Statevector {"011"};
        ket::simulate(pauli_string, statevector);

        auto expected = ket::Statevector {"101"};
        expected.at("101") *= std::complex<double> {0.0, 1.0};
        REQUIRE(ket::almost_eq(statevector, expected));
    }
}

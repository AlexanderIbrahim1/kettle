#include <catch2/catch_test_macros.hpp>

#include "kettle/operator/pauli/sparse_pauli_string.hpp"


using PT = ket::PauliTerm;


TEST_CASE("SparsePauliString.set_phase()")
{
    auto pauli_string = ket::SparsePauliString {5};
    pauli_string.set_phase(ket::PauliPhase::MINUS_EYE);

    REQUIRE(pauli_string.phase() == ket::PauliPhase::MINUS_EYE);
}


TEST_CASE("SparsePauliString.contains_index()")
{
    auto pauli_string = ket::SparsePauliString {5};

    SECTION("one element")
    {
        SECTION("contains")
        {
            pauli_string.add(3, PT::X);
            REQUIRE(pauli_string.contains_index(3));
        }

        SECTION("does not contain")
        {
            pauli_string.add(3, PT::X);
            REQUIRE(!pauli_string.contains_index(1));
        }
    }

    SECTION("two elements")
    {
        SECTION("contains")
        {
            pauli_string.add(3, PT::X);
            pauli_string.add(1, PT::X);
            REQUIRE(pauli_string.contains_index(3));
            REQUIRE(pauli_string.contains_index(1));
        }

        SECTION("does not contain")
        {
            pauli_string.add(3, PT::X);
            pauli_string.add(1, PT::X);
            REQUIRE(!pauli_string.contains_index(2));
        }
    }

    SECTION("out of bounds returns std::nullopt")
    {
        REQUIRE(!pauli_string.contains_index(10));
    }
}


TEST_CASE("SparsePauliString.add()")
{
    auto pauli_string = ket::SparsePauliString {5};

    SECTION("add one element")
    {
        pauli_string.add(0, PT::X);
        REQUIRE(pauli_string.at(0) == PT::X);
        REQUIRE(pauli_string.size() == 1);
    }

    SECTION("add two elements")
    {
        SECTION("no conflict")
        {
            pauli_string.add(0, PT::X);
            pauli_string.add(2, PT::Y);
            REQUIRE(pauli_string.at(0) == PT::X);
            REQUIRE(pauli_string.at(2) == PT::Y);
            REQUIRE(pauli_string.size() == 2);
        }

        SECTION("conflict throws")
        {
            pauli_string.add(0, PT::X);
            REQUIRE_THROWS_AS(pauli_string.add(0, PT::Y), std::runtime_error);
        }
    }

    SECTION("add index out of range throws")
    {
        REQUIRE_THROWS_AS(pauli_string.add(5, PT::Y), std::runtime_error);
    }
}


TEST_CASE("SparsePauliString.overwrite()")
{
    auto pauli_string = ket::SparsePauliString {5};

    SECTION("overwrite with no conflict")
    {
        pauli_string.overwrite(0, PT::X);
        pauli_string.overwrite(2, PT::Y);
        REQUIRE(pauli_string.at(0) == PT::X);
        REQUIRE(pauli_string.at(2) == PT::Y);
        REQUIRE(pauli_string.size() == 2);
    }

    SECTION("overwrite with conflict")
    {
        pauli_string.overwrite(0, PT::X);
        REQUIRE(pauli_string.at(0) == PT::X);
        REQUIRE(pauli_string.size() == 1);

        pauli_string.overwrite(0, PT::Y);
        REQUIRE(pauli_string.at(0) == PT::Y);
        REQUIRE(pauli_string.size() == 1);
    }

    SECTION("overwrite index out of range throws")
    {
        REQUIRE_THROWS_AS(pauli_string.overwrite(5, PT::Y), std::runtime_error);
    }
}


TEST_CASE("SparsePauliString.remove()")
{
    auto pauli_string = ket::SparsePauliString {5};

    SECTION("remove nonexisting without throw")
    {
        REQUIRE_NOTHROW(pauli_string.remove(0));
    }

    SECTION("remove existing")
    {
        SECTION("one element")
        {
            pauli_string.add(3, PT::X);
            REQUIRE(pauli_string.size() == 1);
            pauli_string.remove(3);
            REQUIRE(pauli_string.size() == 0);
        }

        SECTION("two elements")
        {
            pauli_string.add(3, PT::X);
            pauli_string.add(2, PT::Z);

            REQUIRE(pauli_string.contains_index(2));
            REQUIRE(pauli_string.contains_index(3));
            REQUIRE(pauli_string.size() == 2);

            pauli_string.remove(3);

            REQUIRE(pauli_string.contains_index(2));
            REQUIRE(pauli_string.size() == 1);

            pauli_string.remove(2);
            REQUIRE(pauli_string.size() == 0);
        }
    }
}


TEST_CASE("SparsePauliString construct from vector of terms")
{
    auto pauli_string = ket::SparsePauliString {{PT::X, PT::Y, PT::Z}};
    REQUIRE(pauli_string.size() == 3);
    REQUIRE(pauli_string.phase() == ket::PauliPhase::PLUS_ONE);
    REQUIRE(pauli_string.at(0) == PT::X);
    REQUIRE(pauli_string.at(1) == PT::Y);
    REQUIRE(pauli_string.at(2) == PT::Z);
}

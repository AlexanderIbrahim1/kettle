#include <cstddef>
#include <numeric>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <kettle/state/state.hpp>
#include <kettle/state/project_state.hpp>


static constexpr auto get_normalized_amplitudes(const std::vector<int>& values) -> std::vector<std::complex<double>>
{
    auto amplitudes = std::vector<std::complex<double>> {};
    amplitudes.reserve(values.size());
    
    for (auto value : values) {
        amplitudes.emplace_back(static_cast<double>(value), 0.0);
    }

    const auto sum_of_square_amplitudes = std::accumulate(
        amplitudes.begin(), amplitudes.end(), 0.0, [](double x, auto y) { return x + std::norm(y); }
    );

    const auto norm = std::sqrt(1.0 / sum_of_square_amplitudes);

    for (auto& amp : amplitudes) {
        amp *= norm;
    }

    return amplitudes;
}


TEST_CASE("is_index_set()")
{
    struct TestCase {
        std::size_t index;
        std::size_t value;
        bool expected;
    };

    const auto testcase = GENERATE(
        TestCase {0, 0, false},
        TestCase {0, 1, true},
        TestCase {0, 2, false},
        TestCase {0, 3, true},
        TestCase {1, 0, false},
        TestCase {1, 1, false},
        TestCase {1, 2, true},
        TestCase {1, 3, true}
    );

    const auto actual = impl_ket::is_index_set_(testcase.index, testcase.value);

    REQUIRE(actual == testcase.expected);
}


TEST_CASE("QubitStateChecker_")
{
    struct TestCase {
        std::size_t value;
        bool expected;
    };

    SECTION("2 qubits")
    {
        SECTION("qubit at state 0 is 0")
        {
            const auto checker = impl_ket::QubitStateChecker_ {{0}, {0}};

            const auto testcase = GENERATE(
                TestCase {0, true},   // 00
                TestCase {1, false},  // 10
                TestCase {2, true},   // 01
                TestCase {3, false}   // 11
            );

            REQUIRE(checker.all_indices_match(testcase.value) == testcase.expected);
        }

        SECTION("qubit at state 0 is 1")
        {
            const auto checker = impl_ket::QubitStateChecker_ {{0}, {1}};

            const auto testcase = GENERATE(
                TestCase {0, false},  // 00
                TestCase {1, true},   // 10
                TestCase {2, false},  // 01
                TestCase {3, true}    // 11
            );

            REQUIRE(checker.all_indices_match(testcase.value) == testcase.expected);
        }
    }

    SECTION("3 qubits")
    {
        SECTION("qubits at states {0, 2} are {0, 0}")
        {
            const auto checker = impl_ket::QubitStateChecker_ {{0, 2}, {0, 0}};

            const auto testcase = GENERATE(
                TestCase {0, true},   // 000
                TestCase {1, false},  // 100
                TestCase {2, true},   // 010
                TestCase {3, false},  // 110
                TestCase {4, false},  // 001
                TestCase {5, false},  // 101
                TestCase {6, false},  // 011
                TestCase {7, false}   // 111
            );

            REQUIRE(checker.all_indices_match(testcase.value) == testcase.expected);
        }

        SECTION("qubits at states {0, 1} are {1, 0}")
        {
            const auto checker = impl_ket::QubitStateChecker_ {{0, 1}, {1, 0}};

            const auto testcase = GENERATE(
                TestCase {0, false},  // 000
                TestCase {1, true},   // 100
                TestCase {2, false},  // 010
                TestCase {3, false},  // 110
                TestCase {4, false},  // 001
                TestCase {5, true},   // 101
                TestCase {6, false},  // 011
                TestCase {7, false}   // 111
            );

            REQUIRE(checker.all_indices_match(testcase.value) == testcase.expected);
        }
    }
}


TEST_CASE("measure_and_collapse()")
{
    SECTION("uniform linear superposition with 2 qubits")
    {
        const auto x_in = std::sqrt(1.0 / 4.0);
        const auto x_out = std::sqrt(1.0 / 2.0);

        const auto input = ket::QuantumState{ { {x_in, 0.0}, {x_in, 0.0}, {x_in, 0.0}, {x_in, 0.0} } };
        const auto expected = ket::QuantumState{ { {x_out, 0.0}, {x_out, 0.0} } };

        const auto actual = ket::project_statevector(input, {0}, {0});

        REQUIRE(ket::almost_eq(actual, expected));
    }

    SECTION("general state, 3 qubits")
    {
        // 000 -> 1
        // 100 -> 2
        // 010 -> 3
        // 110 -> 4
        // 001 -> 5
        // 101 -> 6
        // 011 -> 7
        // 111 -> 8
        auto initial_amplitudes = get_normalized_amplitudes({1, 2, 3, 4, 5, 6, 7, 8});
        const auto initial_state = ket::QuantumState {std::move(initial_amplitudes)};

        SECTION("collase: 0th qubit is 0")
        {
            // 000 -> 1
            // 010 -> 3
            // 001 -> 5
            // 011 -> 7
            auto expected_amplitudes = get_normalized_amplitudes({1, 3, 5, 7});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {0}, {0});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }

        SECTION("collase: 0th qubit is 1")
        {
            // 100 -> 2
            // 110 -> 4
            // 101 -> 6
            // 111 -> 8
            auto expected_amplitudes = get_normalized_amplitudes({2, 4, 6, 8});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {0}, {1});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }

        SECTION("collase: 1st qubit is 0")
        {
            // 000 -> 1
            // 100 -> 2
            // 001 -> 5
            // 101 -> 6
            auto expected_amplitudes = get_normalized_amplitudes({1, 2, 5, 6});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {1}, {0});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }

        SECTION("collase: 1st qubit is 1")
        {
            // 010 -> 3
            // 110 -> 4
            // 011 -> 7
            // 111 -> 8
            auto expected_amplitudes = get_normalized_amplitudes({3, 4, 7, 8});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {1}, {1});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }

        SECTION("collase: qubits {0, 1} collapse to {1, 0}")
        {
            // 100 -> 2
            // 101 -> 6
            auto expected_amplitudes = get_normalized_amplitudes({2, 6});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {0, 1}, {1, 0});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }
    }

    SECTION("general state, 4 qubits")
    {
        // 0000 -> 1
        // 1000 -> 2
        // 0100 -> 3
        // 1100 -> 4
        // 0010 -> 5
        // 1010 -> 6
        // 0110 -> 7
        // 1110 -> 8
        // 0001 -> 9
        // 1001 -> 10
        // 0101 -> 11
        // 1101 -> 12
        // 0011 -> 13
        // 1011 -> 14
        // 0111 -> 15
        // 1111 -> 16
        auto initial_amplitudes = get_normalized_amplitudes({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
        const auto initial_state = ket::QuantumState {std::move(initial_amplitudes)};

        SECTION("collase: qubits {1, 3} collapse to {1, 0}")
        {
            // 0100 -> 3
            // 1100 -> 4
            // 0110 -> 7
            // 1110 -> 8
            auto expected_amplitudes = get_normalized_amplitudes({3, 4, 7, 8});
            const auto expected_state = ket::QuantumState {std::move(expected_amplitudes)};

            const auto actual = ket::project_statevector(initial_state, {1, 3}, {1, 0});

            REQUIRE(ket::almost_eq(actual, expected_state));
        }
    }
}

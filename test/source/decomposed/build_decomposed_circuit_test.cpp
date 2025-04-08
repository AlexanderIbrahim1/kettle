#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <mini-qiskit/decomposed/decomposed_gate.hpp>
#include <mini-qiskit/decomposed/read_decomposition_file.hpp>
#include <mini-qiskit/decomposed/build_decomposed_circuit.hpp>
#include <mini-qiskit/circuit.hpp>
#include <mini-qiskit/simulation/simulate.hpp>
#include <mini-qiskit/state.hpp>


template <typename Container, typename Lambda>
auto comprehension(Container&& container, Lambda&& lambda)
{
    using T = decltype(lambda(*container.begin()));

    auto output = std::vector<T> {};

    const auto size = static_cast<std::size_t>(std::distance(container.begin(), container.end()));
    output.reserve(size);

    std::transform(container.begin(), container.end(), std::back_inserter(output), lambda);

    return output;
}


TEST_CASE("build sample circuit")
{
    /*
    Another program was used to decompose the quantum circuit described by the unitary operator
    {
        1.0,             0.0,             0.0,        0.0,
        1.0, exp(i 2 PI / 3), exp(i 4 PI / 3),        0.0,
        1.0, exp(i 4 PI / 3), exp(i 2 PI / 3),        0.0,
        0.0,             0.0,             0.0, -i sqrt(3),
    } / sqrt(3)

    The stream below contains the contents of the file produced from decomposing this unitary operator.
    The eigenvectors and eigenvalues were calculated using a separate program.
    */
    auto stream = std::stringstream {
        "NUMBER_OF_COMMANDS : 7                          \n"
        "ALLCONTROL : 0                                  \n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  1.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  1.0000000000000000e+00 -0.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        "  7.0710678118654757e-01  0.0000000000000000e+00\n"
        "  7.0710678118654746e-01 -8.6595605623549316e-17\n"
        " -7.0710678118654746e-01 -8.6595605623549316e-17\n"
        "  7.0710678118654757e-01 -0.0000000000000000e+00\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 0                                  \n"
        "  5.7735026918962584e-01  0.0000000000000000e+00\n"
        "  8.1649658092772603e-01 -9.9991992434789747e-17\n"
        " -8.1649658092772603e-01 -9.9991992434789747e-17\n"
        "  5.7735026918962584e-01 -0.0000000000000000e+00\n"
        "SINGLEGATE : 1                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  1.0000000000000000e+00  0.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        "ALLCONTROL : 1                                  \n"
        " -7.0710678118654757e-01  8.6595605623549341e-17\n"
        " -3.5731629454852966e-16 -7.0710678118654746e-01\n"
        "  3.5731629454852966e-16 -7.0710678118654746e-01\n"
        " -7.0710678118654757e-01 -8.6595605623549341e-17\n"
        "ALLCONTROL : 0                                  \n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
        " -5.7667474161826995e-16 -1.0000000000000000e+00\n"
        "  0.0000000000000000e+00 -1.0000000000000000e+00\n"
        "  0.0000000000000000e+00  0.0000000000000000e+00\n"
    };

    const auto gates = mqis::read_decomposed_gate_info(stream);
    const auto circuit = mqis::make_circuit_from_decomposed_gates(gates);

    SECTION("computational basis states")
    {
        struct TestCase
        {
            std::string input_bitstring;
            std::vector<std::complex<double>> output_amplitudes;
        };
        
        const auto sqrt3 = std::sqrt(3.0);
        const auto exp2 = std::exp(std::complex<double> {0.0, 2.0 * M_PI / 3.0});
        const auto exp4 = std::exp(std::complex<double> {0.0, 4.0 * M_PI / 3.0});

        const auto testcase = GENERATE_COPY(
            TestCase {"00", {1.0 / sqrt3,  1.0 / sqrt3, 1.0  / sqrt3,          0.0}},
            TestCase {"10", {1.0 / sqrt3, exp2 / sqrt3, exp4 / sqrt3,          0.0}},
            TestCase {"01", {1.0 / sqrt3, exp4 / sqrt3, exp2 / sqrt3,          0.0}},
            TestCase {"11", {        0.0,          0.0,          0.0,  {0.0, -1.0}}}
        );

        auto statevector = mqis::QuantumState {testcase.input_bitstring};
        mqis::simulate(circuit, statevector);

        const auto expected = mqis::QuantumState {testcase.output_amplitudes};

        REQUIRE(mqis::almost_eq(statevector, expected));
    }

    SECTION("eigenvectors")
    {
        struct TestCase
        {
            std::complex<double> eigenvalue;
            std::vector<std::complex<double>> eigenvector;
        };

        const auto testcase = GENERATE(
            TestCase {{ 1.0,  0.0}, { 0.8880738339771153,  0.3250575836718680,  0.3250575836718680,  0.0000000000000000}},
            TestCase {{-1.0,  0.0}, {-0.4597008433809830,  0.6279630301995544,  0.6279630301995544,  0.0000000000000000}},
            TestCase {{ 0.0,  1.0}, { 0.0000000000000000, -0.7071067811865475,  0.7071067811865476,  0.0000000000000000}},
            TestCase {{ 0.0, -1.0}, { 0.0000000000000000,  0.0000000000000000,  0.0000000000000000,  1.0000000000000000}}
        );

        const auto expected_amplitudes = comprehension(testcase.eigenvector, [&](auto amplitude) {return testcase.eigenvalue * amplitude; });

        auto statevector = mqis::QuantumState {testcase.eigenvector};
        const auto expected = mqis::QuantumState {expected_amplitudes};

        mqis::simulate(circuit, statevector);

        REQUIRE(mqis::almost_eq(statevector, expected));
    }
}

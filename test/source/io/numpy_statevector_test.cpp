#include <cstddef>
#include <numeric>
#include <sstream>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <mini-qiskit/state/state.hpp>
#include <mini-qiskit/io/numpy_statevector.hpp>


TEST_CASE("read_numpy_statevector()")
{
    auto stream = std::stringstream {
        "3                                                    \n"
        " (4.950737714883371443e-02+4.950737714883371443e-02j)\n"
        " (9.901475429766742886e-02+9.901475429766742886e-02j)\n"
        " (1.485221314465011433e-01+1.485221314465011433e-01j)\n"
        " (1.980295085953348577e-01+1.980295085953348577e-01j)\n"
        " (2.475368857441685444e-01+2.475368857441685444e-01j)\n"
        " (2.970442628930022866e-01+2.970442628930022866e-01j)\n"
        " (3.465516400418360288e-01+3.465516400418360288e-01j)\n"
        " (3.960590171906697154e-01+3.960590171906697154e-01j)\n"
    };

    const auto actual = mqis::read_numpy_statevector(stream);

    auto expected_amplitudes = []() {
        auto output = std::vector<std::complex<double>> {};
        output.reserve(8);

        for (std::size_t i {1}; i < 9; ++i) {
            const auto x = static_cast<double>(i) * 1.1;
            const auto value = std::complex<double> {x, x};
            output.push_back(value);
        }

        const auto norm_sq = std::accumulate(output.begin(), output.end(), 0.0, [](auto acc, auto elem) { return acc + std::norm(elem); });
        const auto norm = std::sqrt(norm_sq);

        std::transform(output.begin(), output.end(), output.begin(), [&](auto elem) { return elem / norm; });

        return output;
    }();

    const auto expected = mqis::QuantumState {std::move(expected_amplitudes)};

    REQUIRE(mqis::almost_eq(actual, expected));
}

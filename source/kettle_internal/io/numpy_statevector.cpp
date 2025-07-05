#include <complex>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "kettle/state/statevector.hpp"
#include "kettle/io/numpy_statevector.hpp"

namespace
{

auto read_complex_numpy_format_(std::istream& stream) -> std::complex<double>
{
    // reads in text that looks like (1.23456e005+5.43210e002j) into a std::complex<double> instance
    double real;  // NOLINT(cppcoreguidelines-init-variables)
    double imag;  // NOLINT(cppcoreguidelines-init-variables)
    char ch;  // NOLINT(cppcoreguidelines-init-variables)

    stream >> ch; // '('
    stream >> real;
    stream >> imag;
    stream >> ch; // 'j'
    stream >> ch; // ')'

    return {real, imag};
}

}  // namespace


namespace ket
{

auto read_numpy_statevector(
    std::istream& instream,
    Endian input_endian
) -> Statevector
{
    // the very first line contains the number of qubits
    const auto n_qubits = [&]() {
        std::size_t n_qubits_;  // NOLINT(cppcoreguidelines-init-variables)
        instream >> n_qubits_;

        return n_qubits_;
    }();

    const auto n_states = 1UL << n_qubits;

    auto amplitudes = std::vector<std::complex<double>> {};
    amplitudes.reserve(n_states);

    for (std::size_t i {0}; i < n_states; ++i) {
        amplitudes.push_back(read_complex_numpy_format_(instream));
    }

    return Statevector {std::move(amplitudes), input_endian};
}

auto read_numpy_statevector(
    const std::filesystem::path& filepath,
    Endian input_endian
) -> Statevector
{
    auto instream = std::ifstream {filepath};

    if (!instream.is_open()) {
        auto err_msg = std::stringstream {};
        err_msg << "ERROR: unable to open file for statevector: \n";
        err_msg << "'" << filepath << "'\n";

        throw std::ios::failure {err_msg.str()};
    }

    return read_numpy_statevector(instream, input_endian);
}

}  // namespace ket

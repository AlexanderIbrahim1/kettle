#pragma once

#include <complex>
#include <filesystem>
#include <iostream>
#include <vector>

#include <mini-qiskit/state/state.hpp>

namespace impl_mqis
{

inline auto read_complex_numpy_format(std::istream& stream) -> std::complex<double>
{
    // reads in text that looks like (1.23456e005+5.43210e002j) into a std::complex<double> instance
    double real;
    double imag;
    char ch;

    stream >> ch; // '('
    stream >> real;
    stream >> imag;
    stream >> ch; // 'j'
    stream >> ch; // ')'

    return {real, imag};
}

}  // namespace impl_mqis


namespace mqis
{

inline auto read_numpy_statevector(
    std::istream& instream,
    QuantumStateEndian input_endian = QuantumStateEndian::LITTLE
) -> QuantumState
{
    // the very first line contains the number of qubits
    const auto n_qubits = [&]() {
        std::size_t n_qubits_;
        instream >> n_qubits_;

        return n_qubits_;
    }();

    const auto n_states = 1ul << n_qubits;

    auto amplitudes = std::vector<std::complex<double>> {};
    amplitudes.reserve(n_states);

    for (std::size_t i {0}; i < n_states; ++i) {
        amplitudes.push_back(impl_mqis::read_complex_numpy_format(instream));
    }

    return QuantumState {std::move(amplitudes), input_endian};
}

inline auto read_numpy_statevector(
    const std::filesystem::path& filepath,
    QuantumStateEndian input_endian = QuantumStateEndian::LITTLE
) -> QuantumState
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

}  // namespace mqis

#pragma once

#include <complex>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <kettle/state/state.hpp>

namespace impl_ket
{

inline auto endian_to_string(ket::QuantumStateEndian endian) -> std::string
{
    using QSE = ket::QuantumStateEndian;

    if (endian == QSE::BIG) {
        return "BIG";
    }
    else if (endian == QSE::LITTLE) {
        return "LITTLE";
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error in `endian_to_string()`"};
    }
}

inline auto string_to_endian(const std::string& endian) -> ket::QuantumStateEndian
{
    using QSE = ket::QuantumStateEndian;

    if (endian == "BIG") {
        return QSE::BIG;
    }
    else if (endian == "LITTLE") {
        return QSE::LITTLE;
    }
    else {
        throw std::runtime_error {"UNREACHABLE: dev error in `string_to_endian()`"};
    }
}

inline auto format_complex(const std::complex<double>& value) -> std::string
{
    auto output = std::stringstream {};
    output << std::fixed << std::setprecision(14);

    // use a space to pad the negative sign
    if (value.real() >= 0.0) {
        output << ' ';
    }
    output << value.real();

    output << "   ";

    // use a space to pad the negative sign
    if (value.imag() >= 0.0) {
        output << ' ';
    }
    output << value.imag();


    return output.str();
}

}  // namespace impl_ket


namespace ket
{

inline void save_statevector(
    std::ostream& outstream,
    const QuantumState& state,
    QuantumStateEndian endian = QuantumStateEndian::LITTLE
)
{
    outstream << "ENDIANNESS: " << impl_ket::endian_to_string(endian) << '\n';
    outstream << "NUMBER OF STATES: " << state.n_states() << '\n';

    for (std::size_t i {0}; i < state.n_states(); ++i) {
        outstream << impl_ket::format_complex(state[i]) << '\n';
    }
}

inline void save_statevector(
    const std::filesystem::path& filepath,
    const QuantumState& state,
    QuantumStateEndian endian = QuantumStateEndian::LITTLE
)
{
    auto outstream = std::ofstream {filepath};

    if (!outstream.is_open()) {
        auto err_msg = std::stringstream {};
        err_msg << "ERROR: unable to open file to save statevector: \n";
        err_msg << "'" << filepath << "'\n";

        throw std::ios::failure {err_msg.str()};
    }

    save_statevector(outstream, state, endian);
}

inline auto load_statevector(std::istream& instream) -> QuantumState
{
    // the first line contains the endianness
    const auto endian = [&]() {
        std::string dummy;
        std::string endian_str;

        instream >> dummy;  // 'ENDIANNESS'
        instream >> endian_str;

        return impl_ket::string_to_endian(endian_str);
    }();

    // the next line contains the number of states
    const auto n_states = [&]() {
        std::string dummy;
        std::size_t n_states_;

        instream >> dummy;  // 'NUMBER'
        instream >> dummy;  // 'OF'
        instream >> dummy;  // 'STATES'
        instream >> n_states_;

        return n_states_;
    }();

    // the remaining lines contain the amplitudes
    auto amplitudes = std::vector<std::complex<double>> {};
    amplitudes.reserve(n_states);

    for (std::size_t i {0}; i < n_states; ++i) {
        double real;
        double imag;

        instream >> real;
        instream >> imag;

        amplitudes.emplace_back(real, imag);
    }

    return QuantumState {amplitudes, endian};
}

inline auto load_statevector(const std::filesystem::path& filepath) -> QuantumState
{
    auto instream = std::ifstream {filepath};

    if (!instream.is_open()) {
        auto err_msg = std::stringstream {};
        err_msg << "ERROR: unable to open file to load statevector: \n";
        err_msg << "'" << filepath << "'\n";

        throw std::ios::failure {err_msg.str()};
    }

    return load_statevector(instream);
}

}  // namespace ket

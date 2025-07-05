#include <complex>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "kettle/common/mathtools.hpp"
#include "kettle/state/statevector.hpp"
#include "kettle/io/statevector.hpp"

namespace
{

auto endian_to_string_(ket::Endian endian) -> std::string
{
    using QSE = ket::Endian;

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

auto string_to_endian_(const std::string& endian) -> ket::Endian
{
    using QSE = ket::Endian;

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

auto format_complex_(const std::complex<double>& value) -> std::string
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

}  // namespace


namespace ket
{

void save_statevector(
    std::ostream& outstream,
    const Statevector& state,
    Endian endian
)
{
    using QSE = Endian;

    outstream << "ENDIANNESS: " << endian_to_string_(endian) << '\n';
    outstream << "NUMBER OF STATES: " << state.n_states() << '\n';

    for (std::size_t i {0}; i < state.n_states(); ++i) {
        if (endian == QSE::LITTLE) {
            outstream << format_complex_(state[i]) << '\n';
        } else {
            outstream << format_complex_(state[ket::endian_flip(i, state.n_qubits())]) << '\n';
        }
    }
}

void save_statevector(
    const std::filesystem::path& filepath,
    const Statevector& state,
    Endian endian
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

auto load_statevector(std::istream& instream) -> Statevector
{
    // the first line contains the endianness
    const auto endian = [&]() {
        std::string dummy;
        std::string endian_str;

        instream >> dummy;  // 'ENDIANNESS'
        instream >> endian_str;

        return string_to_endian_(endian_str);
    }();

    // the next line contains the number of states
    const auto n_states = [&]() {
        std::string dummy;
        std::size_t n_states_;  // NOLINT(cppcoreguidelines-init-variables)

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
        double real;  // NOLINT(cppcoreguidelines-init-variables)
        double imag;  // NOLINT(cppcoreguidelines-init-variables)

        instream >> real;
        instream >> imag;

        amplitudes.emplace_back(real, imag);
    }

    return Statevector {amplitudes, endian};
}

auto load_statevector(const std::filesystem::path& filepath) -> Statevector
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

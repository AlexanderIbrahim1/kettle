#pragma once

#include <array>
#include <complex>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "mini-qiskit/common/matrix2x2.hpp"

namespace mqis
{

enum class DecomposedGateState {
    SINGLEGATE = 0,
    ALLCONTROL = 1
};

struct DecomposedGateInfo
{
    DecomposedGateState state;
    std::size_t qubit_index;
    Matrix2X2 matrix;
};

}  // namespace mqis

namespace impl_mqis
{

auto read_number_of_gates_(std::istream& stream) -> std::size_t
{
    // the first line of the decomposition file is `NUMBER_OF_COMMANDS : <number>`, and
    // we want to extract <number>
    auto sstream = [&]() {
        std::string line;
        std::getline(stream, line);
        return std::istringstream {line};
    }();

    std::string dummy;
    std::size_t n_gates;

    sstream >> dummy >> dummy >> n_gates;

    return n_gates;
}

auto read_gate_state_and_qubit_index_(std::istream& stream) -> std::tuple<mqis::DecomposedGateState, std::size_t>
{
    auto sstream = [&]() {
        std::string line;
        std::getline(stream, line);
        return std::istringstream {line};
    }();

    std::string state_str;
    std::string dummy;
    std::size_t qubit_index;
    
    sstream >> state_str >> dummy >> qubit_index;

    auto state = [&]() {
        if (state_str == "ALLCONTROL") {
            return mqis::DecomposedGateState::ALLCONTROL;
        } else if (state_str == "SINGLEGATE") {
            return mqis::DecomposedGateState::SINGLEGATE;
        } else {
            throw std::runtime_error("Unknown control state.");
        }
    }();

    return {state, qubit_index};
}

auto read_unitary_matrix_(std::istream& stream) -> mqis::Matrix2X2
{
    auto values = std::array<std::complex<double>, 4> {};

    double real;
    double imag;

    for (std::size_t i {0}; i < 4; ++i) {
        auto sstream = [&]() {
            std::string line;
            std::getline(stream, line);
            return std::istringstream {line};
        }();

        sstream >> real >> imag;
        values[i] = {real, imag};
    }

    return {values[0], values[1], values[2], values[3]};
}

}  // namespace impl_mqis

namespace mqis
{

auto read_decomposed_gate_info(std::istream& stream) -> std::vector<DecomposedGateInfo> {
    const auto n_gates = impl_mqis::read_number_of_gates_(stream);

    auto gates = std::vector<DecomposedGateInfo> {};
    gates.reserve(n_gates);

    // Read each block of 5 lines to create DecomposedGateInfo instances
    for (std::size_t i = 0; i < n_gates; ++i) {
        const auto [control_state, qubit_index] = impl_mqis::read_gate_state_and_qubit_index_(stream);
        auto matrix = impl_mqis::read_unitary_matrix_(stream);

        gates.emplace_back(control_state, qubit_index, std::move(matrix));
    }

    return gates;
}

}  // namespace mqis

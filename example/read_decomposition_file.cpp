#include <filesystem>
#include <fstream>
#include <iostream>

#include "mini-qiskit/decomposed/read_decomposition_file.hpp"

auto main() -> int
{
    const auto filename = std::string {"decomposition_example.txt"};
    const auto filepath = std::filesystem::path {"example"} / filename;

    auto instream = std::ifstream {filepath};
    if (!instream.is_open()) {
        throw std::ios::failure {"Failed to open the file."};
    }

    const auto gates = mqis::read_decomposed_gate_info(instream);

    for (const auto& gate : gates) {
        std::cout << "STATE: " << static_cast<int>(gate.state) << '\n';
        std::cout << "QUBIT: " << gate.qubit_index << '\n';
        std::cout << "(" << gate.matrix.elem00.real() << ", " << gate.matrix.elem00.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem01.real() << ", " << gate.matrix.elem01.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem10.real() << ", " << gate.matrix.elem10.imag() << ")\n";
        std::cout << "(" << gate.matrix.elem11.real() << ", " << gate.matrix.elem11.imag() << ")\n";
    }

    return 0;
}

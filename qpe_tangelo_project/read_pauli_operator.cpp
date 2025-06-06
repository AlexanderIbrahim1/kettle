#include <sstream>
#include <string>
#include <unordered_map>

#include <kettle/kettle.hpp>

/*
    This file contains the `read_pauli_operator()` function, which takes an output file
    from the Python qpe_dipolar_planar_rotor project, and reads it into a `PauliOperator`
    instance.
*/

const auto MAP_PAULI_GATE_STRING_TO_TERM = std::unordered_map<char, ket::PauliTerm> {
    {'X', ket::PauliTerm::X},
    {'Y', ket::PauliTerm::Y},
    {'Z', ket::PauliTerm::Z},
};


auto read_pauli_operator(std::istream& instream, std::size_t n_qubits) -> ket::PauliOperator
{
    auto pauli_op = ket::PauliOperator {n_qubits};

    char ch;  // NOLINT
    std::string line;  // NOLINT

    while (std::getline(instream, line)) {
        auto linestream = std::stringstream {line};

        double real;  // NOLINT
        double imag;  // NOLINT
        linestream >> real;
        linestream >> imag;

        const auto coeff = std::complex<double> {real, imag};

        linestream >> ch; // :

        auto pauli_string = ket::SparsePauliString {n_qubits};

        while (linestream >> ch) {
            if (ch == '(') {
                std::size_t qubit_index;  // NOLINT
                char pauli_gate;  // NOLINT

                linestream >> qubit_index;
                linestream >> ch;  // ,
                linestream >> pauli_gate;
                linestream >> ch;  // )

                const auto pauli_term = MAP_PAULI_GATE_STRING_TO_TERM.at(pauli_gate);
                pauli_string.add(qubit_index, pauli_term);
            }
        }

        pauli_op.add(coeff, std::move(pauli_string));
    }

    return pauli_op;
}

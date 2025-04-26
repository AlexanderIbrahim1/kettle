#pragma once

#include <complex>
#include <iostream>
#include <sstream>
#include <string>

#include "kettle/state/state.hpp"


namespace impl_ket
{

inline void print_state_(const ket::QuantumState& state)
{
    // for the time being, fix this as being little-endian
    const auto endian = ket::QuantumStateEndian::LITTLE;

    for (std::size_t i {0}; i < state.n_states(); ++i) {
        const auto bitstring = ket::state_index_to_bitstring(i, state.n_qubits(), endian);
        std::cout << bitstring << " : (" << state[i].real() << ", " << state[i].imag() << ")\n";
    }
}

inline auto ae_err_msg_diff_number_of_qubits_(std::size_t n_left_qubits, std::size_t n_right_qubits)
-> std::string
{
    auto err_msg = std::stringstream {};
    err_msg << "FALSE: ALMOST_EQ_WITH_PRINT()\n";
    err_msg << "REASON: different number of qubits in the states\n";
    err_msg << "left state: " << n_left_qubits << '\n';
    err_msg << "right state: " << n_right_qubits << '\n';

    return err_msg.str();
}

inline auto ae_err_msg_diff_states_(
    const ket::QuantumState& left,
    const ket::QuantumState& right
) -> std::string
{
    auto err_msg = std::stringstream {};
    err_msg << "FALSE: ALMOST_EQ_WITH_PRINT()\n";
    err_msg << "REASON: different states\n";

    err_msg << "LEFT STATE:\n";
    print_state_(left);

    err_msg << "RIGHT STATE:\n";
    print_state_(right);

    return err_msg.str();
}

}  // namespace impl_ket


namespace ket
{

inline void print_state(const QuantumState& state)
{
    impl_ket::print_state_(state);
}

enum PrintAlmostEq : std::uint8_t
{
    PRINT,
    NOPRINT
};

constexpr auto almost_eq_with_print(
    const QuantumState& left,
    const QuantumState& right,
    PrintAlmostEq print_state = PrintAlmostEq::PRINT,
    double tolerance_sq = impl_ket::COMPLEX_ALMOST_EQ_TOLERANCE_SQ
) noexcept -> bool
{
    using PAE = PrintAlmostEq;

    if (left.n_qubits() != right.n_qubits()) {
        if (print_state == PAE::PRINT) {
            std::cout << impl_ket::ae_err_msg_diff_number_of_qubits_(left.n_qubits(), right.n_qubits());
        }
        return false;
    }

    for (std::size_t i {0}; i < left.n_states(); ++i) {
        if (!almost_eq(left[i], right[i], tolerance_sq)) {
            if (print_state == PAE::PRINT) {
                std::cout << impl_ket::ae_err_msg_diff_states_(left, right);
            }
            return false;
        }
    }

    return true;
}


}  // namespace ket

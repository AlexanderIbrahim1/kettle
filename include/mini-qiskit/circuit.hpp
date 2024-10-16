#pragma once

#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "mini-qiskit/gate.hpp"

namespace mqis
{

class QuantumCircuit
{
public:
    explicit QuantumCircuit(std::size_t n_qubits, std::size_t n_bits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_bits}
    {}

    explicit QuantumCircuit(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_qubits}
    {}

    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    constexpr auto n_bits() const noexcept -> std::size_t
    {
        return n_bits_;
    }

    constexpr auto operator[](std::size_t index) const noexcept -> const GateInfo&
    {
        return gates_[index];
    }

    constexpr auto begin() const noexcept
    {
        return std::begin(gates_);
    }

    constexpr auto end() const noexcept
    {
        return std::end(gates_);
    }

    void add_x_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "X");
        gates_.emplace_back(create_x_gate(qubit_index));
    }

    void add_rx_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RX");
        gates_.emplace_back(create_rx_gate(theta, qubit_index));
    }

    void add_h_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "H");
        gates_.emplace_back(create_h_gate(qubit_index));
    }

    void add_cx_gate(std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CX");
        check_qubit_range_(target_index, "target qubit", "CX");
        gates_.emplace_back(create_cx_gate(source_index, target_index));
    }

    void add_crx_gate(double theta, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CRX");
        check_qubit_range_(target_index, "target qubit", "CRX");
        gates_.emplace_back(create_crx_gate(source_index, target_index, theta));
    }

    void add_m_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "M");

        // MicroQiskit only allows measurements where the bit index and qubit index are
        // the same, and I'll be following the same convention
        const auto bit_index = qubit_index;

        gates_.emplace_back(create_m_gate(qubit_index, bit_index));
    }

    void add_rz_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RZ");
        gates_.emplace_back(create_h_gate(qubit_index));
        gates_.emplace_back(create_rx_gate(theta, qubit_index));
        gates_.emplace_back(create_h_gate(qubit_index));
    }

    void add_ry_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RY");
        gates_.emplace_back(create_rx_gate(M_PI_2, qubit_index));
        add_rz_gate(theta, qubit_index);
        gates_.emplace_back(create_rx_gate(-M_PI_2, qubit_index));
    }

    void add_z_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Z");
        add_rz_gate(M_PI, qubit_index);
    }

    void add_y_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Y");
        add_z_gate(qubit_index);
        add_x_gate(qubit_index);
    }

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<GateInfo> gates_ {};

    void check_qubit_range_(std::size_t qubit_index, std::string_view qubit_name, std::string_view gate_name)
    {
        if (qubit_index >= n_qubits_) {
            auto err_msg = std::stringstream {};

            err_msg << "The index for the " << qubit_name;
            err_msg << " at which the '" << gate_name << "' gate is applied, is out of bounds.\n";
            err_msg << "n_qubits             = " << n_qubits_ << '\n';
            err_msg << "provided qubit index = " << qubit_index << '\n';

            throw std::runtime_error {err_msg.str()};
        }
    }

    void check_bit_range_(std::size_t bit_index)
    {
        if (bit_index >= n_bits_) {
            auto err_msg = std::stringstream {};

            err_msg << "The bit index at which the 'M' gate is applied, is out of bounds.\n";
            err_msg << "n_bits             = " << n_bits_ << '\n';
            err_msg << "provided bit index = " << bit_index << '\n';

            throw std::runtime_error {err_msg.str()};
        }
    }
};

}  // namespace mqis

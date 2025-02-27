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
    static constexpr auto MEASURED_FLAG = std::uint8_t {1};
    static constexpr auto UNMEASURED_FLAG = std::uint8_t {0};

    explicit QuantumCircuit(std::size_t n_qubits, std::size_t n_bits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_bits}
        , measure_bitmask_(n_qubits, UNMEASURED_FLAG)
    {}

    explicit QuantumCircuit(std::size_t n_qubits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_qubits}
        , measure_bitmask_(n_qubits, UNMEASURED_FLAG)
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
        check_previous_gate_is_not_measure_(qubit_index, "X");
        gates_.emplace_back(impl_mqis::create_x_gate(qubit_index));
    }

    void add_rx_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RX");
        check_previous_gate_is_not_measure_(qubit_index, "RX");
        gates_.emplace_back(impl_mqis::create_rx_gate(theta, qubit_index));
    }

    void add_h_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "H");
        check_previous_gate_is_not_measure_(qubit_index, "H");
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
    }

    void add_cx_gate(std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CX");
        check_qubit_range_(target_index, "target qubit", "CX");
        check_previous_gate_is_not_measure_(source_index, "CX");
        check_previous_gate_is_not_measure_(target_index, "CX");
        gates_.emplace_back(impl_mqis::create_cx_gate(source_index, target_index));
    }

    void add_crx_gate(double theta, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CRX");
        check_qubit_range_(target_index, "target qubit", "CRX");
        check_previous_gate_is_not_measure_(source_index, "CRX");
        check_previous_gate_is_not_measure_(target_index, "CRX");
        gates_.emplace_back(impl_mqis::create_crx_gate(source_index, target_index, theta));
    }

    void add_rz_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RZ");
        check_previous_gate_is_not_measure_(qubit_index, "RZ");
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
        gates_.emplace_back(impl_mqis::create_rx_gate(theta, qubit_index));
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
    }

    void add_ry_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RY");
        check_previous_gate_is_not_measure_(qubit_index, "RY");
        gates_.emplace_back(impl_mqis::create_rx_gate(M_PI_2, qubit_index));
        add_rz_gate(theta, qubit_index);
        gates_.emplace_back(impl_mqis::create_rx_gate(-M_PI_2, qubit_index));
    }

    void add_z_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Z");
        check_previous_gate_is_not_measure_(qubit_index, "Z");
        add_rz_gate(M_PI, qubit_index);
    }

    void add_y_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Y");
        check_previous_gate_is_not_measure_(qubit_index, "Y");
        add_z_gate(qubit_index);
        add_x_gate(qubit_index);
    }

    void add_m_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "M");

        if (measure_bitmask_[qubit_index] == MEASURED_FLAG) {
            throw std::runtime_error {""};
        }

        // MicroQiskit only allows measurements where the bit index and qubit index are
        // the same, and I'll be following the same convention
        const auto bit_index = qubit_index;

        gates_.emplace_back(impl_mqis::create_m_gate(qubit_index, bit_index));
        measure_bitmask_[qubit_index] = MEASURED_FLAG;
    }

    constexpr auto measure_bitmask() const noexcept -> const std::vector<std::uint8_t>&
    {
        return measure_bitmask_;
    }

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<std::uint8_t> measure_bitmask_;
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

    void check_previous_gate_is_not_measure_(std::size_t qubit_index, std::string_view gate_name)
    {
        if (gates_.size() == 0) {
            return;
        }

        if (gates_.back().gate == Gate::M) {
            auto err_msg = std::stringstream {};

            err_msg << "The current implementation only allows measurement gates at the very\n";
            err_msg << "end of the circuit. A non-measurement gate cannot be added after a\n";
            err_msg << "measurement gate.\n";
            err_msg << "Cannot add '" << gate_name << "' gate at qubit " << qubit_index << '\n';

            throw std::runtime_error {err_msg.str()};
        }
    }

    void check_gate_is_not_already_measure_(std::size_t qubit_index)
    {
        if (measure_bitmask_[qubit_index] == MEASURED_FLAG) {
            auto err_msg = std::stringstream {};

            err_msg << "Cannot measure qubit " << qubit_index << " twice.\n";

            throw std::runtime_error {err_msg.str()};
        }
    }
};

}  // namespace mqis

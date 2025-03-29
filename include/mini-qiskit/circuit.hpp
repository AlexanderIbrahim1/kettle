#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/gates/common_u_gates.hpp"
#include "mini-qiskit/primitive_gate.hpp"

namespace mqis
{

class QuantumCircuit
{
public:
    static constexpr auto MEASURED_FLAG = std::uint8_t {1};
    static constexpr auto UNMEASURED_FLAG = std::uint8_t {0};

    explicit constexpr QuantumCircuit(std::size_t n_qubits, std::size_t n_bits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_bits}
        , measure_bitmask_(n_qubits, UNMEASURED_FLAG)
    {}

    explicit constexpr QuantumCircuit(std::size_t n_qubits)
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

    void add_x_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "X");
        check_previous_gate_is_not_measure_(target_index, "X");
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::X>(target_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_x_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_x_gate(index);
        }
    }

    void add_rx_gate(double theta, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "RX");
        check_previous_gate_is_not_measure_(target_index, "RX");
        gates_.emplace_back(impl_mqis::create_one_target_one_angle_gate<Gate::RX>(theta, target_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_rx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rx_gate(pair.first, pair.second);
        }
    }

    void add_h_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "H");
        check_previous_gate_is_not_measure_(target_index, "H");
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::H>(target_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_h_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_h_gate(index);
        }
    }

    void add_cx_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CX");
        check_qubit_range_(target_index, "target qubit", "CX");
        check_previous_gate_is_not_measure_(control_index, "CX");
        check_previous_gate_is_not_measure_(target_index, "CX");
        gates_.emplace_back(impl_mqis::create_cx_gate(control_index, target_index));
    }

    template <
        impl_mqis::ContainerOfControlAndTargetQubitIndices Container =
            std::initializer_list<std::pair<std::size_t, std::size_t>>>
    void add_cx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_cx_gate(pair.first, pair.second);
        }
    }

    void add_cz_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CZ");
        check_qubit_range_(target_index, "target qubit", "CZ");
        check_previous_gate_is_not_measure_(control_index, "CZ");
        check_previous_gate_is_not_measure_(target_index, "CZ");
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::H>(target_index));
        gates_.emplace_back(impl_mqis::create_cx_gate(control_index, target_index));
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::H>(target_index));
    }

    template <
        impl_mqis::ContainerOfControlAndTargetQubitIndices Container =
            std::initializer_list<std::pair<std::size_t, std::size_t>>>
    void add_cz_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_cz_gate(pair.first, pair.second);
        }
    }

    void add_crx_gate(double theta, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CRX");
        check_qubit_range_(target_index, "target qubit", "CRX");
        check_previous_gate_is_not_measure_(control_index, "CRX");
        check_previous_gate_is_not_measure_(target_index, "CRX");
        gates_.emplace_back(impl_mqis::create_one_control_one_target_one_angle_gate<Gate::CRX>(control_index, target_index, theta));
    }

    template <
        impl_mqis::ContainerOfAnglesAndControlAndTargetQubitIndices Container =
            std::initializer_list<std::tuple<double, std::size_t, std::size_t>>>
    void add_crx_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_crx_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_cry_gate(double theta, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CRY");
        check_qubit_range_(target_index, "target qubit", "CRY");
        check_previous_gate_is_not_measure_(control_index, "CRY");
        check_previous_gate_is_not_measure_(target_index, "CRY");
        gates_.emplace_back(impl_mqis::create_one_control_one_target_one_angle_gate<Gate::CRY>(control_index, target_index, theta));
    }

    template <
        impl_mqis::ContainerOfAnglesAndControlAndTargetQubitIndices Container =
            std::initializer_list<std::tuple<double, std::size_t, std::size_t>>>
    void add_cry_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_cry_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_crz_gate(double theta, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CRZ");
        check_qubit_range_(target_index, "target qubit", "CRZ");
        check_previous_gate_is_not_measure_(control_index, "CRZ");
        check_previous_gate_is_not_measure_(target_index, "CRZ");
        gates_.emplace_back(impl_mqis::create_one_control_one_target_one_angle_gate<Gate::CRZ>(control_index, target_index, theta));
    }

    template <
        impl_mqis::ContainerOfAnglesAndControlAndTargetQubitIndices Container =
            std::initializer_list<std::tuple<double, std::size_t, std::size_t>>>
    void add_crz_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_crz_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_cp_gate(double theta, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CP");
        check_qubit_range_(target_index, "target qubit", "CP");
        check_previous_gate_is_not_measure_(control_index, "CP");
        check_previous_gate_is_not_measure_(target_index, "CP");
        gates_.emplace_back(impl_mqis::create_one_control_one_target_one_angle_gate<Gate::CP>(control_index, target_index, theta));
    }

    template <
        impl_mqis::ContainerOfAnglesAndControlAndTargetQubitIndices Container =
            std::initializer_list<std::tuple<double, std::size_t, std::size_t>>>
    void add_cp_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_cp_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_rz_gate(double theta, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "RZ");
        check_previous_gate_is_not_measure_(target_index, "RZ");
        gates_.emplace_back(impl_mqis::create_one_target_one_angle_gate<Gate::RZ>(theta, target_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_rz_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rz_gate(pair.first, pair.second);
        }
    }

    void add_p_gate(double theta, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "P");
        check_previous_gate_is_not_measure_(target_index, "P");
        gates_.emplace_back(impl_mqis::create_one_target_one_angle_gate<Gate::P>(theta, target_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_p_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_p_gate(pair.first, pair.second);
        }
    }

    void add_ry_gate(double theta, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "RY");
        check_previous_gate_is_not_measure_(target_index, "RY");
        gates_.emplace_back(impl_mqis::create_one_target_one_angle_gate<Gate::RY>(theta, target_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_ry_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_ry_gate(pair.first, pair.second);
        }
    }

    void add_z_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "Z");
        check_previous_gate_is_not_measure_(target_index, "Z");
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::Z>(target_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_z_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_z_gate(index);
        }
    }

    void add_y_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "Y");
        check_previous_gate_is_not_measure_(target_index, "Y");
        gates_.emplace_back(impl_mqis::create_one_target_gate<Gate::Y>(target_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_y_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_y_gate(index);
        }
    }

    void add_u_gate(const Matrix2X2& gate, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "U");
        check_previous_gate_is_not_measure_(target_index, "U");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_mqis::create_u_gate(target_index, gate_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_u_gate(const Matrix2X2& gate, const Container& indices)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto target_index : indices) {
            check_qubit_range_(target_index, "qubit", "U");
            check_previous_gate_is_not_measure_(target_index, "U");

            gates_.emplace_back(impl_mqis::create_u_gate(target_index, gate_index));
        }
    }

    void add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CU");
        check_qubit_range_(target_index, "target qubit", "CU");
        check_previous_gate_is_not_measure_(control_index, "CU");
        check_previous_gate_is_not_measure_(target_index, "CU");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_mqis::create_cu_gate(control_index, target_index, gate_index));
    }

    template <
        impl_mqis::ContainerOfControlAndTargetQubitIndices Container =
            std::initializer_list<std::pair<std::size_t, std::size_t>>>
    void add_cu_gate(const Matrix2X2& gate, const Container& tuples)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto tuple : tuples) {
            const auto control_index = std::get<0>(tuple);
            const auto target_index = std::get<1>(tuple);

            check_qubit_range_(control_index, "control qubit", "CU");
            check_qubit_range_(target_index, "target qubit", "CU");
            check_previous_gate_is_not_measure_(control_index, "CU");
            check_previous_gate_is_not_measure_(target_index, "CU");

            gates_.emplace_back(impl_mqis::create_cu_gate(control_index, target_index, gate_index));
        }
    }

    void add_m_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "M");

        if (measure_bitmask_[target_index] == MEASURED_FLAG) {
            throw std::runtime_error {""};
        }

        // MicroQiskit only allows measurements where the bit index and qubit index are
        // the same, and I'll be following the same convention
        const auto bit_index = target_index;

        gates_.emplace_back(impl_mqis::create_m_gate(target_index, bit_index));
        measure_bitmask_[target_index] = MEASURED_FLAG;
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_m_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_m_gate(index);
        }
    }

    constexpr auto measure_bitmask() const noexcept -> const std::vector<std::uint8_t>&
    {
        return measure_bitmask_;
    }

    constexpr auto unitary_gate(std::size_t matrix_index) const noexcept -> const Matrix2X2&
    {
        return unitary_gates_[matrix_index];
    }

    friend auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit;
    friend void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right);

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<std::uint8_t> measure_bitmask_;
    std::vector<GateInfo> gates_ {};
    std::vector<Matrix2X2> unitary_gates_ {};

    void check_qubit_range_(std::size_t target_index, std::string_view qubit_name, std::string_view gate_name)
    {
        if (target_index >= n_qubits_) {
            auto err_msg = std::stringstream {};

            err_msg << "The index for the " << qubit_name;
            err_msg << " at which the '" << gate_name << "' gate is applied, is out of bounds.\n";
            err_msg << "n_qubits             = " << n_qubits_ << '\n';
            err_msg << "provided qubit index = " << target_index << '\n';

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

    void check_previous_gate_is_not_measure_(std::size_t target_index, std::string_view gate_name)
    {
        if (gates_.size() == 0) {
            return;
        }

        if (gates_.back().gate == Gate::M) {
            auto err_msg = std::stringstream {};

            err_msg << "The current implementation only allows measurement gates at the very\n";
            err_msg << "end of the circuit. A non-measurement gate cannot be added after a\n";
            err_msg << "measurement gate.\n";
            err_msg << "Cannot add '" << gate_name << "' gate at qubit " << target_index << '\n';

            throw std::runtime_error {err_msg.str()};
        }
    }

    void check_gate_is_not_already_measure_(std::size_t target_index)
    {
        if (measure_bitmask_[target_index] == MEASURED_FLAG) {
            auto err_msg = std::stringstream {};

            err_msg << "Cannot measure qubit " << target_index << " twice.\n";

            throw std::runtime_error {err_msg.str()};
        }
    }
};

}  // namespace mqis

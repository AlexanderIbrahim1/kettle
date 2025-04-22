#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/common_u_gates.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/control_flow.hpp"

namespace ket
{

// TODO: use templates to replace a lot of duplicated code with these gate-creating member functions
class QuantumCircuit
{
public:
    explicit constexpr QuantumCircuit(std::size_t n_qubits, std::size_t n_bits)
        : n_qubits_ {n_qubits}
        , n_bits_ {n_bits}
    {}

    explicit constexpr QuantumCircuit(std::size_t n_qubits)
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

    void add_h_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "H");
        gates_.emplace_back(impl_ket::create_one_target_gate(Gate::H, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_h_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_h_gate(index);
        }
    }

    void add_x_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "X");
        gates_.emplace_back(impl_ket::create_one_target_gate(Gate::X, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_x_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_x_gate(index);
        }
    }

    void add_y_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "Y");
        gates_.emplace_back(impl_ket::create_one_target_gate(Gate::Y, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_y_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_y_gate(index);
        }
    }

    void add_z_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "Z");
        gates_.emplace_back(impl_ket::create_one_target_gate(Gate::Z, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_z_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_z_gate(index);
        }
    }

    void add_sx_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "SX");
        gates_.emplace_back(impl_ket::create_one_target_gate(Gate::SX, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_sx_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_sx_gate(index);
        }
    }

    void add_rx_gate(std::size_t target_index, double angle)
    {
        check_qubit_range_(target_index, "qubit", "RX");
        gates_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RX, angle, target_index));
    }

    template <impl_ket::QubitIndicesAndAngles Container = impl_ket::QubitIndicesAndAnglesIList>
    void add_rx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rx_gate(pair.first, pair.second);
        }
    }

    void add_ry_gate(std::size_t target_index, double angle)
    {
        check_qubit_range_(target_index, "qubit", "RY");
        gates_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RY, angle, target_index));
    }

    template <impl_ket::QubitIndicesAndAngles Container = impl_ket::QubitIndicesAndAnglesIList>
    void add_ry_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_ry_gate(pair.first, pair.second);
        }
    }

    void add_rz_gate(std::size_t target_index, double angle)
    {
        check_qubit_range_(target_index, "qubit", "RZ");
        gates_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RZ, angle, target_index));
    }

    template <impl_ket::QubitIndicesAndAngles Container = impl_ket::QubitIndicesAndAnglesIList>
    void add_rz_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rz_gate(pair.first, pair.second);
        }
    }

    void add_p_gate(std::size_t target_index, double angle)
    {
        check_qubit_range_(target_index, "qubit", "P");
        gates_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::P, angle, target_index));
    }

    template <impl_ket::QubitIndicesAndAngles Container = impl_ket::QubitIndicesAndAnglesIList>
    void add_p_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_p_gate(pair.first, pair.second);
        }
    }

    void add_ch_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CH");
        check_qubit_range_(target_index, "target qubit", "CH");
        gates_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CH, control_index, target_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_ch_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_ch_gate(pair.first, pair.second);
        }
    }

    void add_cx_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CX");
        check_qubit_range_(target_index, "target qubit", "CX");
        gates_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CX, control_index, target_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_cx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_cx_gate(pair.first, pair.second);
        }
    }

    void add_cy_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CY");
        check_qubit_range_(target_index, "target qubit", "CY");
        gates_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CY, control_index, target_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_cy_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_cy_gate(pair.first, pair.second);
        }
    }

    void add_cz_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CZ");
        check_qubit_range_(target_index, "target qubit", "CZ");
        gates_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CZ, control_index, target_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_cz_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_cz_gate(pair.first, pair.second);
        }
    }

    void add_csx_gate(std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CSX");
        check_qubit_range_(target_index, "target qubit", "CSX");
        gates_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CSX, control_index, target_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_csx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_csx_gate(pair.first, pair.second);
        }
    }

    void add_crx_gate(std::size_t control_index, std::size_t target_index, double angle)
    {
        check_qubit_range_(control_index, "control qubit", "CRX");
        check_qubit_range_(target_index, "target qubit", "CRX");
        gates_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRX, control_index, target_index, angle));
    }

    template <impl_ket::ControlAndTargetIndicesAndAngles Container = impl_ket::ControlAndTargetIndicesAndAnglesIList>
    void add_crx_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_crx_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_cry_gate(std::size_t control_index, std::size_t target_index, double angle)
    {
        check_qubit_range_(control_index, "control qubit", "CRY");
        check_qubit_range_(target_index, "target qubit", "CRY");
        gates_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRY, control_index, target_index, angle));
    }

    template <impl_ket::ControlAndTargetIndicesAndAngles Container = impl_ket::ControlAndTargetIndicesAndAnglesIList>
    void add_cry_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_cry_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_crz_gate(std::size_t control_index, std::size_t target_index, double angle)
    {
        check_qubit_range_(control_index, "control qubit", "CRZ");
        check_qubit_range_(target_index, "target qubit", "CRZ");
        gates_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRZ, control_index, target_index, angle));
    }

    template <impl_ket::ControlAndTargetIndicesAndAngles Container = impl_ket::ControlAndTargetIndicesAndAnglesIList>
    void add_crz_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_crz_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_cp_gate(std::size_t control_index, std::size_t target_index, double angle)
    {
        check_qubit_range_(control_index, "control qubit", "CP");
        check_qubit_range_(target_index, "target qubit", "CP");
        gates_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CP, control_index, target_index, angle));
    }

    template <impl_ket::ControlAndTargetIndicesAndAngles Container = impl_ket::ControlAndTargetIndicesAndAnglesIList>
    void add_cp_gate(const Container& tuples)
    {
        for (auto tuple : tuples) {
            add_cp_gate(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
        }
    }

    void add_u_gate(const Matrix2X2& gate, std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "U");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_ket::create_u_gate(target_index, gate_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_u_gate(const Matrix2X2& gate, const Container& indices)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto target_index : indices) {
            check_qubit_range_(target_index, "qubit", "U");
            gates_.emplace_back(impl_ket::create_u_gate(target_index, gate_index));
        }
    }

    void add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CU");
        check_qubit_range_(target_index, "target qubit", "CU");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, gate_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_cu_gate(const Matrix2X2& gate, const Container& tuples)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto tuple : tuples) {
            const auto control_index = std::get<0>(tuple);
            const auto target_index = std::get<1>(tuple);

            check_qubit_range_(control_index, "control qubit", "CU");
            check_qubit_range_(target_index, "target qubit", "CU");

            gates_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, gate_index));
        }
    }

    /*
        If no bit is provided to `add_m_gate()`, then the measured bit is assigned to the same
        index as the qubit's index.
    */
    void add_m_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "M");
        check_bit_range_(target_index);
        gates_.emplace_back(impl_ket::create_m_gate(target_index, target_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_m_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_m_gate(index);
        }
    }

    void add_m_gate(std::size_t target_index, std::size_t bit_index)
    {
        check_qubit_range_(target_index, "qubit", "M");
        check_bit_range_(bit_index);
        gates_.emplace_back(impl_ket::create_m_gate(target_index, bit_index));
    }

    template <impl_ket::QubitAndBitIndices Container = impl_ket::QubitAndBitIndicesIList>
    void add_m_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_m_gate(pair.first, pair.second);
        }
    }

    void add_if_statement(std::size_t bit_index, QuantumCircuit circuit)
    {
        namespace ctrl = impl_ket::control;

        check_bit_range_(bit_index);

        auto cfi = impl_ket::ControlFlowInstruction {
            impl_ket::SingleBitControlFlowFunction {bit_index, impl_ket::ControlBooleanKind::IF},
            std::make_unique<QuantumCircuit>(std::move(circuit))
        };

        control_flow_instructions_.push_back(std::move(cfi));

        const auto cfi_index = control_flow_instructions_.size() - 1;

        gates_.emplace_back(ctrl::create_control_flow_gate(cfi_index, ctrl::IF_STMT));
    }

    constexpr auto unitary_gate(std::size_t matrix_index) const noexcept -> const Matrix2X2&
    {
        return unitary_gates_[matrix_index];
    }

    auto control_flow_instruction(std::size_t index) const noexcept -> const impl_ket::ControlFlowInstruction&
    {
        return control_flow_instructions_[index];
    }

    friend auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit;
    friend void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right);
    friend auto transpile_to_primitive(const QuantumCircuit& circuit, double tolerance_sq) -> QuantumCircuit;

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<GateInfo> gates_ {};
    std::vector<Matrix2X2> unitary_gates_ {};
    std::vector<impl_ket::ControlFlowInstruction> control_flow_instructions_ {};

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
};

}  // namespace ket

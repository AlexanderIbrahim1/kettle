#pragma once

#include <cmath>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/gates/primitive_gate.hpp"
#include "kettle/circuit/control_flow.hpp"
#include "kettle/circuit/circuit_element.hpp"


namespace ket
{

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

    [[nodiscard]]
    constexpr auto n_qubits() const noexcept -> std::size_t
    {
        return n_qubits_;
    }

    [[nodiscard]]
    constexpr auto n_bits() const noexcept -> std::size_t
    {
        return n_bits_;
    }

    [[nodiscard]]
    constexpr auto n_circuit_elements() const noexcept -> std::size_t
    {
        return elements_.size();
    }

    constexpr auto operator[](std::size_t index) const noexcept -> const impl_ket::CircuitElement&
    {
        return elements_[index];
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept
    {
        return std::begin(elements_);
    }

    [[nodiscard]]
    constexpr auto end() const noexcept
    {
        return std::end(elements_);
    }

    void add_h_gate(std::size_t target_index)
    {
        check_qubit_range_(target_index, "qubit", "H");
        elements_.emplace_back(impl_ket::create_one_target_gate(Gate::H, target_index));
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
        elements_.emplace_back(impl_ket::create_one_target_gate(Gate::X, target_index));
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
        elements_.emplace_back(impl_ket::create_one_target_gate(Gate::Y, target_index));
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
        elements_.emplace_back(impl_ket::create_one_target_gate(Gate::Z, target_index));
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
        elements_.emplace_back(impl_ket::create_one_target_gate(Gate::SX, target_index));
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
        elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RX, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RY, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::RZ, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_target_one_angle_gate(Gate::P, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CH, control_index, target_index));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CX, control_index, target_index));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CY, control_index, target_index));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CZ, control_index, target_index));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_gate(Gate::CSX, control_index, target_index));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRX, control_index, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRY, control_index, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CRZ, control_index, target_index, angle));
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
        elements_.emplace_back(impl_ket::create_one_control_one_target_one_angle_gate(Gate::CP, control_index, target_index, angle));
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

        unitaries_.push_back(gate);
        const auto gate_index = unitaries_.size() - 1;

        elements_.emplace_back(impl_ket::create_u_gate(target_index, gate_index));
    }

    template <impl_ket::QubitIndices Container = impl_ket::QubitIndicesIList>
    void add_u_gate(const Matrix2X2& gate, const Container& indices)
    {
        unitaries_.push_back(gate);
        const auto gate_index = unitaries_.size() - 1;

        for (auto target_index : indices) {
            check_qubit_range_(target_index, "qubit", "U");
            elements_.emplace_back(impl_ket::create_u_gate(target_index, gate_index));
        }
    }

    void add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index)
    {
        check_qubit_range_(control_index, "control qubit", "CU");
        check_qubit_range_(target_index, "target qubit", "CU");

        unitaries_.push_back(gate);
        const auto gate_index = unitaries_.size() - 1;

        elements_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, gate_index));
    }

    template <impl_ket::ControlAndTargetIndices Container = impl_ket::ControlAndTargetIndicesIList>
    void add_cu_gate(const Matrix2X2& gate, const Container& tuples)
    {
        unitaries_.push_back(gate);
        const auto gate_index = unitaries_.size() - 1;

        for (auto tuple : tuples) {
            const auto control_index = std::get<0>(tuple);
            const auto target_index = std::get<1>(tuple);

            check_qubit_range_(control_index, "control qubit", "CU");
            check_qubit_range_(target_index, "target qubit", "CU");

            elements_.emplace_back(impl_ket::create_cu_gate(control_index, target_index, gate_index));
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
        elements_.emplace_back(impl_ket::create_m_gate(target_index, target_index));
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
        elements_.emplace_back(impl_ket::create_m_gate(target_index, bit_index));
    }

    template <impl_ket::QubitAndBitIndices Container = impl_ket::QubitAndBitIndicesIList>
    void add_m_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_m_gate(pair.first, pair.second);
        }
    }

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `subcircuit` executed.
    */
    void add_if_statement(ControlFlowPredicate predicate, QuantumCircuit circuit)
    {
        for (auto bit_index : predicate.bit_indices_to_check()) {
            check_bit_range_(bit_index);
        }

        auto cfi = impl_ket::ClassicalIfStatement {
            std::move(predicate),
            std::make_unique<QuantumCircuit>(std::move(circuit))
        };

        elements_.emplace_back(std::move(cfi));
    }

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `1`, executes `subcircuit`.
    */
    void add_if_statement(
        std::size_t bit_index,
        QuantumCircuit subcircuit
    )
    {
        auto predicate = ControlFlowPredicate {{bit_index}, {1}, ControlFlowBooleanKind::IF};
        add_if_statement(std::move(predicate), std::move(subcircuit));
    }

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `0`, executes `subcircuit`.
    */
    void add_if_not_statement(
        std::size_t bit_index,
        QuantumCircuit subcircuit
    )
    {
        auto predicate = ControlFlowPredicate {{bit_index}, {0}, ControlFlowBooleanKind::IF};
        add_if_statement(std::move(predicate), std::move(subcircuit));
    }

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `if_subcircuit` is executed, and if it evaluates
        to `false`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(
        ControlFlowPredicate predicate,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit)
    {
        for (auto bit_index : predicate.bit_indices_to_check()) {
            check_bit_range_(bit_index);
        }

        auto cfi = impl_ket::ClassicalIfElseStatement {
            std::move(predicate),
            std::make_unique<QuantumCircuit>(std::move(if_subcircuit)),
            std::make_unique<QuantumCircuit>(std::move(else_subcircuit))
        };

        elements_.emplace_back(std::move(cfi));
    }

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `1`, then `if_subcircuit` is executed, and
        if the classical bit is `0`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(
        std::size_t bit_index,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit
    )
    {
        auto predicate = ControlFlowPredicate {{bit_index}, {1}, ControlFlowBooleanKind::IF};
        add_if_else_statement(std::move(predicate), std::move(if_subcircuit), std::move(else_subcircuit));
    }

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `0`, then `if_subcircuit` is executed, and
        if the classical bit is `1`, then `else_subcircuit` is executed.
    */
    void add_if_not_else_statement(
        std::size_t bit_index,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit
    )
    {
        auto predicate = ControlFlowPredicate {{bit_index}, {0}, ControlFlowBooleanKind::IF};
        add_if_else_statement(std::move(predicate), std::move(if_subcircuit), std::move(else_subcircuit));
    }

    [[nodiscard]]
    constexpr auto unitary_gate(std::size_t matrix_index) const noexcept -> const Matrix2X2&
    {
        return unitaries_[matrix_index];
    }

    friend auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit;
    friend void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right);
    friend auto transpile_to_primitive(const QuantumCircuit& circuit, double tolerance_sq) -> QuantumCircuit;

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<impl_ket::CircuitElement> elements_;
    std::vector<Matrix2X2> unitaries_;

    void check_qubit_range_(std::size_t target_index, std::string_view qubit_name, std::string_view gate_name) const
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

    void check_bit_range_(std::size_t bit_index) const
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

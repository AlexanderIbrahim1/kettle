#pragma once

#include <cmath>
#include <iterator>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/utils.hpp"
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

    [[nodiscard]]
    constexpr auto operator[](std::size_t index) const noexcept -> const CircuitElement&
    {
        return elements_[index];
    }

    void pop_back();

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

    [[nodiscard]]
    constexpr auto circuit_elements() const noexcept -> const std::vector<CircuitElement>&
    {
        return elements_;
    }

    /*
        Add an H gate that acts on the qubit at `target_index`.
    */
    void add_h_gate(std::size_t target_index);

    /*
        Add an H gate that acts on each qubit whose indices are specified in `indices`.
    */
    template <QubitIndices Container = QubitIndicesIList>
    void add_h_gate(const Container& indices);

    void add_x_gate(std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_x_gate(const Container& indices);

    void add_y_gate(std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_y_gate(const Container& indices);

    void add_z_gate(std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_z_gate(const Container& indices);

    void add_sx_gate(std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_sx_gate(const Container& indices);

    void add_rx_gate(std::size_t target_index, double angle);

    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_rx_gate(const Container& pairs);

    void add_ry_gate(std::size_t target_index, double angle);

    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_ry_gate(const Container& pairs);

    void add_rz_gate(std::size_t target_index, double angle);

    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_rz_gate(const Container& pairs);

    void add_p_gate(std::size_t target_index, double angle);

    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_p_gate(const Container& pairs);

    void add_ch_gate(std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_ch_gate(const Container& pairs);

    void add_cx_gate(std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_cx_gate(const Container& pairs);

    void add_cy_gate(std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_cy_gate(const Container& pairs);

    void add_cz_gate(std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_cz_gate(const Container& pairs);

    void add_csx_gate(std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_csx_gate(const Container& pairs);

    void add_crx_gate(std::size_t control_index, std::size_t target_index, double angle);

    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_crx_gate(const Container& tuples);

    void add_cry_gate(std::size_t control_index, std::size_t target_index, double angle);

    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_cry_gate(const Container& tuples);

    void add_crz_gate(std::size_t control_index, std::size_t target_index, double angle);

    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_crz_gate(const Container& tuples);

    void add_cp_gate(std::size_t control_index, std::size_t target_index, double angle);

    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_cp_gate(const Container& tuples);

    void add_u_gate(const Matrix2X2& gate, std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_u_gate(const Matrix2X2& gate, const Container& indices);

    void add_cu_gate(const Matrix2X2& gate, std::size_t control_index, std::size_t target_index);

    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_cu_gate(const Matrix2X2& gate, const Container& pairs);

    /*
        If no bit is provided to `add_m_gate()`, then the measured bit is assigned to the same
        index as the qubit's index.
    */
    void add_m_gate(std::size_t target_index);

    template <QubitIndices Container = QubitIndicesIList>
    void add_m_gate(const Container& indices);

    void add_m_gate(std::size_t target_index, std::size_t bit_index);

    template <QubitAndBitIndices Container = QubitAndBitIndicesIList>
    void add_m_gate(const Container& pairs);

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `subcircuit` executed.
    */
    void add_if_statement(ControlFlowPredicate predicate, QuantumCircuit circuit);

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `1`, executes `subcircuit`.
    */
    void add_if_statement(std::size_t bit_index, QuantumCircuit subcircuit);

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `0`, executes `subcircuit`.
    */
    void add_if_not_statement(std::size_t bit_index, QuantumCircuit subcircuit);

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `if_subcircuit` is executed, and if it evaluates
        to `false`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(ControlFlowPredicate predicate, QuantumCircuit if_subcircuit, QuantumCircuit else_subcircuit);

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `1`, then `if_subcircuit` is executed, and
        if the classical bit is `0`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(std::size_t bit_index, QuantumCircuit if_subcircuit, QuantumCircuit else_subcircuit);

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `0`, then `if_subcircuit` is executed, and
        if the classical bit is `1`, then `else_subcircuit` is executed.
    */
    void add_if_not_else_statement(std::size_t bit_index, QuantumCircuit if_subcircuit, QuantumCircuit else_subcircuit);

    void add_classical_register_circuit_logger();

    void add_statevector_circuit_logger();

    void add_circuit_logger(CircuitLogger circuit_logger);

    friend auto append_circuits(QuantumCircuit left, const QuantumCircuit& right) -> QuantumCircuit;
    friend void extend_circuit(QuantumCircuit& left, const QuantumCircuit& right);
    friend auto transpile_to_primitive(const QuantumCircuit& circuit, double tolerance_sq) -> QuantumCircuit;

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<CircuitElement> elements_;

    void check_qubit_range_(std::size_t target_index, std::string_view qubit_name, std::string_view gate_name) const;

    void check_bit_range_(std::size_t bit_index) const;
};

}  // namespace ket

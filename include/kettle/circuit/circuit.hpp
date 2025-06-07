#pragma once

#include <cmath>
#include <iterator>
#include <unordered_map>
#include <vector>

#include "kettle/circuit/control_flow_predicate.hpp"
#include "kettle/circuit/control_flow.hpp"
#include "kettle/circuit/circuit_element.hpp"
#include "kettle/common/matrix2x2.hpp"
#include "kettle/common/tolerance.hpp"
#include "kettle/common/utils.hpp"
#include "kettle/parameter/parameter.hpp"


namespace ket
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

    [[nodiscard]]
    constexpr auto parameter_data_map() const noexcept -> const ket::param::ParameterDataMap&
    {
        return parameter_data_;
    }

    /*
        Takes the `id` of a parameter that is already present in the `QuantumCircuit`, and sets
        its value to `angle`.
    */
    void set_parameter_value(const ket::param::ParameterID& id, double angle);

    /*
        Add an H gate that acts on the qubit at `target_index`.
    */
    void add_h_gate(std::size_t target_index);
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

    void add_s_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_s_gate(const Container& indices);

    void add_sdag_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_sdag_gate(const Container& indices);

    void add_t_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_t_gate(const Container& indices);

    void add_tdag_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_tdag_gate(const Container& indices);

    void add_sx_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_sx_gate(const Container& indices);

    void add_sxdag_gate(std::size_t target_index);
    template <QubitIndices Container = QubitIndicesIList>
    void add_sxdag_gate(const Container& indices);

    void add_rx_gate(std::size_t target_index, double angle);
    auto add_rx_gate(std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_rx_gate(std::size_t target_index, const ket::param::ParameterID& id);
    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_rx_gate(const Container& pairs);

    void add_ry_gate(std::size_t target_index, double angle);
    auto add_ry_gate(std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_ry_gate(std::size_t target_index, const ket::param::ParameterID& id);
    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_ry_gate(const Container& pairs);

    void add_rz_gate(std::size_t target_index, double angle);
    auto add_rz_gate(std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_rz_gate(std::size_t target_index, const ket::param::ParameterID& id);
    template <QubitIndicesAndAngles Container = QubitIndicesAndAnglesIList>
    void add_rz_gate(const Container& pairs);

    void add_p_gate(std::size_t target_index, double angle);
    auto add_p_gate(std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_p_gate(std::size_t target_index, const ket::param::ParameterID& id);
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

    void add_cs_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_cs_gate(const Container& pairs);

    void add_csdag_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_csdag_gate(const Container& pairs);

    void add_ct_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_ct_gate(const Container& pairs);

    void add_ctdag_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_ctdag_gate(const Container& pairs);

    void add_csx_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_csx_gate(const Container& pairs);

    void add_csxdag_gate(std::size_t control_index, std::size_t target_index);
    template <ControlAndTargetIndices Container = ControlAndTargetIndicesIList>
    void add_csxdag_gate(const Container& pairs);

    void add_crx_gate(std::size_t control_index, std::size_t target_index, double angle);
    auto add_crx_gate(std::size_t control_index, std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_crx_gate(std::size_t control_index, std::size_t target_index, const ket::param::ParameterID& id);
    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_crx_gate(const Container& tuples);

    void add_cry_gate(std::size_t control_index, std::size_t target_index, double angle);
    auto add_cry_gate(std::size_t control_index, std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_cry_gate(std::size_t control_index, std::size_t target_index, const ket::param::ParameterID& id);
    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_cry_gate(const Container& tuples);

    void add_crz_gate(std::size_t control_index, std::size_t target_index, double angle);
    auto add_crz_gate(std::size_t control_index, std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_crz_gate(std::size_t control_index, std::size_t target_index, const ket::param::ParameterID& id);
    template <ControlAndTargetIndicesAndAngles Container = ControlAndTargetIndicesAndAnglesIList>
    void add_crz_gate(const Container& tuples);

    void add_cp_gate(std::size_t control_index, std::size_t target_index, double angle);
    auto add_cp_gate(std::size_t control_index, std::size_t target_index, double initial_angle, ket::param::parameterized key) -> ket::param::ParameterID;
    void add_cp_gate(std::size_t control_index, std::size_t target_index, const ket::param::ParameterID& id);
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

    // --- NON-PRIMITIVE GATES ---
    void add_ccx_gate(std::size_t control_index0, std::size_t control_index1, std::size_t target_index);
    template <TwoControlOneTargetIndices Container = TwoControlOneTargetIndicesIList>
    void add_ccx_gate(const Container& triplets);

    void add_ccu_gate(const Matrix2X2& unitary, std::size_t control_index0, std::size_t control_index1, std::size_t target_index);
    template <TwoControlOneTargetIndices Container = TwoControlOneTargetIndicesIList>
    void add_ccu_gate(const Matrix2X2& unitary, const Container& triplets);

    // --- NON-GATE CIRCUIT ELEMENTS ---

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `subcircuit` executed.
    */
    void add_if_statement(
        ControlFlowPredicate predicate,
        QuantumCircuit circuit,
        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `1`, executes `subcircuit`.
    */
    void add_if_statement(
        std::size_t bit_index,
        QuantumCircuit subcircuit//,
//        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

    /*
        Add a classical if statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`, and if it is set to `0`, executes `subcircuit`.
    */
    void add_if_not_statement(
        std::size_t bit_index,
        QuantumCircuit subcircuit//,
//        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This function takes a custom `ControlFlowPredicate` instance, and if it evaluates to `true`
        for the current classical register, then `if_subcircuit` is executed, and if it evaluates
        to `false`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(
        ControlFlowPredicate predicate,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit//,
//        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `1`, then `if_subcircuit` is executed, and
        if the classical bit is `0`, then `else_subcircuit` is executed.
    */
    void add_if_else_statement(
        std::size_t bit_index,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit//,
//        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

    /*
        Add a classical if-else statement to the `QuantumCircuit`.

        This statement reads the value of the measured classical bit in the classical register
        given by `bit_index`; if the classical bit is `0`, then `if_subcircuit` is executed, and
        if the classical bit is `1`, then `else_subcircuit` is executed.
    */
    void add_if_not_else_statement(
        std::size_t bit_index,
        QuantumCircuit if_subcircuit,
        QuantumCircuit else_subcircuit//,
//        double tolerance = MATCHING_PARAMETER_VALUE_TOLERANCE
    );

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
    ket::param::ParameterDataMap parameter_data_;
    std::size_t parameter_count_ {0};

    void check_qubit_range_(std::size_t target_index, std::string_view qubit_name, std::string_view gate_name) const;

    void check_bit_range_(std::size_t bit_index) const;

    void add_one_target_gate_(std::size_t target_index, ket::Gate gate);
    void add_one_target_one_angle_gate_(std::size_t target_index, double angle, ket::Gate gate);
    void add_one_control_one_target_gate_(std::size_t control_index, std::size_t target_index, ket::Gate gate);
    void add_one_control_one_target_one_angle_gate_(std::size_t control_index, std::size_t target_index, double angle, ket::Gate gate);

    auto add_one_target_one_parameter_gate_with_angle_(
        std::size_t target_index,
        double initial_angle,
        Gate gate,
        [[maybe_unused]] param::parameterized key
    ) -> ket::param::ParameterID;

    void add_one_target_one_parameter_gate_without_angle_(
        std::size_t target_index,
        Gate gate,
        const ket::param::ParameterID& id
    );

    auto add_one_control_one_target_one_parameter_gate_with_angle_(
        std::size_t control_index,
        std::size_t target_index,
        double initial_angle,
        Gate gate,
        [[maybe_unused]] ket::param::parameterized key
    ) -> ket::param::ParameterID;

    void add_one_control_one_target_one_parameter_gate_without_angle_(
        std::size_t control_index,
        std::size_t target_index,
        Gate gate,
        const ket::param::ParameterID& id
    );

    void merge_subcircuit_parameters_(const QuantumCircuit& subcircuit, double tolerance);

    auto update_existing_parameter_data_(const ket::param::ParameterID& id) -> ket::param::ParameterExpression;
    auto create_uninitialized_parameter_data_(const ket::param::ParameterID& id) -> ket::param::ParameterExpression;
    auto create_initialized_parameter_data_(
        double value
    ) -> std::tuple<ket::param::ParameterExpression, ket::param::ParameterID>;
};

}  // namespace ket

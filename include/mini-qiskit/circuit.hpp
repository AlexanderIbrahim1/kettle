#pragma once

#include <cmath>
#include <concepts>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "mini-qiskit/common/matrix2x2.hpp"
#include "mini-qiskit/common/utils.hpp"
#include "mini-qiskit/primitive_gate.hpp"

namespace impl_mqis
{

struct MatrixIndexPair
{
    std::size_t i_left;
    std::size_t i_right;

    bool operator==(const MatrixIndexPair& other) const = default;
};

struct MatrixIndexPairKeyHash
{
    auto operator()(const MatrixIndexPair& pair) const
    {
        return std::hash<std::size_t>()(pair.i_left) ^ (std::hash<std::size_t>()(pair.i_right) << 1);
    }
};

inline auto is_gate_info_almost_eq(const mqis::GateInfo& left, const mqis::GateInfo& right, double tolerance) -> bool
{
    using G = mqis::Gate;

    if (left.gate != right.gate) {
        return false;
    }

    switch (left.gate)
    {
        case G::X : {
            return unpack_x_gate(left) == unpack_x_gate(right);
        }
        case G::RX : {
            const auto [left_angle, left_qubit] = unpack_rx_gate(left);
            const auto [right_angle, right_qubit] = unpack_rx_gate(right);
            return left_qubit == right_qubit && std::fabs(left_angle - right_angle) < tolerance;
        }
        case G::H : {
            return unpack_h_gate(left) == unpack_h_gate(right);
        }
        case G::CX : {
            return unpack_cx_gate(left) == unpack_cx_gate(right);
        }
        case G::CRX : {
            const auto [left_control_qubit, left_target_qubit, left_angle] = unpack_crx_gate(left);
            const auto [right_control_qubit, right_target_qubit, right_angle] = unpack_crx_gate(right);
            return left_control_qubit == right_control_qubit && \
                left_target_qubit == right_target_qubit && \
                std::fabs(left_angle - right_angle) < tolerance;
        }
        case G::CP : {
            const auto [left_control_qubit, left_target_qubit, left_angle] = unpack_cp_gate(left);
            const auto [right_control_qubit, right_target_qubit, right_angle] = unpack_cp_gate(right);
            return left_control_qubit == right_control_qubit && \
                left_target_qubit == right_target_qubit && \
                std::fabs(left_angle - right_angle) < tolerance;
        }
        case G::U : {
            [[maybe_unused]]
            const auto [left_qubit_index, left_ignore] = unpack_u_gate(left);
            [[maybe_unused]]
            const auto [right_qubit_index, right_ignore] = unpack_u_gate(right);
            return left_qubit_index == right_qubit_index;
        }
        case G::CU : {
            [[maybe_unused]]
            const auto [left_control_qubit, left_target_qubit, left_ignore] = unpack_cu_gate(left);
            [[maybe_unused]]
            const auto [right_control_qubit, right_target_qubit, right_ignore] = unpack_cu_gate(right);
            return left_control_qubit == right_control_qubit && left_target_qubit == right_target_qubit;
        }
        case G::M : {
            return unpack_m_gate(left) == unpack_m_gate(right);
        }
        default : {
            throw std::runtime_error {"UNREACHABLE: invalid GateInfo instance given"};
        }
    }
}

}  // namespace impl_mqis

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

    void add_x_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "X");
        check_previous_gate_is_not_measure_(qubit_index, "X");
        gates_.emplace_back(impl_mqis::create_x_gate(qubit_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_x_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_x_gate(index);
        }
    }

    void add_rx_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RX");
        check_previous_gate_is_not_measure_(qubit_index, "RX");
        gates_.emplace_back(impl_mqis::create_rx_gate(theta, qubit_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_rx_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rx_gate(pair.first, pair.second);
        }
    }

    void add_h_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "H");
        check_previous_gate_is_not_measure_(qubit_index, "H");
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_h_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_h_gate(index);
        }
    }

    void add_cx_gate(std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CX");
        check_qubit_range_(target_index, "target qubit", "CX");
        check_previous_gate_is_not_measure_(source_index, "CX");
        check_previous_gate_is_not_measure_(target_index, "CX");
        gates_.emplace_back(impl_mqis::create_cx_gate(source_index, target_index));
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

    void add_cz_gate(std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CZ");
        check_qubit_range_(target_index, "target qubit", "CZ");
        check_previous_gate_is_not_measure_(source_index, "CZ");
        check_previous_gate_is_not_measure_(target_index, "CZ");
        gates_.emplace_back(impl_mqis::create_h_gate(target_index));
        gates_.emplace_back(impl_mqis::create_cx_gate(source_index, target_index));
        gates_.emplace_back(impl_mqis::create_h_gate(target_index));
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

    void add_crx_gate(double theta, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CRX");
        check_qubit_range_(target_index, "target qubit", "CRX");
        check_previous_gate_is_not_measure_(source_index, "CRX");
        check_previous_gate_is_not_measure_(target_index, "CRX");
        gates_.emplace_back(impl_mqis::create_crx_gate(source_index, target_index, theta));
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

    void add_crz_gate(double theta, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CRZ");
        check_qubit_range_(target_index, "target qubit", "CRZ");
        check_previous_gate_is_not_measure_(source_index, "CRZ");
        check_previous_gate_is_not_measure_(target_index, "CRZ");
        gates_.emplace_back(impl_mqis::create_h_gate(target_index));
        gates_.emplace_back(impl_mqis::create_crx_gate(source_index, target_index, theta));
        gates_.emplace_back(impl_mqis::create_h_gate(target_index));
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

    void add_cp_gate(double theta, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CP");
        check_qubit_range_(target_index, "target qubit", "CP");
        check_previous_gate_is_not_measure_(source_index, "CP");
        check_previous_gate_is_not_measure_(target_index, "CP");
        gates_.emplace_back(impl_mqis::create_cp_gate(source_index, target_index, theta));
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

    void add_rz_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RZ");
        check_previous_gate_is_not_measure_(qubit_index, "RZ");
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
        gates_.emplace_back(impl_mqis::create_rx_gate(theta, qubit_index));
        gates_.emplace_back(impl_mqis::create_h_gate(qubit_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_rz_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_rz_gate(pair.first, pair.second);
        }
    }

    void add_ry_gate(double theta, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "RY");
        check_previous_gate_is_not_measure_(qubit_index, "RY");
        gates_.emplace_back(impl_mqis::create_rx_gate(M_PI_2, qubit_index));
        add_rz_gate(theta, qubit_index);
        gates_.emplace_back(impl_mqis::create_rx_gate(-M_PI_2, qubit_index));
    }

    template <
        impl_mqis::ContainerOfAnglesAndQubitIndices Container = std::initializer_list<std::pair<double, std::size_t>>>
    void add_ry_gate(const Container& pairs)
    {
        for (auto pair : pairs) {
            add_ry_gate(pair.first, pair.second);
        }
    }

    void add_z_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Z");
        check_previous_gate_is_not_measure_(qubit_index, "Z");
        add_rz_gate(M_PI, qubit_index);
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_z_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_z_gate(index);
        }
    }

    void add_y_gate(std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "Y");
        check_previous_gate_is_not_measure_(qubit_index, "Y");
        add_z_gate(qubit_index);
        add_x_gate(qubit_index);
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_y_gate(const Container& indices)
    {
        for (auto index : indices) {
            add_y_gate(index);
        }
    }

    void add_u_gate(const Matrix2X2& gate, std::size_t qubit_index)
    {
        check_qubit_range_(qubit_index, "qubit", "U");
        check_previous_gate_is_not_measure_(qubit_index, "U");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_mqis::create_u_gate(qubit_index, gate_index));
    }

    template <impl_mqis::ContainerOfQubitIndices Container = std::initializer_list<std::size_t>>
    void add_u_gate(const Matrix2X2& gate, const Container& indices)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto qubit_index : indices) {
            check_qubit_range_(qubit_index, "qubit", "U");
            check_previous_gate_is_not_measure_(qubit_index, "U");

            gates_.emplace_back(impl_mqis::create_u_gate(qubit_index, gate_index));
        }
    }

    void add_cu_gate(const Matrix2X2& gate, std::size_t source_index, std::size_t target_index)
    {
        check_qubit_range_(source_index, "source qubit", "CU");
        check_qubit_range_(target_index, "target qubit", "CU");
        check_previous_gate_is_not_measure_(source_index, "CU");
        check_previous_gate_is_not_measure_(target_index, "CU");

        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        gates_.emplace_back(impl_mqis::create_cu_gate(source_index, target_index, gate_index));
    }

    template <
        impl_mqis::ContainerOfControlAndTargetQubitIndices Container =
            std::initializer_list<std::pair<std::size_t, std::size_t>>>
    void add_cu_gate(const Matrix2X2& gate, const Container& tuples)
    {
        unitary_gates_.push_back(gate);
        const auto gate_index = unitary_gates_.size() - 1;

        for (auto tuple : tuples) {
            const auto source_index = std::get<0>(tuple);
            const auto target_index = std::get<1>(tuple);

            check_qubit_range_(source_index, "source qubit", "CU");
            check_qubit_range_(target_index, "target qubit", "CU");
            check_previous_gate_is_not_measure_(source_index, "CU");
            check_previous_gate_is_not_measure_(target_index, "CU");

            gates_.emplace_back(impl_mqis::create_cu_gate(source_index, target_index, gate_index));
        }
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

private:
    std::size_t n_qubits_;
    std::size_t n_bits_;
    std::vector<std::uint8_t> measure_bitmask_;
    std::vector<GateInfo> gates_ {};
    std::vector<Matrix2X2> unitary_gates_ {};

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

auto almost_eq(
    const QuantumCircuit& left,
    const QuantumCircuit& right,
    double matrix_complex_tolerance_sq = impl_mqis::COMPLEX_ALMOST_EQ_TOLERANCE_SQ,
    double angle_tolerance_sq = impl_mqis::ANGLE_ALMOST_EQ_TOLERANCE
) -> bool
{
    // begin with the fastest checks first (qubits, bits, and bitmask values)
    if (left.n_qubits() != right.n_qubits()) {
        return false;
    }

    if (left.n_bits() != right.n_bits()) {
        return false;
    }

    if (left.measure_bitmask() != right.measure_bitmask()) {
        return false;
    }

    // don't bother checking the gates if there aren't the same number on both sides
    const auto n_left_gates  = static_cast<std::size_t>(std::distance(left.begin(), left.end()));
    const auto n_right_gates = static_cast<std::size_t>(std::distance(right.begin(), right.end()));

    if (n_left_gates != n_right_gates) {
        return false;
    }

    // don't bother rechecking matrix index pairs already seen
    auto checked_matrix_index_pairs = std::unordered_set<impl_mqis::MatrixIndexPair, impl_mqis::MatrixIndexPairKeyHash> {};

    for (std::size_t i_gate {0}; i_gate < n_left_gates; ++i_gate) {
        const auto& left_info = left[i_gate];
        const auto& right_info = right[i_gate];

        if (!impl_mqis::is_gate_info_almost_eq(left_info, right_info, angle_tolerance_sq)) {
            return false;
        }

        // the previous function has already checked that left_info.gate == right_info.gate
        // NOTE: we check the matrices independently of the index because two gates could refer to the
        // same matrix, but that matrix could be located at a different index
        if (left_info.gate == Gate::U || left_info.gate == Gate::CU) {
            const auto i_left_matrix = impl_mqis::unpack_matrix_index(left_info);
            const auto i_right_matrix = impl_mqis::unpack_matrix_index(right_info);

            if (checked_matrix_index_pairs.contains({i_left_matrix, i_right_matrix})) {
                continue;
            }

            const auto& lmatrix = left.unitary_gate(i_left_matrix);
            const auto& rmatrix = right.unitary_gate(i_right_matrix);

            if (!almost_eq(lmatrix, rmatrix, matrix_complex_tolerance_sq)) {
                return false;
            }

            checked_matrix_index_pairs.insert({i_left_matrix, i_right_matrix});
        }
    }

    return true;
}

}  // namespace mqis

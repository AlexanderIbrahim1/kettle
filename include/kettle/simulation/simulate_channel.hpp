#pragma once

#include <vector>

#include <Eigen/Dense>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/state/density_matrix.hpp"


namespace ket
{

class OneQubitKrausChannelSimulator;

template <typename Derived>
class ChannelSimulatorMixin
{
public:
    [[nodiscard]]
    auto has_been_run() const noexcept -> bool
    {
        return has_been_run_;
    }

    [[nodiscard]]
    auto classical_register() const -> const ClassicalRegister&
    {
        if (!cregister_) {
            throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
        }

        return *cregister_;
    }

    auto classical_register() -> ClassicalRegister&
    {
        if (!cregister_) {
            throw std::runtime_error {"ERROR: Cannot access classical register; no simulation has been run\n"};
        }

        return *cregister_;
    }

    [[nodiscard]]
    auto circuit_loggers() const noexcept -> const std::vector<CircuitLogger>&
    {
        return circuit_loggers_;
    }

protected:
    // these member functions provide the derived channels with access to the private data members
    auto cregister_mixin() noexcept -> ket::ClonePtr<ClassicalRegister>& {
        return cregister_;
    }

    auto has_been_run_mixin() noexcept -> bool& {
        return has_been_run_;
    }

    auto circuit_loggers_mixin() noexcept -> std::vector<CircuitLogger>& {
        return circuit_loggers_;
    }

private:
    ChannelSimulatorMixin() = default;

    // there is no default constructor for the ClassicalRegsiter (it wouldn't make sense), and we
    // only find out how many bits are needed after the first simulation; hence why we use a pointer
    ket::ClonePtr<ClassicalRegister> cregister_ {nullptr};
    bool has_been_run_ {false};
    std::vector<CircuitLogger> circuit_loggers_;

    friend OneQubitKrausChannelSimulator;
};


class OneQubitKrausChannelSimulator : public ChannelSimulatorMixin<OneQubitKrausChannelSimulator>
{
public:
    explicit OneQubitKrausChannelSimulator(std::size_t n_qubits);

    void run(const OneQubitKrausChannel& channel, DensityMatrix& state);

private:
    Eigen::MatrixXcd writing_buffer_;
    Eigen::MatrixXcd left_mul_buffer_;
    Eigen::MatrixXcd right_mul_buffer_;
    std::size_t n_qubits_;
};

void simulate(const OneQubitKrausChannel& circuit, DensityMatrix& state);

}  // namespace ket

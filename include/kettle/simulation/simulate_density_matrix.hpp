#pragma once

#include <optional>
#include <vector>

#include <Eigen/Dense>

#include "kettle/circuit/classical_register.hpp"
#include "kettle/circuit/circuit.hpp"
#include "kettle/circuit_loggers/circuit_logger.hpp"
#include "kettle/common/clone_ptr.hpp"
#include "kettle/state/density_matrix.hpp"


namespace ket
{

class DensityMatrixSimulator
{
public:
    explicit DensityMatrixSimulator(std::size_t n_qubits);

    void run(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed = std::nullopt);

    [[nodiscard]]
    auto has_been_run() const -> bool;

    [[nodiscard]]
    auto classical_register() const -> const ClassicalRegister&;

    auto classical_register() -> ClassicalRegister&;

    [[nodiscard]]
    auto circuit_loggers() const -> const std::vector<CircuitLogger>&;

private:
    // there is no default constructor for the ClassicalRegsiter (it wouldn't make sense), and we
    // only find out how many bits are needed after the first simulation; hence why we use a pointer
    ket::ClonePtr<ClassicalRegister> cregister_ {nullptr};
    bool has_been_run_ {false};
    std::vector<CircuitLogger> circuit_loggers_;
    Eigen::MatrixXcd buffer_;
};


void simulate(const QuantumCircuit& circuit, DensityMatrix& state, std::optional<int> prng_seed = std::nullopt);

}  // namespace ket

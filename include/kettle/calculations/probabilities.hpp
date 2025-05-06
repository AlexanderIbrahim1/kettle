#pragma once

#include <map>
#include <string>
#include <vector>

#include "kettle/state/state.hpp"

/*
    This file contains code components to calculate the probabilities of each of
    the individual computational states in the overall QuantumState object.

    It is also possible to add noise to the measurements.
*/

namespace ket
{

/*
    The QuantumNoise class holds the noise applied to the probabilities calculated
    from the QuantumState object.

    This is basically a thin wrapper around a `std::vector<double>` that checks if
    the noise satisfies certain conditions.
*/
class QuantumNoise
{
public:
    explicit QuantumNoise(std::size_t n_qubits);

    void set(std::size_t index, double noise);

    [[nodiscard]]
    auto get(std::size_t index) const -> const double&;

private:
    std::size_t n_qubits_;
    std::vector<double> noise_;

    void check_index_(std::size_t index) const;
};

auto calculate_probabilities_raw(
    const QuantumState& state,
    const QuantumNoise* noise = nullptr
) -> std::vector<double>;

auto calculate_probabilities(
    const QuantumState& state,
    const QuantumNoise* noise = nullptr
) -> std::map<std::string, double>;

}  // namespace ket

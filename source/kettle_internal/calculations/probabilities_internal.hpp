#pragma once

#include <vector>


namespace ket::internal
{

void apply_noise_(double noise, std::size_t i_qubit, std::size_t n_qubits, std::vector<double>& probabilities);

/*
    Ensures that the noise parameter lies in [0.0, 1.0]; otherwise, the noise application is invalid.
*/
void check_noise_value_(double value);

}  // namespace ket::internal

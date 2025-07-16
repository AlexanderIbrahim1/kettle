#pragma once

#include "kettle/operator/channels/one_qubit_kraus_channel.hpp"
#include "kettle/operator/channels/pauli_channel.hpp"
#include "kettle/common/utils.hpp"

/*
    This header contains the functions for implementing the following error types:
      - symmetric depolarizing error (as a Pauli Channel)
      - amplitude error (as a Kraus channel)
      - phase damping error (as a Kraus channel)
      - phase-amplitude damping error (as a Kraus channel)
      - thermal relaxation error (as a Kraus channel)
      - reset error (as a Mixed Circuit channel)
      - coherent unitary error (as a Mixed Circuit channel)
    
    Some error types aren't implemented as functions; you should call the constructors directly:
      - general pauli errors (as a PauliChannel)
      - mixed unitary error (as a MixedCircuitChannel)
*/

namespace ket
{

/*
    The symmetric depolarizing error channel applied to a single qubit.

    For this definition:
      - p = 0 gives a noiseless channel
      - p = 3/4 gives a full depolarized channel, and the output will be proportional to the identity matrix
      - p = 1 gives the uniform Pauli error channel, where X, Y, and Z are applied equally to the 1-qubit density matrix
*/
template <QubitIndices Container = QubitIndicesIList>
auto symmetric_depolarizing_error_channel(
    double depolarizing_parameter,
    std::size_t n_qubits,
    const Container& indices
) -> ket::PauliChannel;

/*
    The parameters needed to create phase-amplitude damping error channel
      - `amplitude` is the amplitude damping parameter
      - `phase` is phase damping parameter
      - `excited_population` is the population of the `|1>` state in the thermal bath

    For this definition:
      - `parameters.amplitude`, `parameters.phase`, and `parameters.excited_population`
         must all lie within [0, 1].
      - the sum of `parameters.amplitude` and `parameters.phase` must not exceed 1
*/
struct PhaseAmplitudeDampingParameters {
    double amplitude;
    double phase;
    double excited_population;
};

/*
    The one-qubit combined phase and amplitude damping error channel, applied to a single qubit.
      - `tolerance`: omit Kraus matrices from the channel if their Frobenius norm is less than this
*/
auto one_qubit_phase_amplitude_damping_error_channel(
    const PhaseAmplitudeDampingParameters& parameters,
    std::size_t target_index,
    double tolerance = 1.0e-6
) -> ket::OneQubitKrausChannel;

struct relax_infinite {};

struct RelaxationTime
{
public:
    explicit RelaxationTime(double time)
        : time_ {time}
        , is_infinite_ {false}
    {
        if (time_ <= 0.0) {
            throw std::runtime_error {"ERROR: relaxation time must be positive.\n"};
        }
    }

    explicit RelaxationTime([[maybe_unused]] relax_infinite infinite_tag)
        : time_ {0.0}  // fine if value is invalid; cannot get time under this state
        , is_infinite_ {true}
    {}

    [[nodiscard]]
    constexpr auto is_infinite() const -> bool
    {
        return is_infinite_;
    }

    [[nodiscard]]
    constexpr auto time() const -> double
    {
        if (is_infinite_) {
            throw std::runtime_error {"ERROR: the relaxation time is infinte; cannot get time.\n"};
        }

        return time_;
    }

private:
    double time_;
    bool is_infinite_;
};

/*
    The parameters needed to create a thermal relaxation error channel
      - `t1` is the relaxation time for the loss of energy of the qubit
      - `t2` is the phase coherence time
      - `gate_time` is the amount of relaxation time that the gate operation takes
      - `excited_population` is the population of the `|1>` state in the thermal bath
*/
struct ThermalRelaxationParameters {
    // NOTE: T1 and T2 are very common variables in QC literature, so it's probably better to leave these
    // as is instead of giving them full names; also to prevent confusion with the "gate_time" variable,
    // which is also a time but not a constant.
    RelaxationTime t1;
    RelaxationTime t2;
    double gate_time;
    double excited_population;
};

/*
    The one-qubit thermal relaxation error channel, applied to a single qubit.
      - `tolerance`: omit Kraus matrices from the channel if their Frobenius norm is less than this

    In principle, the thermal relaxation error is just the phase-amplitude damping error, with the
    parameters given by:
    
    param[amplitude] = 1 - exp(- t / T1)
    param[phase]     = 1 - exp(- t / Tp), where `Tp = (T1 * T2) / (2 * T1 - T2)`

    However, the Kraus decomposition used in the phase-amplitude damping error restricts the paramters
    to the parameter space of `0 <= param[amplitude] + param[phase] <= 1`, while the thermal relaxation
    error's formalism does not!

    TODO: remove the magic number for the tolerance
*/
auto one_qubit_thermal_relaxation_error_channel(
    const ThermalRelaxationParameters& parameters,
    std::size_t target_index,
    double tolerance = 1.0e-6
) -> ket::OneQubitKrausChannel;

}  // namespace ket
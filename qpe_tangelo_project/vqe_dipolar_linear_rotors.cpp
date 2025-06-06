#include "kettle/io/read_pauli_operator.hpp"
#include "kettle/optimize/n_local.hpp"
#include <vector>

#include <nlopt.hpp>

#include <kettle/kettle.hpp>

/*
    This code:
      - loads in the pauli operator for a Hamiltonian for a system of dipolar linear rotors
      - performs VQE to find the ground state energy
*/

using PT = ket::PauliTerm;
using G = ket::Gate;
using Entangle = ket::NLocalEntangelement;

static constexpr auto N_UNITARY_QUBITS_TWO_ROTOR = std::size_t {6};
static constexpr auto N_UNITARY_QUBITS_THREE_ROTOR = std::size_t {9};

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 5) {
            throw std::runtime_error {
                "./a.out n_rotors n_repetitions abs_input_filepath abs_output_filepath\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_rotors = std::stoul(arguments[0]);
        n_repetitions = std::stoul(arguments[1]);
        abs_input_filepath = std::filesystem::path {arguments[2]};
        abs_output_filepath = std::filesystem::path {arguments[3]};

        if (n_rotors == 2) {
            n_qubits = N_UNITARY_QUBITS_TWO_ROTOR;
        }
        else if (n_rotors == 3) {
            n_qubits = N_UNITARY_QUBITS_THREE_ROTOR;
        }
        else {
            throw std::runtime_error {
                "Invalid number of rotors passed; allowed values are '2' and '3'\n"
            };
        }
    }

    std::size_t n_rotors;
    std::size_t n_repetitions;
    std::size_t n_qubits;
    std::filesystem::path abs_input_filepath;
    std::filesystem::path abs_output_filepath;
};

struct OptimizationContext
{
    ket::QuantumCircuit circuit;
    std::vector<ket::param::ParameterID> parameter_ids;
    ket::PauliOperator pauli_op;
    std::size_t iteration;
    std::size_t n_qubits;
};

auto main(int argc, char** argv) -> int
{
    const auto args = [&]() {
        try {
            return CommandLineArguments {argc, argv};
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
            std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
        }
    }();

    auto pauli_op = ket::read_pauli_operator(args.abs_input_filepath, args.n_qubits);

    // this is the circuit we use to find the ideal parameters
    auto circuit = ket::QuantumCircuit {args.n_qubits};

    // perform the initial state construction
    for (std::size_t i {0}; i < args.n_rotors; ++i) {
        circuit.add_x_gate((3 * i) + 2);
    }

    // create the ansatz, and append it to the existing circuit
    auto [n_local, parameter_ids] = ket::n_local(args.n_qubits, {G::RX, G::RY, G::RZ}, {G::CX}, Entangle::LINEAR, args.n_repetitions);
    ket::extend_circuit(circuit, n_local);

    auto context = OptimizationContext {
        .circuit=std::move(circuit),
        .parameter_ids=parameter_ids,
        .pauli_op=std::move(pauli_op),
        .iteration=0,
        .n_qubits=args.n_qubits,
    };

    const auto n_parameters = context.parameter_ids.size();

    std::cout << std::fixed << std::setprecision(12);

    // this is the function that gets passed to the optimizer
    const auto cost_function = [](
        const std::vector<double> &parameters,
        [[maybe_unused]] std::vector<double> &grad,
        void *data
    ) -> double
    {
        auto* ctx = static_cast<OptimizationContext*>(data);

        for (std::size_t i {0}; i < parameters.size(); ++i) {
            ctx->circuit.set_parameter_value(ctx->parameter_ids[i], parameters[i]);
        }

        auto statevector = ket::QuantumState {ctx->n_qubits};
        ket::simulate(ctx->circuit, statevector);

        const auto exp_value = ket::expectation_value(ctx->pauli_op, statevector);

        std::cout << "exp_value[" << ctx->iteration << "] = " << exp_value.real() << '\n';
        ++(ctx->iteration);

        return exp_value.real();
    };

    // create the initial set of parameters
    // TODO: maybe make this random later
    auto parameters = std::vector<double>(n_parameters, 1.0);

    // creat the optimization function, and set some parameters to make sure it converges or stops
    auto opt = nlopt::opt {nlopt::LN_COBYLA, static_cast<unsigned int>(n_parameters)};
    opt.set_min_objective(cost_function, &context);
    opt.set_xtol_rel(1.0e-4);
    opt.set_maxeval(1000);

    double minimum_eigenvalue;  // NOLINT

    [[maybe_unused]]
    const auto result = opt.optimize(parameters, minimum_eigenvalue);

    std::cout << "The minimum eigenvalue is " << minimum_eigenvalue << '\n';

    return 0;
}

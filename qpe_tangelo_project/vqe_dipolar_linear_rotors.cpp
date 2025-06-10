#include <format>
#include <fstream>
#include <ios>
#include <random>
#include <sstream>
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
using CG = ket::CompoundGate;
using Entangle = ket::NLocalEntangelement;

struct CommandLineArguments
{
    CommandLineArguments(int argc, char** argv)
    {
        if (argc != 10) {
            throw std::runtime_error {
                "./a.out n_rotors n_qubits_per_rotor n_repetitions n_parameters_saveevery n_max_iterations abs_input_filepath abs_output_dirpath data_filename i_load_parameters\n"
            };
        }

        const auto arguments = std::vector<std::string> (argv + 1, argv + argc);

        n_rotors = std::stoul(arguments[0]);
        n_qubits_per_rotor = std::stoul(arguments[1]);
        n_repetitions = std::stoul(arguments[2]);
        n_parameters_saveevery = std::stoul(arguments[3]);
        n_max_iterations = std::stoul(arguments[4]);
        abs_input_filepath = std::filesystem::path {arguments[5]};
        abs_output_dirpath = std::filesystem::path {arguments[6]};
        data_filename = arguments[7];
        i_load_parameters = std::stol(arguments[8]);
        n_qubits = n_rotors * n_qubits_per_rotor;
    }

    std::size_t n_rotors;
    std::size_t n_qubits_per_rotor;
    std::size_t n_repetitions;
    std::size_t n_parameters_saveevery;
    std::size_t n_max_iterations;
    std::size_t n_qubits;
    std::filesystem::path abs_input_filepath;
    std::filesystem::path abs_output_dirpath;
    std::string data_filename;
    std::int64_t i_load_parameters;
};

auto random_parameters(std::size_t size) -> std::vector<double>
{
    auto output = std::vector<double> {};
    output.reserve(size);

    auto device = std::random_device {};
    auto seed = device();
    auto prng = std::mt19937 {seed};
    auto dist = std::uniform_real_distribution<double> {0.0, 2.0 * M_PI};

    for (std::size_t i {0}; i < size; ++i) {
        output.emplace_back(dist(prng));
    }

    return output;
}

auto parameter_filename(std::size_t i_iteration) -> std::string
{
    return std::format("parameters{}.dat", i_iteration);
}

void save_parameters(const std::filesystem::path& abs_output_filepath, const std::vector<double>& parameters)
{
    auto outstream = std::ofstream {abs_output_filepath};
    if (!outstream.is_open()) {
        throw std::runtime_error {std::format("ERROR: unable to open '{}'", abs_output_filepath.c_str())};
    }

    outstream << std::scientific << std::setprecision(16);
    for (auto param : parameters) {
        outstream << param << '\n';
    }
}

auto load_parameters(const std::filesystem::path& abs_input_filepath) -> std::vector<double>
{
    auto instream = std::ifstream {abs_input_filepath};
    if (!instream.is_open()) {
        throw std::runtime_error {std::format("ERROR: unable to open '{}'", abs_input_filepath.c_str())};
    }

    auto output = std::vector<double> {};

    std::string line;
    while (std::getline(instream, line)) {
        auto linestream = std::stringstream {line};
        double param;  // NOLINT

        linestream >> param;
        output.emplace_back(param);
    }

    return output;
}

struct ValueBuffer
{
    std::size_t max_values;
    std::vector<double> values;

    void write_and_empty(std::ostream& outstream)
    {
        for (auto value : values) {
            outstream << value << '\n';
        }

        values.clear();
    }
};

struct OptimizationContext
{
    ket::QuantumCircuit circuit;
    std::vector<ket::param::ParameterID> parameter_ids;
    ket::PauliOperator pauli_op;
    std::size_t iteration;
    std::size_t n_qubits;
    ValueBuffer buffer;
    std::ofstream expvalue_outstream;
    std::size_t n_parameters_saveevery;
    std::filesystem::path abs_output_dirpath;
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
        circuit.add_x_gate((args.n_qubits_per_rotor * i) + 1);
    }

    // create the ansatz, and append it to the existing circuit
    auto [n_local, parameter_ids] = ket::n_local(args.n_qubits, {G::RX, G::RY, G::RZ}, {CG::CCX}, Entangle::FULL, args.n_repetitions);
    ket::extend_circuit(circuit, n_local);

    const auto mode = [&]() {
        if (args.i_load_parameters == -1) {
            return std::ios::out;
        } else {
            return std::ios::app;
        }
    }();

    auto outstream = std::ofstream {args.abs_output_dirpath / args.data_filename, mode};
    if (!outstream.is_open()) {
        throw std::ios::failure {std::format("ERROR: unable to open '{}'", args.abs_output_dirpath.c_str())};
    }

    const auto init_iteration = [&]() {
        if (args.i_load_parameters == -1) {
            return std::size_t {0};
        } else {
            return static_cast<std::size_t>(args.i_load_parameters);
        }
    }();

    auto context = OptimizationContext {
        .circuit=std::move(circuit),
        .parameter_ids=parameter_ids,
        .pauli_op=std::move(pauli_op),
        .iteration=init_iteration,
        .n_qubits=args.n_qubits,
        .buffer=ValueBuffer {.max_values=100, .values={}},
        .expvalue_outstream=std::move(outstream),
        .n_parameters_saveevery=args.n_parameters_saveevery,
        .abs_output_dirpath=args.abs_output_dirpath
    };

    const auto n_parameters = context.parameter_ids.size();

    context.expvalue_outstream << std::fixed << std::setprecision(12);

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

        ctx->buffer.values.push_back(exp_value.real());

        if (ctx->buffer.values.size() >= ctx->buffer.max_values) {
            ctx->buffer.write_and_empty(ctx->expvalue_outstream);
        }

        ++ctx->iteration;
        if (ctx->iteration % ctx->n_parameters_saveevery == 0) {
            const auto output_parameter_filepath = ctx->abs_output_dirpath / parameter_filename(ctx->iteration);
            save_parameters(output_parameter_filepath, parameters);
        }

        return exp_value.real();
    };

    // create the initial set of parameters
    auto parameters = [&]() {
        if (args.i_load_parameters == -1) {
            return random_parameters(n_parameters);
        } else {
            const auto i_iteration = static_cast<std::size_t>(args.i_load_parameters);
            const auto input_parameter_filepath = args.abs_output_dirpath / parameter_filename(i_iteration);
            return load_parameters(input_parameter_filepath);
        }
    }();

    // creat the optimization function, and set some parameters to make sure it converges or stops
    auto opt = nlopt::opt {nlopt::LN_COBYLA, static_cast<unsigned int>(n_parameters)};
    opt.set_min_objective(cost_function, &context);
    opt.set_xtol_rel(1.0e-4);
    opt.set_maxeval(static_cast<int>(args.n_max_iterations - init_iteration));
    opt.set_lower_bounds(0.0);
    opt.set_upper_bounds(2.0 * M_PI);
    opt.set_initial_step(0.1);

    double minimum_eigenvalue;  // NOLINT

    [[maybe_unused]]
    const auto result = opt.optimize(parameters, minimum_eigenvalue);

    context.buffer.write_and_empty(context.expvalue_outstream);

    std::cout << "The minimum eigenvalue is " << minimum_eigenvalue << '\n';

    return 0;
}

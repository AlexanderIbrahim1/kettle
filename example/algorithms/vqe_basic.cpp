#include <vector>

#include <nlopt.hpp>

#include <kettle/kettle.hpp>

/*
    This file shows an example of performing noiseless VQE to find the ground state eigenvalue
    of the following operator:

        E = 2 * II  -  2 * XX  +  3 * YY  -  3 * ZZ
    
    REFERENCE: https://learning.quantum.ibm.com/course/variational-algorithm-design/examples-and-applications

    The exact eigenvalues are {-6, 4, 4, 6}, so we should expect a result of approximately -6.
*/

using PT = ket::PauliTerm;
using G = ket::Gate;

struct OptimizationContext
{
    ket::QuantumCircuit circuit;
    std::vector<ket::param::ParameterID> parameter_ids;
    ket::PauliOperator pauli_op;
    std::size_t iteration;
};

auto main() -> int
{
    // this is the Pauli Operator that acts as the cost function
    auto pauli_op = ket::PauliOperator {
        {.coefficient= 2.0, .pauli_string={PT::I, PT::I}},
        {.coefficient=-2.0, .pauli_string={PT::X, PT::X}},
        {.coefficient= 3.0, .pauli_string={PT::Y, PT::Y}},
        {.coefficient=-3.0, .pauli_string={PT::Z, PT::Z}}
    };

    // this is the circuit we use to find the ideal parameters
    auto circuit = ket::QuantumCircuit {2};

    // perform the initial state construction
    circuit.add_x_gate(0);

    // create the ansatz, and append it to the existing circuit
    auto [n_local, parameter_ids] = ket::n_local(2, {G::RZ, G::RY}, {G::CX}, ket::NLocalEntangelement::LINEAR, 1);
    ket::extend_circuit(circuit, n_local);

    auto context = OptimizationContext {
        .circuit=std::move(circuit),
        .parameter_ids=parameter_ids,
        .pauli_op=std::move(pauli_op),
        .iteration=0,
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

        auto statevector = ket::Statevector {"00"};
        ket::simulate(ctx->circuit, statevector);

        const auto exp_value = ket::expectation_value(ctx->pauli_op, statevector);

        std::cout << "exp_value[" << ctx->iteration << "] = " << exp_value.real() << '\n';
        ++(ctx->iteration);

        return exp_value.real();
    };

    // create the initial set of parameters
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

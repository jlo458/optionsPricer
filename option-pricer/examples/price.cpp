#include "pricer/pricer.hpp"

#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct CliArgs {
    pricer::Contract contract{};
    std::string method{"bs"};
    int steps{500};
    std::int64_t paths{100000};
    std::uint64_t seed{42};
};

void print_usage(const char* argv0) {
    std::cerr
        << "Usage: " << argv0 << " [options]\n"
        << "  --type call|put           (default: call)\n"
        << "  --exercise european|american  (default: european)\n"
        << "  --S <spot>                (default: 100)\n"
        << "  --K <strike>              (default: 100)\n"
        << "  --r <rate>                (default: 0.05)\n"
        << "  --q <div yield>           (default: 0)\n"
        << "  --sigma <vol>             (default: 0.2)\n"
        << "  --T <years>               (default: 1)\n"
        << "  --method bs|binomial|mc   (default: bs)\n"
        << "  --steps <n>               binomial steps (default: 500)\n"
        << "  --paths <n>               MC paths (default: 100000)\n"
        << "  --seed <n>                MC seed (default: 42)\n"
        << "  --help\n";
}

double parse_double(const std::string& name, const std::string& value) {
    try {
        std::size_t idx = 0;
        const double x = std::stod(value, &idx);
        if (idx != value.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return x;
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid number for " + name + ": " + value);
    }
}

long long parse_ll(const std::string& name, const std::string& value) {
    try {
        std::size_t idx = 0;
        const long long x = std::stoll(value, &idx);
        if (idx != value.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return x;
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid integer for " + name + ": " + value);
    }
}

CliArgs parse_args(int argc, char** argv) {
    CliArgs args;
    args.contract.S = 100.0;
    args.contract.K = 100.0;
    args.contract.r = 0.05;
    args.contract.q = 0.0;
    args.contract.sigma = 0.2;
    args.contract.T = 1.0;
    args.contract.type = pricer::OptionType::Call;
    args.contract.exercise = pricer::ExerciseStyle::European;

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto need = [&](const char* flag) -> std::string {
            if (i + 1 >= argc) {
                throw std::invalid_argument(std::string("Missing value for ") + flag);
            }
            return argv[++i];
        };

        if (a == "--help" || a == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (a == "--type") {
            args.contract.type = pricer::parse_option_type(need("--type"));
        } else if (a == "--exercise") {
            args.contract.exercise = pricer::parse_exercise(need("--exercise"));
        } else if (a == "--S") {
            args.contract.S = parse_double("--S", need("--S"));
        } else if (a == "--K") {
            args.contract.K = parse_double("--K", need("--K"));
        } else if (a == "--r") {
            args.contract.r = parse_double("--r", need("--r"));
        } else if (a == "--q") {
            args.contract.q = parse_double("--q", need("--q"));
        } else if (a == "--sigma") {
            args.contract.sigma = parse_double("--sigma", need("--sigma"));
        } else if (a == "--T") {
            args.contract.T = parse_double("--T", need("--T"));
        } else if (a == "--method") {
            args.method = need("--method");
        } else if (a == "--steps") {
            args.steps = static_cast<int>(parse_ll("--steps", need("--steps")));
        } else if (a == "--paths") {
            args.paths = parse_ll("--paths", need("--paths"));
        } else if (a == "--seed") {
            const long long s = parse_ll("--seed", need("--seed"));
            if (s < 0) {
                throw std::invalid_argument("Seed must be non-negative");
            }
            args.seed = static_cast<std::uint64_t>(s);
        } else {
            throw std::invalid_argument("Unknown argument: " + a);
        }
    }
    return args;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const CliArgs args = parse_args(argc, argv);
        std::cout << std::setprecision(10) << std::fixed;

        if (args.method == "bs" || args.method == "black-scholes" || args.method == "BSM") {
            const auto result = pricer::black_scholes(args.contract);
            std::cout << "price " << result.price << '\n'
                      << "delta " << result.greeks.delta << '\n'
                      << "gamma " << result.greeks.gamma << '\n'
                      << "vega  " << result.greeks.vega << '\n'
                      << "theta " << result.greeks.theta << '\n'
                      << "rho   " << result.greeks.rho << '\n';
        } else if (args.method == "binomial" || args.method == "crr") {
            const double price = pricer::binomial_crr(args.contract, args.steps);
            std::cout << "price " << price << '\n';
        } else if (args.method == "mc" || args.method == "montecarlo" || args.method == "monte-carlo") {
            const auto result = pricer::monte_carlo(args.contract, args.paths, args.seed);
            std::cout << "price  " << result.price << '\n'
                      << "stderr " << result.stderr << '\n';
        } else {
            throw std::invalid_argument("Unknown method: " + args.method
                                        + " (expected bs, binomial, or mc)");
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }
}

#include "pricer/binomial.hpp"

#include "pricer/math.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace pricer {

double binomial_crr(const Contract& c, int steps) {
    validate_contract(c, /*allow_american=*/true);
    if (steps < 1) {
        throw std::invalid_argument("Binomial steps must be at least 1");
    }

    const double S = c.S;
    const double K = c.K;
    const double r = c.r;
    const double q = c.q;
    const double sigma = c.sigma;
    const double T = c.T;
    const bool is_call = (c.type == OptionType::Call);
    const bool american = (c.exercise == ExerciseStyle::American);

    const double dt = T / static_cast<double>(steps);
    const double u = std::exp(sigma * std::sqrt(dt));
    const double d = 1.0 / u;
    const double disc = std::exp(-r * dt);
    const double a = std::exp((r - q) * dt);
    const double p = (a - d) / (u - d);

    if (!(p > 0.0 && p < 1.0)) {
        throw std::invalid_argument("CRR risk-neutral probability out of (0,1); check parameters");
    }

    std::vector<double> values(static_cast<std::size_t>(steps) + 1);
    for (int j = 0; j <= steps; ++j) {
        const double ST = S * std::pow(u, j) * std::pow(d, steps - j);
        values[static_cast<std::size_t>(j)] = detail::payoff(ST, K, is_call);
    }

    for (int step = steps - 1; step >= 0; --step) {
        for (int j = 0; j <= step; ++j) {
            const double cont = disc * (p * values[static_cast<std::size_t>(j + 1)]
                                        + (1.0 - p) * values[static_cast<std::size_t>(j)]);
            if (american) {
                const double St = S * std::pow(u, j) * std::pow(d, step - j);
                const double exercise = detail::payoff(St, K, is_call);
                values[static_cast<std::size_t>(j)] = std::max(cont, exercise);
            } else {
                values[static_cast<std::size_t>(j)] = cont;
            }
        }
    }

    return values[0];
}

}  // namespace pricer

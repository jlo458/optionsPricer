#include "pricer/monte_carlo.hpp"

#include "pricer/math.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace pricer {

McResult monte_carlo(const Contract& c, std::int64_t paths, std::uint64_t seed) {
    validate_contract(c, /*allow_american=*/false);
    if (c.exercise != ExerciseStyle::European) {
        throw std::invalid_argument("Monte Carlo supports European exercise only");
    }
    if (paths < 1) {
        throw std::invalid_argument("Monte Carlo paths must be at least 1");
    }

    const double S = c.S;
    const double K = c.K;
    const double r = c.r;
    const double q = c.q;
    const double sigma = c.sigma;
    const double T = c.T;
    const bool is_call = (c.type == OptionType::Call);

    const double drift = (r - q - 0.5 * sigma * sigma) * T;
    const double vol = sigma * std::sqrt(T);
    const double disc = std::exp(-r * T);

    std::mt19937_64 rng(seed);
    std::normal_distribution<double> Z(0.0, 1.0);

    double sum = 0.0;
    double sum_sq = 0.0;

    for (std::int64_t i = 0; i < paths; ++i) {
        const double ST = S * std::exp(drift + vol * Z(rng));
        const double pv = disc * detail::payoff(ST, K, is_call);
        sum += pv;
        sum_sq += pv * pv;
    }

    const double n = static_cast<double>(paths);
    const double mean = sum / n;
    // Unbiased sample variance; for paths == 1 report se = 0.
    double se = 0.0;
    if (paths > 1) {
        const double variance = (sum_sq - sum * sum / n) / (n - 1.0);
        se = std::sqrt(std::max(variance, 0.0) / n);
    }

    return McResult{mean, se};
}

}  // namespace pricer

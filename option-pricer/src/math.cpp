#include "pricer/math.hpp"

#include <algorithm>
#include <cmath>

namespace pricer {
namespace detail {

double norm_pdf(double x) {
    static constexpr double inv_sqrt_2pi = 0.398942280401432677939946;
    return inv_sqrt_2pi * std::exp(-0.5 * x * x);
}

double norm_cdf(double x) {
    // Φ(x) = ½ erfc(-x / √2); uses the C++17 standard library, not Boost.Math.
    return 0.5 * std::erfc(-x * std::sqrt(0.5));
}

double payoff(double ST, double K, bool is_call) {
    if (is_call) {
        return std::max(ST - K, 0.0);
    }
    return std::max(K - ST, 0.0);
}

}  // namespace detail
}  // namespace pricer

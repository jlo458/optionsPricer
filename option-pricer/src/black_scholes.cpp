#include "pricer/black_scholes.hpp"

#include "pricer/math.hpp"

#include <cmath>
#include <stdexcept>

namespace pricer {

BsResult black_scholes(const Contract& c) {
    validate_contract(c, /*allow_american=*/false);
    if (c.exercise != ExerciseStyle::European) {
        throw std::invalid_argument("Black–Scholes supports European exercise only");
    }

    const double S = c.S;
    const double K = c.K;
    const double r = c.r;
    const double q = c.q;
    const double sigma = c.sigma;
    const double T = c.T;
    const bool is_call = (c.type == OptionType::Call);

    const double sqrtT = std::sqrt(T);
    const double disc_r = std::exp(-r * T);
    const double disc_q = std::exp(-q * T);

    const double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * sqrtT);
    const double d2 = d1 - sigma * sqrtT;

    const double Nd1 = detail::norm_cdf(d1);
    const double Nd2 = detail::norm_cdf(d2);
    const double Nmd1 = detail::norm_cdf(-d1);
    const double Nmd2 = detail::norm_cdf(-d2);
    const double pdf_d1 = detail::norm_pdf(d1);

    BsResult out;
    if (is_call) {
        out.price = S * disc_q * Nd1 - K * disc_r * Nd2;
        out.greeks.delta = disc_q * Nd1;
        out.greeks.theta = -S * disc_q * pdf_d1 * sigma / (2.0 * sqrtT)
                           + q * S * disc_q * Nd1
                           - r * K * disc_r * Nd2;
        out.greeks.rho = K * T * disc_r * Nd2;
    } else {
        out.price = K * disc_r * Nmd2 - S * disc_q * Nmd1;
        out.greeks.delta = -disc_q * Nmd1;
        out.greeks.theta = -S * disc_q * pdf_d1 * sigma / (2.0 * sqrtT)
                           - q * S * disc_q * Nmd1
                           + r * K * disc_r * Nmd2;
        out.greeks.rho = -K * T * disc_r * Nmd2;
    }

    out.greeks.gamma = disc_q * pdf_d1 / (S * sigma * sqrtT);
    // Vega is ∂V/∂σ (not divided by 100).
    out.greeks.vega = S * disc_q * pdf_d1 * sqrtT;

    return out;
}

double black_scholes_price(const Contract& c) {
    return black_scholes(c).price;
}

}  // namespace pricer

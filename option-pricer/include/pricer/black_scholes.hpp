#pragma once

#include "pricer/types.hpp"

namespace pricer {

struct Greeks {
    double delta{0.0};
    double gamma{0.0};
    double vega{0.0};
    double theta{0.0};  // calendar time: ∂V/∂t with t in years (negative for long vanilla)
    double rho{0.0};
};

struct BsResult {
    double price{0.0};
    Greeks greeks{};
};

// Black–Scholes–Merton closed form for European options with continuous yield q.
BsResult black_scholes(const Contract& c);
double black_scholes_price(const Contract& c);

}  // namespace pricer

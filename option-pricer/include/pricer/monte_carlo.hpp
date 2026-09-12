#pragma once

#include "pricer/types.hpp"

#include <cstdint>

namespace pricer {

struct McResult {
    double price{0.0};
    double stderr{0.0};  // standard error of the mean
};

// Risk-neutral Monte Carlo under GBM for European options only.
McResult monte_carlo(const Contract& c, std::int64_t paths, std::uint64_t seed);

}  // namespace pricer

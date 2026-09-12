#pragma once

#include "pricer/types.hpp"

namespace pricer {

// Cox–Ross–Rubinstein binomial tree for European and American vanillas.
double binomial_crr(const Contract& c, int steps);

}  // namespace pricer

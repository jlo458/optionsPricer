#pragma once

namespace pricer {
namespace detail {

double norm_pdf(double x);
double norm_cdf(double x);
double payoff(double ST, double K, bool is_call);

}  // namespace detail
}  // namespace pricer

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "pricer/binomial.hpp"
#include "pricer/black_scholes.hpp"

#include <cmath>

using Catch::Matchers::WithinAbs;

namespace {

pricer::Contract atm_euro_call() {
    pricer::Contract c;
    c.S = 100.0;
    c.K = 100.0;
    c.r = 0.05;
    c.q = 0.0;
    c.sigma = 0.2;
    c.T = 1.0;
    c.type = pricer::OptionType::Call;
    c.exercise = pricer::ExerciseStyle::European;
    return c;
}

}  // namespace

TEST_CASE("Binomial European put-call parity", "[binomial][parity]") {
    auto call = atm_euro_call();
    auto put = call;
    put.type = pricer::OptionType::Put;

    constexpr int steps = 500;
    const double C = pricer::binomial_crr(call, steps);
    const double P = pricer::binomial_crr(put, steps);
    const double rhs = call.S * std::exp(-call.q * call.T) - call.K * std::exp(-call.r * call.T);

    // Tree discretisation leaves a small residual; still tight for n=500.
    REQUIRE_THAT(C - P, WithinAbs(rhs, 1e-6));
}

TEST_CASE("European binomial converges toward BS", "[binomial][convergence]") {
    auto c = atm_euro_call();
    const double bs = pricer::black_scholes_price(c);

    const double p50 = pricer::binomial_crr(c, 50);
    const double p400 = pricer::binomial_crr(c, 400);
    const double p1000 = pricer::binomial_crr(c, 1000);

    const double e50 = std::abs(p50 - bs);
    const double e400 = std::abs(p400 - bs);
    const double e1000 = std::abs(p1000 - bs);

    // Documented gaps (order 1/n): do not fake 1e-12 agreement.
    REQUIRE(e50 > 1e-4);
    REQUIRE(e50 < 0.05);
    REQUIRE(e400 < e50);
    REQUIRE(e1000 < e400);
    REQUIRE(e1000 < 5e-3);
}

TEST_CASE("American call with q=0 equals European call", "[binomial][american]") {
    auto euro = atm_euro_call();
    auto amer = euro;
    amer.exercise = pricer::ExerciseStyle::American;

    constexpr int steps = 400;
    const double pe = pricer::binomial_crr(euro, steps);
    const double pa = pricer::binomial_crr(amer, steps);
    REQUIRE_THAT(pa, WithinAbs(pe, 1e-12));
}

TEST_CASE("American put is at least European put", "[binomial][american]") {
    auto euro = atm_euro_call();
    euro.type = pricer::OptionType::Put;
    auto amer = euro;
    amer.exercise = pricer::ExerciseStyle::American;

    constexpr int steps = 400;
    const double pe = pricer::binomial_crr(euro, steps);
    const double pa = pricer::binomial_crr(amer, steps);
    REQUIRE(pa >= pe - 1e-12);
    // With rates > 0, early exercise premium is typically strictly positive.
    REQUIRE(pa > pe);
}

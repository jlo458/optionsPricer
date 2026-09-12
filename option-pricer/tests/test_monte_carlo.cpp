#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "pricer/black_scholes.hpp"
#include "pricer/monte_carlo.hpp"

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

TEST_CASE("MC error vs BS falls as paths increase; stderr shrinks", "[mc]") {
    auto c = atm_euro_call();
    const double bs = pricer::black_scholes_price(c);

    constexpr std::uint64_t seed = 42;
    const auto mc10k = pricer::monte_carlo(c, 10'000, seed);
    const auto mc100k = pricer::monte_carlo(c, 100'000, seed);

    REQUIRE(mc10k.stderr > 0.0);
    REQUIRE(mc100k.stderr > 0.0);
    REQUIRE(mc100k.stderr < mc10k.stderr);

    // Standard error should scale roughly like 1/sqrt(N): 10x paths => ~sqrt(10)~3.16x smaller.
    const double se_ratio = mc10k.stderr / mc100k.stderr;
    REQUIRE(se_ratio > 2.5);
    REQUIRE(se_ratio < 4.0);

    const double err10k = std::abs(mc10k.price - bs);
    const double err100k = std::abs(mc100k.price - bs);

    // Fixed seed 42: absolute error vs BS falls as paths grow (order 1/sqrt(N) in expectation).
    REQUIRE(err100k < err10k);
    REQUIRE(err10k < 5.0 * mc10k.stderr + 0.05);
    REQUIRE(err100k < 5.0 * mc100k.stderr + 0.02);
}

TEST_CASE("MC rejects American", "[mc][validation]") {
    auto c = atm_euro_call();
    c.exercise = pricer::ExerciseStyle::American;
    REQUIRE_THROWS_AS(pricer::monte_carlo(c, 1000, 1), std::invalid_argument);
}

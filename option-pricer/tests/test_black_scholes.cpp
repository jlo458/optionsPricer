#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "pricer/black_scholes.hpp"

#include <cmath>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

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

TEST_CASE("BS put-call parity", "[bs][parity]") {
    auto call = atm_euro_call();
    auto put = call;
    put.type = pricer::OptionType::Put;

    const double C = pricer::black_scholes_price(call);
    const double P = pricer::black_scholes_price(put);
    const double rhs = call.S * std::exp(-call.q * call.T) - call.K * std::exp(-call.r * call.T);

    REQUIRE_THAT(C - P, WithinAbs(rhs, 1e-10));
}

TEST_CASE("BS call delta matches bump-and-revalue", "[bs][greeks]") {
    auto c = atm_euro_call();
    const auto greeks = pricer::black_scholes(c).greeks;

    const double h = 1e-4;
    auto up = c;
    auto dn = c;
    up.S = c.S + h;
    dn.S = c.S - h;
    const double bump_delta =
        (pricer::black_scholes_price(up) - pricer::black_scholes_price(dn)) / (2.0 * h);

    REQUIRE_THAT(greeks.delta, WithinAbs(bump_delta, 1e-6));
}

TEST_CASE("BS deep ITM/OTM call limits", "[bs][limits]") {
    auto c = atm_euro_call();

    SECTION("as K -> 0, call -> S e^{-qT}") {
        c.K = 1e-8;
        const double price = pricer::black_scholes_price(c);
        const double limit = c.S * std::exp(-c.q * c.T);
        REQUIRE_THAT(price, WithinRel(limit, 1e-6));
    }

    SECTION("as K -> infinity, call -> 0") {
        c.K = 1e8;
        const double price = pricer::black_scholes_price(c);
        REQUIRE_THAT(price, WithinAbs(0.0, 1e-8));
    }

    SECTION("as K -> 0, put -> 0") {
        c.type = pricer::OptionType::Put;
        c.K = 1e-8;
        const double price = pricer::black_scholes_price(c);
        REQUIRE_THAT(price, WithinAbs(0.0, 1e-6));
    }

    SECTION("as K -> infinity, put ~ K e^{-rT} - S e^{-qT}") {
        c.type = pricer::OptionType::Put;
        c.K = 1e6;
        const double price = pricer::black_scholes_price(c);
        const double limit = c.K * std::exp(-c.r * c.T) - c.S * std::exp(-c.q * c.T);
        REQUIRE_THAT(price, WithinRel(limit, 1e-6));
    }
}

TEST_CASE("BS rejects American", "[bs][validation]") {
    auto c = atm_euro_call();
    c.exercise = pricer::ExerciseStyle::American;
    REQUIRE_THROWS_AS(pricer::black_scholes(c), std::invalid_argument);
}

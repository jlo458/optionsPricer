#include <catch2/catch_test_macros.hpp>

#include "pricer/binomial.hpp"
#include "pricer/black_scholes.hpp"
#include "pricer/monte_carlo.hpp"

namespace {

pricer::Contract base() {
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

TEST_CASE("Invalid inputs throw clear errors", "[validation]") {
    SECTION("sigma <= 0") {
        auto c = base();
        c.sigma = 0.0;
        REQUIRE_THROWS_AS(pricer::black_scholes(c), std::invalid_argument);
        REQUIRE_THROWS_AS(pricer::binomial_crr(c, 10), std::invalid_argument);
        REQUIRE_THROWS_AS(pricer::monte_carlo(c, 100, 1), std::invalid_argument);
    }

    SECTION("T <= 0") {
        auto c = base();
        c.T = 0.0;
        REQUIRE_THROWS_AS(pricer::black_scholes(c), std::invalid_argument);
    }

    SECTION("S <= 0") {
        auto c = base();
        c.S = -1.0;
        REQUIRE_THROWS_AS(pricer::black_scholes(c), std::invalid_argument);
    }

    SECTION("K <= 0") {
        auto c = base();
        c.K = 0.0;
        REQUIRE_THROWS_AS(pricer::black_scholes(c), std::invalid_argument);
    }

    SECTION("American + MC rejected") {
        auto c = base();
        c.exercise = pricer::ExerciseStyle::American;
        REQUIRE_THROWS_AS(pricer::monte_carlo(c, 1000, 7), std::invalid_argument);
    }

    SECTION("binomial steps < 1") {
        auto c = base();
        REQUIRE_THROWS_AS(pricer::binomial_crr(c, 0), std::invalid_argument);
    }

    SECTION("MC paths < 1") {
        auto c = base();
        REQUIRE_THROWS_AS(pricer::monte_carlo(c, 0, 1), std::invalid_argument);
    }
}

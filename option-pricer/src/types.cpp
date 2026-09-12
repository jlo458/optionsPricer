#include "pricer/types.hpp"

namespace pricer {

void validate_contract(const Contract& c, bool allow_american) {
    if (!(c.S > 0.0)) {
        throw std::invalid_argument("Spot S must be positive");
    }
    if (!(c.K > 0.0)) {
        throw std::invalid_argument("Strike K must be positive");
    }
    if (!(c.sigma > 0.0)) {
        throw std::invalid_argument("Volatility sigma must be positive");
    }
    if (!(c.T > 0.0)) {
        throw std::invalid_argument("Time to expiry T must be positive");
    }
    if (!allow_american && c.exercise == ExerciseStyle::American) {
        throw std::invalid_argument("American exercise is not supported by this method");
    }
}

OptionType parse_option_type(const std::string& s) {
    if (s == "call" || s == "Call" || s == "CALL") {
        return OptionType::Call;
    }
    if (s == "put" || s == "Put" || s == "PUT") {
        return OptionType::Put;
    }
    throw std::invalid_argument("Option type must be 'call' or 'put'");
}

ExerciseStyle parse_exercise(const std::string& s) {
    if (s == "european" || s == "European" || s == "EUROPEAN") {
        return ExerciseStyle::European;
    }
    if (s == "american" || s == "American" || s == "AMERICAN") {
        return ExerciseStyle::American;
    }
    throw std::invalid_argument("Exercise must be 'european' or 'american'");
}

}  // namespace pricer

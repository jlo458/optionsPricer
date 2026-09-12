#pragma once

#include <stdexcept>
#include <string>

namespace pricer {

enum class OptionType { Call, Put };
enum class ExerciseStyle { European, American };

struct Contract {
    double S{0.0};      // spot
    double K{0.0};      // strike
    double r{0.0};      // continuously compounded risk-free rate
    double q{0.0};      // continuous dividend yield
    double sigma{0.0};  // volatility
    double T{0.0};      // time to expiry in years
    OptionType type{OptionType::Call};
    ExerciseStyle exercise{ExerciseStyle::European};
};

void validate_contract(const Contract& c, bool allow_american = true);

OptionType parse_option_type(const std::string& s);
ExerciseStyle parse_exercise(const std::string& s);

}  // namespace pricer

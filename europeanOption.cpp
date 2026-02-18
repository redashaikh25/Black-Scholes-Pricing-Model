
#include "EuropeanOption.h"
#include "NormalDistribution.h" 
#include <cmath>

// This source file implements the EuropeanOption class.


// DEfault constructor sets default parameters for a standard European option
EuropeanOption::EuropeanOption()
{
    r = 0.05;          // Default risk-free rate, common for examples
    sig = 0.2;          // Default volatility
    K = 100.0;          // Default strike
    T = 1.0;            // Default time to maturity = 1 year
    b = r;              // Default cost of carry = r for standard BS model
    optType = "C";      // Default option type = Call
}

// Allows creating a EuropeanOption with specific type while inheriting defaults
EuropeanOption::EuropeanOption(const std::string& optionType)
    : EuropeanOption() // Delegate to default constructor for parameter defaults
{
    optType = optionType; // Override type
}

// Implements standard Black-Scholes formula for European calls
// d1 and d2 are temporary terms for efficiency and readability
double EuropeanOption::CallPrice(double S) const
{
    double tmp = sig * std::sqrt(T); // sqrt(T) factor reused in d1/d2
    double d1 = (std::log(S / K) + (b + 0.5 * sig * sig) * T) / tmp;
    double d2 = d1 - tmp; // d2 = d1 - sigma*sqrt(T)

    // Discounted expected payoff formula for European call
    // S * exp((b-r)*T) adjusts for cost of carry (dividends or other carry costs)
    return S * std::exp((b - r) * T) * N_cdf(d1) - K * std::exp(-r * T) * N_cdf(d2);
}

// Implements Black-Scholes formula for European puts
// Reuses d1/d2 to maintain efficiency and consistency with call formula
double EuropeanOption::PutPrice(double S) const
{
    double tmp = sig * std::sqrt(T);
    double d1 = (std::log(S / K) + (b + 0.5 * sig * sig) * T) / tmp;
    double d2 = d1 - tmp;

    // Put formula is based on call-put symmetry
    return K * std::exp(-r * T) * N_cdf(-d2) - S * std::exp((b - r) * T) * N_cdf(-d1);
}


// Decides internally whether to call CallPrice or PutPrice
// Allows polymorphic usage where external code calls Price() without worrying about option type for AmericanOption
double EuropeanOption::Price(double S) const
{
    if (optType == "C") return CallPrice(S);
    else return PutPrice(S);
}

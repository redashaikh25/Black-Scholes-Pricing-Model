
#include "AmericanOption.h"
#include <cmath>


// Default constructor calls EuropeanOption default constructor to inherit standard parameters
AmericanOption::AmericanOption() : EuropeanOption() {}

// Option type constructor: allows user to specify "C" or "P" and calls the corresponding EuropeanOption constructor to initialize common parameters
AmericanOption::AmericanOption(const std::string& optionType)
    : EuropeanOption(optionType) {
}

// Implements closed-form formula for a perpetual American call option
double AmericanOption::CallPriceAmerican(double S) const
{
    // Precompute sigma squared to avoid repeated multiplication
    double sig2 = sig * sig;

    // b_sig is the normalized cost-of-carry parameter
    double b_sig = b / sig2;

    // Compute the sqrt term in the y1 formula
    // This comes directly from the perpetual American option solution
    double sqrtTerm = sqrt(pow((b / sig2 - 0.5), 2) + 2 * r / sig2);

    // y1 exponent determines the shape of the solution
    double y1 = 0.5 - (b / sig2) + sqrtTerm;

    // The formula only makes sense for y1 > 1
    // If y1 <= 1, option is effectively worthless (cannot exercise early profitably)
    return (y1 > 1.0)
        ? (K / (y1 - 1.0)) * pow(((y1 - 1.0) / y1 * S / K), y1)
        : 0.0;
}

// Implements closed-form formula for a perpetual American put option
double AmericanOption::PutPriceAmerican(double S) const
{
    double sig2 = sig * sig;

    // sqrt term for y2, similar to y1 but with negative root for put
    double sqrtTerm = sqrt(pow((b / sig2 - 0.5), 2) + 2 * r / sig2);

    // y2 exponent determines the shape of the put solution
    double y2 = 0.5 - (b / sig2) - sqrtTerm;

    // Only makes sense for y2 < 0
    // Otherwise, the put cannot be exercised early profitably
    return (y2 < 0.0)
        ? (K / (1.0 - y2)) * pow(((y2 - 1.0) / y2 * S / K), y2)
        : 0.0;
}

// Polymorphic implementation of external code calls Price() without knowing the option type automatically chooses call or put perpetual formula based on optType
double AmericanOption::Price(double S) const
{
    if (optType == "C")
        return CallPriceAmerican(S); // Call formula
    else
        return PutPriceAmerican(S);  // Put formula
}

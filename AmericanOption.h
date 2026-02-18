
#ifndef AMERICANOPTION_H
#define AMERICANOPTION_H

#include "EuropeanOption.h" // Inherit from EuropeanOption for parameter reuse
#include <string>
#include <cmath>

// This class represents Perpetual American Options.
// It inherits from EuropeanOption to reuse common parameters (K, r, sig, etc.) and basic interface (Price(), toggle()).
// Perpetual American options can be exercised at any time and have closed-form solutions in the Black-Scholes framework for infinite maturity.

class AmericanOption : public EuropeanOption
{
private:
    // Private helper functions implement the closed-form formulas for perpetual American calls/puts
    double CallPriceAmerican(double S) const;
    double PutPriceAmerican(double S) const;

public:
    // Default constructor initializes parameters with typical defaults
    AmericanOption();

    // Option type constructor allows specifying "C" or "P"
    AmericanOption(const std::string& optionType);

    // Overrides the EuropeanOption::Price() function and calls the appropriate perpetual American pricing formula
    // This allows polymorphic usage where external code calls Price() without knowing the option type
    double Price(double S) const override;
};

#endif

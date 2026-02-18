
#ifndef GREEKS_H
#define GREEKS_H

#include "EuropeanOption.h"



// We use enums for GreekType and NumericGreekType to improve code readability
// It enables the possibility of future extensions where Greeks can be selected dynamically

enum class GreekType { Delta, Gamma, Vega, Theta, Rho };

// This is for numerical approximations of Greeks based ons tep size h.
enum class NumericGreekType { DeltaFD, GammaFD };

class Greeks
{
public:

    // These functions compute closed-form Black-Scholes Greeks.
    // They rely on the mathematical properties of the European option pricing formula s
   
    // All functions take const EuropeanOption& opt : ensures option parameters are read-only and not modified.
    // and double S : explicit spot price input, allowing vectorized evaluation across meshes of S values.

    static double Delta(const EuropeanOption& opt, double S);
    static double Gamma(const EuropeanOption& opt, double S);
    static double Vega(const EuropeanOption& opt, double S);
    static double Theta(const EuropeanOption& opt, double S);
    static double Rho(const EuropeanOption& opt, double S);

    // These functions approximate Greeks numerically using central finite differences.
	// The step size h is passed explicitly to study behavior as h varies.

    static double DeltaFD(const EuropeanOption& opt, double S, double h);
    static double GammaFD(const EuropeanOption& opt, double S, double h);
};

#endif

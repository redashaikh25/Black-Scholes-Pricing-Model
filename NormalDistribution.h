// Plain European Option Pricing with Black-Scholes

#ifndef NORMALDISTRIBUTION_H
#define NORMALDISTRIBUTION_H

#include <boost/math/distributions/normal.hpp>

// We are using the Boost library for the normal distribution functions (CDF and PDF) 
// instead of implementing approximate formulas ourselves. Boost provides highly precise cumulative and probability density calculations, and
// avoids reinventing standard statistical functions.

// Using inline functions ensures minimal overhead because compiler can expand them directly at the call site, avoiding function call overhead.

// ---------------- Cumulative Normal Distribution ----------------
// Computes the cumulative distribution function (CDF) for a standard normal variable
inline double N_cdf(double x)
{
    // We use a static const object for the standard normal distribution; 
    // ensures that the object is only constructed once for the lifetime of the program
    static const boost::math::normal norm(0.0, 1.0); // mean = 0, standard deviation = 1
    return boost::math::cdf(norm, x); // returns P(Z <= x), the cumulative probability up to x
}


// ---------------- Probability Density Function ----------------
// Computes the probability density function (PDF) for a standard normal variable
inline double N_pdf(double x)
{
    static const boost::math::normal norm(0.0, 1.0); // mean = 0, standard deviation = 1
    return boost::math::pdf(norm, x); // returns the density f(x) at point x
}


#endif

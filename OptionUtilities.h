// Utility functions for put-call parity and conversions

#ifndef OPTIONUTILITIES_H
#define OPTIONUTILITIES_H

#include <vector>
#include <cmath>
#include "EuropeanOption.h"

// Computes the Put price from a given Call price using the standard put-call parity formula: P = C - S + K * e^(-r*T)
// Uses inline function and and one line caclulation for efficiency

inline double PutFromCall(double C, double S, double K, double r, double T)
{
    return C - S + K * std::exp(-r * T);
}

// Computes the Call price from a given Put price using parity: C = P + S - K * e^(-r*T)
inline double CallFromPut(double P, double S, double K, double r, double T)
{
    return P + S - K * std::exp(-r * T);
}


// Checks whether put-call parity holds within a tolerance.
// simple implementation returns a boolean for main code usage

inline bool CheckParity(double C, double P, double S, double K, double r, double T, double tol = 1e-6)
{
    double left = C - P;
    double right = S - K * std::exp(-r * T);
    return std::fabs(left - right) < tol;
}

// Computes the absolute error in put-call parity, generalized to include cost-of-carry b
// The function returns a double so we can quantify the exact deviation and is more flexible than a boolean
// includes b parameter: lhs = C - P, rhs = S * exp((b-r)T) - K * exp(-rT)

inline double parityError(double C, double P,
    double S, double K,
    double r, double b, double T)
{
    double lhs = C - P;
    double rhs = S * std::exp((b - r) * T) - K * std::exp(-r * T);
    return std::fabs(lhs - rhs);
}

#endif

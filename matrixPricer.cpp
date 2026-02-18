// Implements vectorized and matrix-based option pricing and Greek computations

#include "MatrixPricer.h"
#include <vector>

// Computes a vector of outputs (price or Greek) across a range of spot prices
// using a simple range-based for loop over S values 

std::vector<double> MatrixPricer::Vector(EuropeanOption& opt,
    const std::vector<double>& S_values,
    OutputType output,
    double h)
{
    std::vector<double> result; // Will store the computed vector
    result.reserve(S_values.size()); 

    for (double S : S_values)
    {
        switch (output)
        {
        case OutputType::Price:     result.push_back(opt.Price(S)); break;      // Exact option price
        case OutputType::Delta:     result.push_back(Greeks::Delta(opt, S)); break; // Exact Delta
        case OutputType::Gamma:     result.push_back(Greeks::Gamma(opt, S)); break; // Exact Gamma
        case OutputType::Vega:      result.push_back(Greeks::Vega(opt, S)); break;  // Exact Vega
        case OutputType::Theta:     result.push_back(Greeks::Theta(opt, S)); break; // Exact Theta
        case OutputType::Rho:       result.push_back(Greeks::Rho(opt, S)); break;   // Exact Rho
        case OutputType::DeltaFD:   result.push_back(Greeks::DeltaFD(opt, S, h)); break; // Numerical Delta
        case OutputType::GammaFD:   result.push_back(Greeks::GammaFD(opt, S, h)); break; // Numerical Gamma
        }
    }

    return result; // Return the full vector
}

// Computes a surface of outputs for multiple sets of option parameters and spot prices.
// Outer loop iterates over parameter sets; inner computation uses Vector() over S values.
// This produces a vector-of-vectors (surface) representing Price/Greek vs Spot Price and Parameter Sweep

std::vector<std::vector<double>> MatrixPricer::Matrix(EuropeanOption& opt,
    const std::vector<std::vector<double>>& paramMatrix,
    const std::vector<double>& S_values,
    OutputType output,
    double h)
{
    std::vector<std::vector<double>> surface;
    surface.reserve(paramMatrix.size());

    for (const auto& p : paramMatrix)
    {
        // Map paramMatrix row to option object
        opt.T = p[0];              // Maturity
        opt.K = p[1];              // Strike
        opt.sig = p[2];            // Volatility
        opt.r = p[3];              // Risk-free rate
        opt.b = (p.size() > 4 ? p[4] : opt.r); // Cost-of-carry, default to r if not provided

        // Use the Vector method to compute values for this parameter set
        surface.push_back(Vector(opt, S_values, output, h));
    }

    return surface; // Return the full surface
}

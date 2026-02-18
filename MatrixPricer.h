// Provides vectorized and matrix-based pricing of European options and Greeks using either exact formulas or finite-difference approximations

#ifndef MATRIXPRICER_H
#define MATRIXPRICER_H

#include <vector>
#include "EuropeanOption.h"
#include "Greeks.h"


// Using an enum class for type safety and clear semantic meaning.
enum class OutputType
{
    Price,      // Option price
    Delta,      // Exact Delta
    Gamma,      // Exact Gamma
    Vega,       // Exact Vega
    Theta,      // Exact Theta
    Rho,        // Exact Rho
    DeltaFD,    // Finite-difference Delta
    GammaFD     // Finite-difference Gamma
};

// Provides static methods for computing option values and Greeks across vectors or matrices 

class MatrixPricer
{
public:

    // Computes a vector of outputs (price or Greek) for each spot price S, taking any form of parameter and vector of spot prices to loop over
    // h, finite step is defined for use in DeltaFD and GammaFD

    static std::vector<double> Vector(EuropeanOption& opt,
        const std::vector<double>& S_values,
        OutputType output,
        double h = 0.01);

    // Computes a vector of vectors (matrix) of outputs for a set of parameter vectors and spot prices
    // Each row of paramMatrix contains a set of option parameters like T, K, sig, and r
	// Two loops are used: outer loop over parameter sets, inner loop over spot prices
   
    static std::vector<std::vector<double>> Matrix(EuropeanOption& opt,
        const std::vector<std::vector<double>>& paramMatrix,
        const std::vector<double>& S_values,
        OutputType output,
        double h = 0.01);
};

#endif

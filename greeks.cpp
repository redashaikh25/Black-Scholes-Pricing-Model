
#include "Greeks.h"
#include "NormalDistribution.h"  
#include <cmath>


// Delta measures sensitivity of option price to changes in spot price S.
// Mathematically: Delta = ∂V / ∂S
double Greeks::Delta(const EuropeanOption& opt, double S)
{
    double tmp = opt.sig * std::sqrt(opt.T); // σ√T reused across Greeks
    //d1 recomputed for clarity and prevent function dependency
    double d1 = 
        (std::log(S / opt.K)  
        + (opt.b + 0.5 * opt.sig * opt.sig) * opt.T) / tmp;

    // This conditional keeps both cases in one function
    if (opt.optType == "C")
        return std::exp((opt.b - opt.r) * opt.T) * N_cdf(d1);
    else
        return std::exp((opt.b - opt.r) * opt.T) * (N_cdf(d1) - 1.0);
}


// Gamma measures curvature of the option value with respect to S.
// Mathematically: Gamma = ∂²V / ∂S²
// Gamma is identical for calls and puts in Black-Scholes,
double Greeks::Gamma(const EuropeanOption& opt, double S)
{
    double tmp = opt.sig * std::sqrt(opt.T);
    double d1 = (std::log(S / opt.K)
        + (opt.b + 0.5 * opt.sig * opt.sig) * opt.T) / tmp;

    return std::exp((opt.b - opt.r) * opt.T) * N_pdf(d1) / (S * tmp);
}

// Vega measures sensitivity of option value to volatility σ.
// Mathematically: Vega = ∂V / ∂σ
double Greeks::Vega(const EuropeanOption& opt, double S)
{
    double tmp = opt.sig * std::sqrt(opt.T);
    double d1 = (std::log(S / opt.K)
        + (opt.b + 0.5 * opt.sig * opt.sig) * opt.T) / tmp;

    return S * std::exp((opt.b - opt.r) * opt.T)
        * N_pdf(d1) * std::sqrt(opt.T);
}

// ------------------------------------------------------------
// EXACT THETA
// ------------------------------------------------------------
//
// Theta measures sensitivity of option value to time decay.
// Mathematically: Theta = ∂V / ∂T
// We use explicit branching for calls and puts to improve readability and correctness
double Greeks::Theta(const EuropeanOption& opt, double S)
{
    double tmp = opt.sig * std::sqrt(opt.T);
    double d1 = (std::log(S / opt.K)
        + (opt.b + 0.5 * opt.sig * opt.sig) * opt.T) / tmp;
    double d2 = d1 - tmp;

    if (opt.optType == "C") //Call
        return
        // Volatility-driven decay
        -(S * opt.sig * std::exp((opt.b - opt.r) * opt.T) * N_pdf(d1))
        / (2 * std::sqrt(opt.T))

        // Cost-of-carry term
        - (opt.b - opt.r) * S * std::exp((opt.b - opt.r) * opt.T) * N_cdf(d1)

        // Discounted strike term
        - opt.r * opt.K * std::exp(-opt.r * opt.T) * N_cdf(d2);
    else
        return
        //Put
        -(S * opt.sig * std::exp((opt.b - opt.r) * opt.T) * N_pdf(d1))
        / (2 * std::sqrt(opt.T))
        + (opt.b - opt.r) * S * std::exp((opt.b - opt.r) * opt.T) * N_cdf(-d1)
        + opt.r * opt.K * std::exp(-opt.r * opt.T) * N_cdf(-d2);
}


// Rho measures sensitivity to the risk-free rate r.
// Mathematically: Rho = ∂V / ∂r
double Greeks::Rho(const EuropeanOption& opt, double S)
{
    double tmp = opt.sig * std::sqrt(opt.T);
    double d1 = (std::log(S / opt.K)
        + (opt.b + 0.5 * opt.sig * opt.sig) * opt.T) / tmp;
    double d2 = d1 - tmp;

    if (opt.optType == "C")
        return opt.K * opt.T * std::exp(-opt.r * opt.T) * N_cdf(d2);
    else
        return -opt.K * opt.T * std::exp(-opt.r * opt.T) * N_cdf(-d2);
}


// Numerical approximation of Delta using central differences.
// Used to validate exact Delta and study convergence as h approaches 0.
double Greeks::DeltaFD(const EuropeanOption& opt, double S, double h)
{
    return (opt.Price(S + h) - opt.Price(S - h)) / (2.0 * h);
}


// Numerical approximation of Gamma
double Greeks::GammaFD(const EuropeanOption& opt, double S, double h)
{
    return (opt.Price(S + h)
        - 2.0 * opt.Price(S)
        + opt.Price(S - h)) / (h * h);
}

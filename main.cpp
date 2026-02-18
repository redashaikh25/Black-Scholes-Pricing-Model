//Option Pricing with Black-Scholes

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

#include "EuropeanOption.h"    // EuropeanOption class: for plain vanilla call/put pricing
#include "MeshGenerator.h"     // MeshGenerator: builds spot price vectors for vectorized pricing
#include "MatrixPricer.h"      // MatrixPricer: compute prices/Greeks over a matrix of parameters
#include "Greeks.h"            // Greeks: compute Delta, Gamma (exact & finite difference)
#include "OptionUtilities.h"   // Put-Call parity functions
#include "AmericanOption.h"    // Perpetual American options

using namespace std;

int main()
{
    // Fixed decimal precision ensures outputs are readable and comparable across batches
    cout << fixed << setprecision(6);

    cout << "\n----------------------------------------\n";
    cout << "        Simple Option Pricing\n";
    cout << "----------------------------------------\n";

    // ---------------- Exact Call and Put Prices ----------------
    cout << "\nExact Black-Scholes Prices\n";

    EuropeanOption optE; // Default European option object. We'll overwrite parameters dynamically.

    // Batch struct groups option parameters with exact known prices for verification
    struct Batch
    {
        double T, K, sig, r, S;   // Time to maturity, strike, volatility, rate, spot
        double C_exact, P_exact;   // Exact benchmark prices for comparison
    };

    // Example batch options to test Black-Scholes formula
    vector<Batch> batches =
    {
        {0.25, 65, 0.30, 0.08, 60, 2.13337, 5.84628},
        {1.0, 100, 0.20, 0.05, 100, 8.91600, 6.20957},
        {1.0, 10, 0.50, 0.12, 5, 0.49392, 4.19257},
        {30.0, 100.0, 0.30, 0.08, 100.0, 92.17570, 1.24750}
    };

    // Loop over each batch to compute prices without duplicating code
    for (size_t i = 0; i < batches.size(); ++i)
    {
        const auto& b = batches[i];

        // Update option parameters for this batch
        optE.T = b.T;
        optE.K = b.K;
        optE.sig = b.sig;
        optE.r = b.r;
        optE.b = b.r; // Cost-of-carry = risk-free rate for standard European options

        // Compute Call price
        optE.optType = "C"; // Dynamic option type assignment
        double C = optE.Price(b.S); // Black-Scholes formula applied

        // Compute Put price
        optE.optType = "P";
        double P = optE.Price(b.S);

        // Print results for this batch
        cout << "Batch " << i + 1 << endl;
        cout << "Call (computed): " << C << " | Call (exact): " << b.C_exact << endl;
        cout << "Put  (computed): " << P << " | Put  (exact): " << b.P_exact << endl;
        cout << "----------------------------------------\n";
    }

    // ---------------- Put-Call Parity ----------------
    cout << "\nPut-Call Parity\n";

    // Loop again to verify parity relationships using utility functions
    for (size_t i = 0; i < batches.size(); ++i)
    {
        const auto& b = batches[i];

        // Calculate put from call and call from put to test consistency
        double P_from_C = PutFromCall(b.C_exact, b.S, b.K, b.r, b.T);
        double C_from_P = CallFromPut(b.P_exact, b.S, b.K, b.r, b.T);

        // Boolean check for parity
        bool parity_ok = CheckParity(b.C_exact, b.P_exact, b.S, b.K, b.r, b.T);

        // Absolute error for further analysis
        double error = parityError(b.C_exact, b.P_exact, b.S, b.K, b.r, b.r, b.T);

        cout << "Batch " << i + 1 << endl;
        cout << "Put from Call: " << P_from_C << " | Exact Put: " << b.P_exact << endl;
        cout << "Call from Put: " << C_from_P << " | Exact Call: " << b.C_exact << endl;
        cout << "Parity satisfied? " << (parity_ok ? "YES" : "NO") << endl;
        cout << "Parity error: " << error << endl;
        cout << "----------------------------------------\n";
    }

    // ---------------- Vector Pricing ----------------
    cout << "\nVector Pricing over S\n";

    // Standard option parameters for vector pricing
    optE.T = 1.0; optE.K = 100.0; optE.sig = 0.2; optE.r = 0.05; optE.b = optE.r; optE.optType = "C";

    // Generate uniform mesh of spot prices for vectorized evaluation
    // Mesh allows visualizing option behavior vs spot
    vector<double> S_meshE = MeshGenerator::Uniform(10, 50, 1.0);

    // Compute call price at each spot price in S_meshE
    vector<double> prices = MatrixPricer::Vector(optE, S_meshE, OutputType::Price);

    // Print in condensed table format
    cout << "Spot Price(S)   Call Price\n";
    for (size_t i = 0; i < S_meshE.size(); ++i)
        cout << S_meshE[i] << "\t" << prices[i] << endl;
    cout << "----------------------------------------\n";

    // ---------------- Matrix Pricing ----------------
    cout << "\nMatrix Pricing (Volatility Surface Example)\n";

    // paramMatrixE: each row = parameter set (T, K, sigma, r)
    vector<vector<double>> paramMatrixE = {
        {1.0, 100.0, 0.15, 0.05},
        {1.0, 100.0, 0.25, 0.05},
        {1.0, 100.0, 0.35, 0.05}
    };

    // Compute matrix: rows = parameter sets, columns = S_meshE
    vector<vector<double>> priceSurface = MatrixPricer::Matrix(optE, paramMatrixE, S_meshE, OutputType::Price);

    // Table: rows = spot prices, columns = sigma scenarios
    cout << "Spot Price\tSigma=0.15\tSigma=0.25\tSigma=0.35\n";
    for (size_t j = 0; j < S_meshE.size(); ++j) // Loop over spot prices (rows)
    {
        cout << S_meshE[j] << "\t";
        for (size_t i = 0; i < priceSurface.size(); ++i) // Loop over sigmas (columns)
            cout << priceSurface[i][j] << "\t";
        cout << "\n";
    }
    cout << "\n----------------------------------------\n";
    cout << "     Option Sensitivities(Greeks)\n";
    cout << "----------------------------------------\n";

    EuropeanOption greekOpt;
    // Set parameters for Greeks demonstration
    greekOpt.K = 102; greekOpt.S = 122; greekOpt.T = 1.65; greekOpt.r = 0.045; greekOpt.b = 0.0; greekOpt.sig = 0.43;

    greekOpt.optType = "P";
    double Z = optE.Price(greekOpt.S);
    cout << "Put  (computed): " << Z;

    // Print results for this batch
    cout << "Batch " << i + 1 << endl;
    cout << "Call (computed): " << C << " | Call (exact): " << b.C_exact << endl;
    cout << "Put  (computed): " << P << " | Put  (exact): " << b.P_exact << endl;


    // ---------------- Exact Delta and Gamma ----------------
    cout << "\nExact Delta and Gamma\n";

    greekOpt.optType = "C";
    cout << "Call Delta: " << Greeks::Delta(greekOpt, greekOpt.S) << endl; // Exact analytical Delta
    cout << "Call Gamma: " << Greeks::Gamma(greekOpt, greekOpt.S) << endl; // Exact analytical Gamma

    greekOpt.optType = "P";
    cout << "Put Delta: " << Greeks::Delta(greekOpt, greekOpt.S) << endl;
    cout << "Put Gamma: " << Greeks::Gamma(greekOpt, greekOpt.S) << endl;
    cout << "----------------------------------------\n";

    // ---------------- Call Delta vs spot price ----------------
    cout << "\nCall Delta vs Spot Price\n";

    greekOpt.optType = "C";
    vector<double> deltaVector = MatrixPricer::Vector(greekOpt, S_meshE, OutputType::Delta);
    // Vectorized call: Delta vs spot, same mesh as before

    cout << "Spot Price(S)   Call Delta\n";
    for (size_t i = 0; i < S_meshE.size(); ++i)
        cout << S_meshE[i] << "\t" << deltaVector[i] << endl;
    cout << "----------------------------------------\n";

    // ---------------- Delta Surface ----------------
    cout << "\nDelta Surface\n";

    // Matrix of Deltas: rows = parameter sets, columns = spot prices
    vector<vector<double>> deltaSurface = MatrixPricer::Matrix(greekOpt, paramMatrixE, S_meshE, OutputType::Delta);

    cout << "Spot Price\tSigma=0.15\tSigma=0.25\tSigma=0.35\n";
    for (size_t j = 0; j < S_meshE.size(); ++j)
    {
        cout << S_meshE[j] << "\t";
        for (size_t i = 0; i < deltaSurface.size(); ++i)
            cout << deltaSurface[i][j] << "\t";
        cout << "\n";
    }
    cout << "----------------------------------------\n";

    // ---------------- Finite Difference Greeks ----------------
    cout << "\nFinite Difference Greeks\n";

    greekOpt.optType = "C";
    vector<double> h_values = { 1.0, 0.5, 0.1, 0.01 }; // Step sizes for finite difference

    double exactDelta = Greeks::Delta(greekOpt, greekOpt.S);
    double exactGamma = Greeks::Gamma(greekOpt, greekOpt.S);

    for (double h : h_values)
    {
        // Compute numerical derivatives
        double numDelta = Greeks::DeltaFD(greekOpt, greekOpt.S, h);
        double numGamma = Greeks::GammaFD(greekOpt, greekOpt.S, h);

        // Compare with exact values
        cout << "\nStep size h = " << h << endl;
        cout << "Delta FD: " << numDelta << " | Exact: " << exactDelta
            << " | Error: " << fabs(numDelta - exactDelta) << endl;
        cout << "Gamma FD: " << numGamma << " | Exact: " << exactGamma
            << " | Error: " << fabs(numGamma - exactGamma) << endl;
    }
    cout << "\n----------------------------------------\n";
    cout << "      Perpetual American Options\n";
    cout << "----------------------------------------\n";

    // ---------------- Single price test ----------------
    cout << "\nSingle price test\n";

    AmericanOption optA;
    optA.K = 100; optA.sig = 0.1; optA.r = 0.1; optA.b = 0.02; optA.S = 110;

    optA.optType = "C";
    cout << "Call Price (S=110): " << optA.Price(optA.S) << " | Expected: 18.5035" << endl;

    optA.optType = "P";
    cout << "Put Price (S=110): " << optA.Price(optA.S) << " | Expected: 3.03106" << endl;
    cout << "----------------------------------------\n";

    // ---------------- Vector pricing over S ----------------
    cout << "\nPerpetual Option Price vs Spot Price\n";

    vector<double> S_meshA = MeshGenerator::Uniform(10, 50, 1.0); // Mesh of spot prices
    optA.optType = "C";
    vector<double> callVec = MatrixPricer::Vector(optA, S_meshA, OutputType::Price); // Vectorized pricing
    optA.optType = "P";
    vector<double> putVec = MatrixPricer::Vector(optA, S_meshA, OutputType::Price);

    cout << "Spot Price\tCall\t\tPut\n";
    for (size_t i = 0; i < S_meshA.size(); ++i)
        cout << S_meshA[i] << "\t" << callVec[i] << "\t" << putVec[i] << endl;
    cout << "----------------------------------------\n";

    // ---------------- Matrix Pricing ----------------
    cout << "\nMatrix Pricing for Perpetual American Options\n";

    vector<vector<double>> paramMatrixA =
    {
        {0, 100.0, 0.1, 0.1, 0.02},
        {0, 100.0, 0.15, 0.1, 0.02},
        {0, 100.0, 0.2, 0.1, 0.02}
    };

    // Compute price surface: rows = sigmas, columns = spot prices
    vector<vector<double>> callSurface = MatrixPricer::Matrix(optA, paramMatrixA, S_meshA, OutputType::Price);

    cout << "Spot Price\tSigma=0.10\tSigma=0.15\tSigma=0.20\n";
    for (size_t j = 0; j < S_meshA.size(); ++j) // columns = spot prices
    {
        cout << S_meshA[j] << "\t";
        for (size_t i = 0; i < callSurface.size(); ++i) // rows = sigmas
            cout << callSurface[i][j] << "\t";
        cout << endl;
    }
    cout << "----------------------------------------\n";

    return 0;
}


#ifndef EUROPEANOPTION_H
#define EUROPEANOPTION_H

#include <string>

// The EuropeanOption class encapsulates all the data and functionality needed to price a European option using the Black-Scholes formula.

class EuropeanOption
{
private:
    // These helper functions compute the exact Call and Put prices 
    // keeping them private ensures that external code uses the unified Price() interface.
    double CallPrice(double S) const; // Compute exact call price for a given spot
    double PutPrice(double S) const;  // Compute exact put price for a given spot

public:
	// Public member variables for option parameters
    double r;      // Risk-free rate, used in discounting
    double sig;    // Volatility, reflects uncertainty of underlying
    double K;      // Strike price of the option
    double T;      // Time to maturity in years
    double b;      // Cost of carry, allows generalization beyond Black-Scholes (e.g., dividends)
    double S;      // Spot price; stored for convenience but not strictly necessary in Price() functions

    std::string optType; // Option type identifier: "C" for Call, "P" for Put
    // Using string allows easy toggling between types

	//Constructors for EuropeanOption
    EuropeanOption();                     // Default constructor: sets default parameters
    EuropeanOption(const std::string& optionType); // Constructor with option type only

    // Using a public method to call price allows for a unified interface
    // We can call Price without worrying about call vs put
	// The correct pricing function is chosen internally based on optType
    virtual double Price(double S) const;
    void toggle();
};


#endif

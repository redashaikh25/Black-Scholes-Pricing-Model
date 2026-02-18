// Generates meshes (arrays) of spot prices or other parameters for vector/matrix pricing

#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include <vector>

class MeshGenerator
{
public:
    // Uniform mesh generator:
    // Generates a vector of equally spaced points from 'start' to 'end' with spacing 'h'.Used to evaluate option
    //prices or Greeks over a range of spot prices or parameters without manually specifying each value.
    static std::vector<double> Uniform(double start, double end, double h);
};

#endif

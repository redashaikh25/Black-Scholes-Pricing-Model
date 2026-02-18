
#include "MeshGenerator.h"

// Generate a uniform mesh
std::vector<double> MeshGenerator::Uniform(double start, double end, double h)
{
    std::vector<double> mesh;

    // Using a a while loop from start to end with increment h
    // each value is explicitly pushed to maintain sizing and alignment
  
    for (double val = start; val <= end; val += h)
    {
        mesh.push_back(val);
    }

    return mesh;
}

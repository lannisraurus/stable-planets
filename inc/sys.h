#ifndef __SYS__
#define __SYS__

#include <vector>
#include <string>

#include "body.h"
#include "vec2.h"

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TLatex.h"

// Planetary System - This class holds all the information about the planets and is able to create simulations.
class frame;
class sys{
private:

    // Bodies
    std::vector<body> bodies;
    std::vector<body> originalBodies;
    std::vector<double> times;
    // Data
    std::vector<std::vector<std::vector<double>>> distanceToBodies;
    std::vector<std::vector<double>> temperature;
    std::vector<std::vector<double>> orbitalSpeed;
    std::vector<std::vector<double>> orbitalAccel;
    std::vector<std::vector<double>> xPositions;
    std::vector<std::vector<double>> yPositions;

public:

    // Constructors
    sys() = default;
    ~sys() = default;
    // Getters
    std::vector<body> getBodies(){return originalBodies;}
    body operator[](const int& i){return originalBodies[i];}
    int size () {return originalBodies.size();}
    // Remove body from the planetary system.
    void deleteBody(std::string name);
    // Add a body to the planetary system
    void linkBody (body b);
    void linkBody(double mass, double radius, double distance, std::vector<std::string> pivots,
      double angularVelocity, std::string name, double temperature, bool heatSource, double orbitalAngle, bool inverted, double dayAngle);
    // data analysis
    void solve(double T, double dT, frame* f);
    void saveData(std::string append="", std::string time_units="s", std::string distance_units="m", double time_convert=1., double distance_convert=1.);

};

#endif

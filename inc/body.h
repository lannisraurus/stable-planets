#ifndef __BODY__
#define __BODY__

#include <cmath>
#include "vec2.h"

// Celestial Body class - holds all the information needed to describe a celestial body
class body{

private:
    // planet scalar and vector quantities
    std::string name;
    double mass, radius, temperature;
    vec2 position, velocity, acceleration;
    bool isHeatSource;
    // rotational quantities
    double angularVelocity, angle;

public:
    // initialize
    body() = default;
    body(double M, double R, vec2 pos, vec2 vel, vec2 acc, double angulVel, std::string n, double T, bool heatSource, double dayAngle)
    : mass(M), radius(R), position(pos), velocity(vel), acceleration(acc), name(n),
     angle(dayAngle), angularVelocity(angulVel), temperature(T), isHeatSource(heatSource)
    {}
    // inherent quantities
    double getMass() { return mass; }
    double getRadius() { return radius; }
    double getTemperature() { return temperature; }
    double getAngle () { return angle; }
    double getAngularVelocity() const { return angularVelocity; }
    vec2 getPosition () { return position; }
    vec2 getVelocity () { return velocity; }
    vec2 getAcceleration () { return acceleration; }
    std::string getName () { return name; }
    bool getIsHeatSource() {return isHeatSource; }
    // useful quantities
    vec2 getRotationPoint () { return position + radius*vec2(sin(angle),cos(angle));}
    double getOriginAngle() { return atan2(position.Y(),position.X()); }

};

#endif

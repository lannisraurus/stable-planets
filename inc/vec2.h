#ifndef __VEC2__
#define __VEC2__

#include <cmath>

// 2D std::vector class - holds basic 2D std::vector operations
class vec2{
private:
    double x,y;
public:
    vec2();
    vec2(double X,double Y);
    double operator* (const vec2& v);
    vec2 operator+ (const vec2& v);
    vec2 operator- ( const vec2& v);
    friend vec2 operator* (const double& k, const vec2& v);
    vec2 operator*(const double& k);
    vec2 operator/(const double& k);
    double size();
    double X();
    double Y();
    vec2 normalized();
};
vec2 operator* (const double& k, const vec2& v);

#endif

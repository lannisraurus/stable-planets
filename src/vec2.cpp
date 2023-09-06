#include "vec2.h"

vec2 operator* (const double& k, const vec2& v){ return vec2(k*v.x,k*v.y); }

vec2::vec2(): x(0), y(0) {}
vec2::vec2(double X,double Y): x(X), y(Y) {}
double vec2::operator* (const vec2& v) { return x*v.x + y*v.y; }
vec2 vec2::operator+ (const vec2& v) { return vec2(x+v.x,y+v.y); }
vec2 vec2::operator- ( const vec2& v) { return vec2(x-v.x,y-v.y); }
vec2 vec2::operator*(const double& k) {return vec2(k*x,k*y); }
vec2 vec2::operator/(const double& k) {return vec2(x/k,y/k); }
double vec2::size() { return sqrt((*this)*(*this)); }
double vec2::X() { return x; }
double vec2::Y() { return y; }
vec2 vec2::normalized() { double k = size(); return (1/k)*(*this) ; }

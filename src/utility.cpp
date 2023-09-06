#include "utility.h"

double lengthSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return 1000; break;
    case 2: return EARTH_RADIUS; break;
    case 3: return AU; break;
    case 4: return LIGHT_YEAR; break;
    case 5: return PARSEC; break;
    default: return 0; break;
  }
}

double timeSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return EARTH_DAY; break;
    case 2: return EARTH_YEAR; break;
    default: return 0; break;
  }
}

double massSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return EARTH_MASS; break;
    case 2: return SOLAR_MASS; break;
    default: return 0; break;
  }
}

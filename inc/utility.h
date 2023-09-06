#pragma once

#include "def.h"

// UI ID's
enum {
    ID_Test = 1,
    ID_Run = 2,
    ID_TimeStep = 3,
    ID_Duration = 4,
    ID_TimeStepUnits = 5,
    ID_DurationUnits = 6,

    ID_Progress = 7,

    ID_CreatePlanet = 8,
    ID_DeletePlanet = 9,
    ID_SelectPlanet = 10,

    ID_SelectPlanetsCM = 11,
    ID_Distance = 12,
    ID_Mass = 13,
    ID_Radius = 14,
    ID_HeatSource = 15,
    ID_Temperature = 16,
    ID_Name = 17,
    ID_Inverted = 18,
    ID_DistanceUnits = 19,
    ID_Atmosphere = 20,
    ID_Albedo = 21,
    ID_MassUnits = 22,
    ID_RadiusUnits = 23,
    ID_Period = 24,
    ID_PeriodUnits = 25,
    ID_IsRotating = 26,
    ID_Save = 27,
    ID_Load = 28
};

double lengthSI(int i);
double timeSI(int i);
double massSI(int i);

#ifndef LANDERSIM_H__
#define LANDERSIM_H__

#include "types.h"

#define LNDR_STATUS_AIRBOURNE 0
#define LNDR_STATUS_LANDED 1

#define LNDR_TIMEINCR (0.002) // time increment per step in seconds

#define LNDR_MAXTHRUST (30.0f)
#define LNDR_MINTHRUST (0.0f)
#define LNDR_MAXFUEL (40.0f)

typedef float lander_float;
typedef lander_float lander_postype;
typedef lander_float lander_angletype;
typedef lander_float lander_thrusttype;
typedef lander_float lander_fueltype;

// Each member of this struct should be normalized
// to range [0.0, 1.0] where 1.0 is best result.
typedef struct lander_resultS
{
  lander_float altitude;
  lander_float impactspeed;
  lander_float orientation;
  lander_float fuelleft;
  lander_float time;

} lander_resultS;

// Returns 0 only if successful
U32 Lander_Init(char * pszLandscapeFile);

void Lander_Reset();

// Simulates one time increment.
// Returns LNDR_STATUS_x
U32 Lander_Step();

lander_postype Lander_GetXpos();
lander_postype Lander_GetYpos();
lander_angletype Lander_GetAngle();
/*
U32 Lander_IsLeftEngineOn();
U32 Lander_IsRightEngineOn();
*/
void SetControls(lander_thrusttype leftengine,
                 lander_thrusttype rightengine);

void Lander_SetControls(lander_thrusttype leftengine,
                 lander_thrusttype rightengine);

void Lander_GetControls(lander_thrusttype *pleftengine,
                 lander_thrusttype *prightengine);

// Retrieves result of last run
void Lander_GetResult(lander_resultS *res);


#endif //LANDERSIM_H__

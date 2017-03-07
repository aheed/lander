
#include <math.h>
#include <string.h>
#include "bitmappic.h"
#include "landersim.h"


#define MY_PI		3.14159265358979323846

#define MY_MAX(a,b) ((a) > (b) ? (a) : (b))
#define MY_MIN(a,b) ((a) < (b) ? (a) : (b))

#define GROUND_R 220
#define GROUND_G 220
#define GROUND_B 220

#define LANDER_MAXSPEED (8.0f)

typedef struct ShipInfo
{
  lander_postype xpos; //[-1.0, 1.0]
  lander_postype ypos; //[-1.0, 1.0]
  lander_postype xvelo;
  lander_postype yvelo;
  lander_angletype angle;
  lander_postype angvelo;
//  U8    leftengineon;
//  U8    rightengineon;
  lander_thrusttype leftengthrust;
  lander_thrusttype rightengthrust;
  lander_fueltype fuelleft;
} ShipInfo;


////////////////////////////////////////////////
static const float gravityconst = 9.81f; // m/s2
//static const float shipmass = 100; //kg
//static const float timeincr = 0.002; // seconds
static const float engthrust = 30.0f;
static const float rotchange = 1000.0f;
static const float fuelrate = 1.0f;



////////////////////////////////////////////////
static ShipInfo myship;
static U32 landerstatus;
static PicInfo mylandscape;
static lander_resultS lastresult;
static lander_float simtime;

////////////////////////////////////////////////
// Returns 0 only if successful
U32 Lander_Init(char * pszLandscapeFile)
{
  if(LoadPic(pszLandscapeFile, &mylandscape))
  {
    // Failed to load landscape file
    return 1;
  }

  Lander_Reset();
  memset(&lastresult, 0, sizeof(lastresult));
  return 0;
}

void Lander_Reset()
{
  // Get ship into start position
  myship.xpos = 0.0;
  myship.ypos = 0.9;
  myship.xvelo = 0.0;
  myship.yvelo = 0;
  myship.angle = 0;
  myship.angvelo = 0.0;
  myship.leftengthrust = LNDR_MINTHRUST;
  myship.rightengthrust = LNDR_MINTHRUST;
  myship.fuelleft = LNDR_MAXFUEL;

  simtime = 0;
  landerstatus = LNDR_STATUS_AIRBOURNE;
}

// Simulates one time increment.
// Returns LNDR_STATUS_x
U32 Lander_Step()
{
  U8 *pcol;
  float xpos, ypos;
  U32 picx, picy;

  if(landerstatus != LNDR_STATUS_AIRBOURNE)
  {
    // nothing more to do
    return landerstatus;
  }

  //// Update velocity, position etc
  if(myship.fuelleft <= 0.0f)
  {
    myship.leftengthrust = 0.0f;
    myship.rightengthrust = 0.0f;
  }

  myship.fuelleft -= (myship.leftengthrust + myship.rightengthrust) * fuelrate * LNDR_TIMEINCR;
  myship.fuelleft = MY_MAX(myship.fuelleft, 0.0f);

  myship.angvelo -= rotchange * myship.leftengthrust * LNDR_TIMEINCR;
  myship.xvelo -= sin(myship.angle * MY_PI / 180) * myship.leftengthrust * LNDR_TIMEINCR;
  myship.yvelo += cos(myship.angle * MY_PI / 180) * myship.leftengthrust * LNDR_TIMEINCR;

  myship.angvelo += rotchange * myship.rightengthrust * LNDR_TIMEINCR;
  myship.xvelo -= sin(myship.angle * MY_PI / 180) * myship.rightengthrust * LNDR_TIMEINCR;
  myship.yvelo += cos(myship.angle * MY_PI / 180) * myship.rightengthrust * LNDR_TIMEINCR;


  myship.yvelo -= gravityconst * LNDR_TIMEINCR;

  myship.xpos += myship.xvelo * LNDR_TIMEINCR;
  myship.ypos += myship.yvelo * LNDR_TIMEINCR;

  myship.angle += myship.angvelo * LNDR_TIMEINCR;

  // bounce off walls
  if( (myship.xpos > 1.0) || (myship.xpos < -1.0) )
  {
    myship.xvelo = -myship.xvelo;
  }



  //// Update status
  ypos = MY_MAX(MY_MIN(myship.ypos, 1.0f), -1.0f);
  xpos = MY_MAX(MY_MIN(myship.xpos, 1.0f), -1.0f);

//  printf("xpos: %f, ypos: %f\n", xpos, ypos);

  picx = floorf((0.5f + xpos * 0.5f) * mylandscape.width);
  picy = floorf((0.5f + ypos * 0.5f) * mylandscape.height);

//  printf("picx: %d, picy: %d mylsw: %d mylsy:%d\n",
//         picx, picy, mylandscape.width, mylandscape.height);

  pcol = mylandscape.pdata + (picy * mylandscape.width + picx) * 3;

//  printf("Rval: %d\n", (U8)*pcol);

  if((*pcol++ == GROUND_R) && (*pcol++ == GROUND_G) && (*pcol == GROUND_B))
//  if(((U8)*pcol++ == GROUND_R))
  {
    // Hit Ground!
    landerstatus = LNDR_STATUS_LANDED;

    //Save result
    lastresult.altitude = 0.5f - ypos * 0.5f; // low altitude <=> good result
    lastresult.impactspeed = 1.0f - MY_MIN(LANDER_MAXSPEED,
           sqrt(myship.xvelo * myship.xvelo + myship.yvelo * myship.yvelo)) /
             LANDER_MAXSPEED;
    lastresult.orientation = cos(myship.angle * MY_PI / 180) * 0.5f + 0.5f;
    lastresult.fuelleft =  myship.fuelleft / LNDR_MAXFUEL;
    lastresult.time = simtime;
  }

  simtime += LNDR_TIMEINCR;
  return landerstatus;
}


lander_postype Lander_GetXpos()
{
  return myship.xpos;
}

lander_postype Lander_GetYpos()
{
  return myship.ypos;
}

lander_angletype Lander_GetAngle()
{
  return myship.angle;
}

/*
U32 Lander_IsLeftEngineOn()
{
  return (myship.leftengthrust > LNDR_MINTHRUST);
}

U32 Lander_IsRightEngineOn()
{
  return (myship.rightengthrust > LNDR_MINTHRUST);
}
*/

void Lander_SetControls(lander_thrusttype leftengine,
                 lander_thrusttype rightengine)
{
  myship.leftengthrust = leftengine;
  myship.rightengthrust = rightengine;
}

void Lander_GetControls(lander_thrusttype *pleftengine,
                 lander_thrusttype *prightengine)
{
  *pleftengine = myship.leftengthrust;
  *prightengine = myship.rightengthrust;
}

void Lander_GetResult(lander_resultS *res)
{
  *res = lastresult;
}


#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glut.h>
#include "types.h"
#include "bitmappic.h"
#include "landersim.h"


#define LNDR_PIC_BACKGR "background.bmp"

#define LNDR_SCORE_ALT  10000
#define LNDR_SCORE_IMP  1000
#define LNDR_SCORE_ORI  100
#define LNDR_SCORE_FUEL 10

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

static GLuint TexObj[2];
static GLuint TexFlares[2];

static GLfloat bgscale = 2.5f;
static GLfloat shipscale = 0.5f;

static GLboolean HaveTexObj = GL_FALSE;

static U32 lengineon = FALSE;
static U32 rengineon = FALSE;

static U32 simsteppending = FALSE;


#if defined(GL_VERSION_1_1)
#define TEXTURE_OBJECT 1
#elif defined(GL_EXT_texture_object)
#define TEXTURE_OBJECT 1
#define glBindTexture(A,B)     glBindTextureEXT(A,B)
#define glGenTextures(A,B)     glGenTexturesEXT(A,B)
#define glDeleteTextures(A,B)  glDeleteTexturesEXT(A,B)
#endif


///////////////////////////////////////////
/////////////////////////////////////////


/*
/////////////////////////////////////////////////
// Experimental stuff
static void output(int x, int y, char *string)
{
  int len, i;

  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
//    glutBitmapCharacter(font, string[i]);
//    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, string[i]);
    printf("%c", string[i]);
  }
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  output(0, 24, "This is written in a GLUT bitmap font.");
  output(100, 100, "dfsfsdyghfdh");
  output(50, 145, "(positioned in pixels with upper-left origin)");
  glutSwapBuffers();
}
// End experimental stuff
////////////////////////////////////////////////
*/


static void
draw(void)
{
  lander_thrusttype lthrust, rthrust;
  lander_postype xpos, ypos;
  lander_angletype angle;

//  printf("5\n");

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);

  // Draw background

  glPushMatrix();

  glBindTexture(GL_TEXTURE_2D, TexObj[1]);

  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(-bgscale, -bgscale);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(bgscale, -bgscale);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(bgscale, bgscale);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(-bgscale, bgscale);
  glEnd();
  glPopMatrix();


  // Draw space ship
  xpos = Lander_GetXpos();
  ypos = Lander_GetYpos();
  angle = Lander_GetAngle();

  glPushMatrix();

  glTranslatef(xpos * bgscale, ypos * bgscale, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);

  glBindTexture(GL_TEXTURE_2D, TexObj[0]);


  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(-shipscale, -shipscale);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(shipscale, -shipscale);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(shipscale, shipscale);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(-shipscale, shipscale);
  glEnd();

  Lander_GetControls(&lthrust, &rthrust);

  // Left engine
  if(lthrust > LNDR_MINTHRUST)
  {
    glBindTexture(GL_TEXTURE_2D, TexFlares[0]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-shipscale, -shipscale);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(shipscale, -shipscale);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(shipscale, shipscale);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(-shipscale, shipscale);
    glEnd();
  }


  // Right engine
  if(rthrust > LNDR_MINTHRUST)
  {
    glBindTexture(GL_TEXTURE_2D, TexFlares[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-shipscale, -shipscale);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(shipscale, -shipscale);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(shipscale, shipscale);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(-shipscale, shipscale);
    glEnd();
  }

//  glColor4f(0.5, 1.0, 0.5, 0.5);
//  output(100, 700, "does this work?");

  glPopMatrix();




  glutSwapBuffers();
  simsteppending = TRUE;
//  printf("2\n");
}

static void
idle(void)
{
  lander_resultS result;
  lander_float score;

  // Run simulator one more step, but only if
  // there was a redraw since last step.
  if(simsteppending)
  {
    if(Lander_Step() != LNDR_STATUS_AIRBOURNE)
    {
      Lander_GetResult(&result);
      printf("Altitude:%f  Impact:%f  Orientation:%f  Fuel left:%f\n",
             result.altitude, result.impactspeed, result.orientation, result.fuelleft);
      score = result.altitude * LNDR_SCORE_ALT +
              result.impactspeed * LNDR_SCORE_IMP +
              result.orientation * LNDR_SCORE_ORI +
              result.fuelleft * LNDR_SCORE_FUEL;
      printf("Total score: %f\n", score);
      printf("Time: %f sec\n", result.time);
      Lander_Reset();
      lengineon = FALSE;
      rengineon = FALSE;
    }
    simsteppending = FALSE;
  }

  glutPostRedisplay();
}

/* exit upon ESC */
/* ARGSUSED1 */
static void
key(unsigned char k, int x, int y)
{

  switch (k) {
  case 27:  /* Escape */
    glDeleteTextures(2, TexObj);
    glDeleteTextures(2, TexFlares);
    exit(0);
  case 'z':
    // turn left engine on
    {
      lengineon = TRUE;
    }
    break;
  case 'm':
    // turn right engine on
    {
      rengineon = TRUE;
    }
    break;
  }

  // Update controls in case it has changed
  Lander_SetControls(lengineon ? LNDR_MAXTHRUST : LNDR_MINTHRUST,
                     rengineon ? LNDR_MAXTHRUST : LNDR_MINTHRUST);
}

static void
upkey(unsigned char k, int x, int y)
{

  switch (k) {
  case 'z':
    // turn left engine off
    {
      lengineon = FALSE;
    }
    break;
  case 'm':
    // turn right engine off
    {
      rengineon = FALSE;
    }
    break;
  }

  // Update controls in case it has changed
  Lander_SetControls(lengineon ? LNDR_MAXTHRUST : LNDR_MINTHRUST,
                     rengineon ? LNDR_MAXTHRUST : LNDR_MINTHRUST);
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* glOrtho( -3.0, 3.0, -3.0, 3.0, -10.0, 10.0 ); */
  glFrustum(-2.0, 2.0, -2.0, 2.0, 6.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -8.0);
}


static void ConvertRGB2RGBA(const PicInfo* psrc, char *pdest)
{
  char *ptmp, *ptmp1, *ptmp2;
  int i;

  ptmp = psrc->pdata;

  for (i = 0; i < (psrc->height * psrc->width); i++) {
      ptmp1 = ptmp++;
      ptmp2 = ptmp++;


      *pdest++ = *ptmp1;
      *pdest++ = *ptmp2;
      *pdest++ = *ptmp;



      if((*ptmp == 0) && (*ptmp1 == 0) && (*ptmp2 == 0) )
      {
        // This pixel is black. Make it transparent.
        *pdest++ = 0;      }
      else
      {
        *pdest++ = 0xFF;
      }

      ptmp++;
  }
}


static void
init(struct PicInfo* pmovingpic,
     struct PicInfo* pbackground,
     struct PicInfo* plflare,
     struct PicInfo* prflare
     )
{

  GLint i, j;

  GLubyte *ptex;

  glDisable(GL_DITHER);

  /* Setup texturing */
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

  glAlphaFunc(GL_GREATER, 0.1f);
  glEnable(GL_ALPHA_TEST);


  /* generate texture object IDs */
  glGenTextures(2, TexObj);


  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /* setup space ship texture object */
  glBindTexture(GL_TEXTURE_2D, TexObj[0]);


  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  ptex = (GLubyte*)malloc(pmovingpic->width * pmovingpic->height * 4);
  ConvertRGB2RGBA(pmovingpic, ptex);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, pmovingpic->width, pmovingpic->height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, ptex);
  free(ptex);


  glGenTextures(2, TexFlares);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Left engine texture
  glBindTexture(GL_TEXTURE_2D, TexFlares[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  ptex = (GLubyte*)malloc(plflare->width * plflare->height * 4);
  ConvertRGB2RGBA(plflare, ptex);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, plflare->width, plflare->height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, ptex);
  free(ptex);


  // Right engine texture
  glBindTexture(GL_TEXTURE_2D, TexFlares[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  ptex = (GLubyte*)malloc(prflare->width * prflare->height * 4);
  ConvertRGB2RGBA(prflare, ptex);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, prflare->width, prflare->height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, ptex);
  free(ptex);


  glBindTexture(GL_TEXTURE_2D, TexObj[0]);  // why must we do this?

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (!HaveTexObj) {
    glEndList();
  }


  // setup background texture object
  glBindTexture(GL_TEXTURE_2D, TexObj[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  ptex = (GLubyte*)malloc(pbackground->width * pbackground->height * 4);
  ConvertRGB2RGBA(pbackground, ptex);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, pbackground->width, pbackground->height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, ptex);
  free(ptex);


  glClearColor(.1f, .1f, .1f, .1f);

  // Get ship into starting position if necessary
  Lander_Reset();

  lengineon = FALSE;
  rengineon = FALSE;
//  printf("1\n");
}

void
visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

int main(int argc, char *argv[])
{
  PicInfo picinfo1, picinfo2, picinfo3, picinfo4;


  if(LoadPic("spaceship.bmp", &picinfo1))
  {
    printf("Failed to load texture pic. Exiting.\n");
    return 1;
  }

  if(LoadPic(LNDR_PIC_BACKGR, &picinfo2))
  {
    printf("Failed to load texture pic. Exiting.\n");
    return 1;
  }

  if(LoadPic("leftflare.bmp", &picinfo3))
  {
    printf("Failed to load texture pic. Exiting.\n");
    return 1;
  }

  if(LoadPic("rightflare.bmp", &picinfo4))
  {
    printf("Failed to load texture pic. Exiting.\n");
    return 1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

  glutInitWindowSize(800, 800);

  glutCreateWindow("Lander");

  /* check that renderer has the GL_EXT_texture_object
     extension * or supports OpenGL 1.1 */
#ifdef TEXTURE_OBJECT
  {
    char *exten = (char *) glGetString(GL_EXTENSIONS);
    char *version = (char *) glGetString(GL_VERSION);
    if (strstr(exten, "GL_EXT_texture_object")
      || strncmp(version, "1.1", 3) == 0) {
      HaveTexObj = GL_TRUE;
    }
  }
#endif

  if (!HaveTexObj) {
    printf("\nThis program doesn't really work the way it is supposed\n");
    printf("to if you lack OpenGL 1.1 or the EXT_texture_object extension.\n");
    printf("Each textured object should have a different numbered image.\n\n");
  }

  if(Lander_Init(LNDR_PIC_BACKGR))
  {
    printf("Failed to initialize lander simulator. Exiting\n");
    return 1;
  }

  init(&picinfo1, &picinfo2, &picinfo3, &picinfo4);

  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutKeyboardUpFunc(upkey);
  glutVisibilityFunc(visible);
  glutDisplayFunc(draw);
//  glutDisplayFunc(display);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}

/*
* main.h - contains auxiliary functions for rendering, event handling
*          and information displaying 
*
*
*/

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <math.h>
#include <GL/glut.h>
#include "SDL.h"
#include <SDL_console.h>
using namespace std;

#ifdef _WIN32
#define M_PI     3.141592653589793
#define M_PI_2   1.570796326794896
#endif

//--------------------------------------------------------------------
// OpenGL and GLUT specific functions 
//--------------------------------------------------------------------
void drawStr(GLfloat x, GLfloat y, GLfloat z, char* format, ...);

GLvoid initgfx( GLvoid );
GLvoid drawScene( GLvoid );
GLvoid drawSplashScreen( GLvoid );
GLvoid reshape( GLsizei, GLsizei );
void initenv();
int initconsole();

void handle_key_down( SDL_keysym* keysym );
void handle_key_up( SDL_keysym* keysym );
void process_events( void );
void quit_app(int code);

//
//
//

int parseCfgFile(char* filename);
void overwriteDefaults(char *flag, char *value);

//--------------------------------------------------------------------
// Data Structures
//--------------------------------------------------------------------

typedef enum {VID400, VID640, VID800} vid_mode;
const int WIDTHS[3]  = {400, 640, 800};
const int HEIGHTS[3] = {240, 380, 480};

struct Screen {
  int                 width, height;
  vid_mode            mode;
  int                 bpp;
  int                 flags;         // flags to set Video Mode with SDL
  SDL_Surface*        surface;       // main application surface
  GLuint              splash_tex[1]; // splash screen texture
  ConsoleInformation* console;       // 
  bool                console_disp;  // is the console displayed ?
  bool                show_fps;      // do we need to show framerate on the screen hud ?
  int		      fonts[5];      // fonts used with DT library
};

#endif // MAIN_H

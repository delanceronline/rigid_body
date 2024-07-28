#include "ShootingScene.h"

#ifndef win32
	#define win32
	#include <windows.h>		// Header File For Windows
	#include <stdio.h>
	#include <time.h>
#endif

#ifndef opengl
	#define opengl
	#include <gl\gl.h>			// Header File For The OpenGL32 Library
	#include <gl\glu.h>			// Header File For The GLu32 Library
	#include <gl\glaux.h>		// Header File For The Glaux Library
	#include <gl\glut.h>
#endif

void SetDCPixelFormat(HDC hDC);
void SetupRC(void);
GLuint LoadGLTexture( const char *filename );
GLvoid KillGLWindow(GLvoid);
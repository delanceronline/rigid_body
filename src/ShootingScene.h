// ShootingScene.h: interface for the ShootingScene class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SHOOTINGSCENE_H__90E1A980_7300_11D5_AF70_000021DEDA8A__INCLUDED_)
#define AFX_SHOOTINGSCENE_H__90E1A980_7300_11D5_AF70_000021DEDA8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#ifndef milkshape
	#define milkshape
	#include "MilkshapeModel.h"
#endif

#ifndef mathtype
	#define mathtype
	#include "mathtype.h"
#endif

#ifndef checkcontact
	#define checkcontact
	#include "heckContact.h"
#endif

#ifndef dynamicsparameters
	#define dynamicsparameters
	#include "DynamicsParameters.h"
#endif

class ShootingScene  
{
public:
	float cube_mass;
	float Tran_Rot_mat[16];
	void DrawScene(void);
	bool glLightOn, stop_movement;
	void SceneLightingOff(void);
	void SceneLightingOn(void);
	void SetInitialParameters();
	float GenerateFloat(float upto);
	unsigned int GenerateInteger(unsigned upto);
	void LoadModel(void);
	
	Model *pBox1, *pBox2, *pBox3, *pBox4, *pChamber;
	int Box1_list, Box2_list, Box3_list, Box4_list, Chamber_list;

	double interval, timepast, time_elapsed, frame_time;
	float rotx, roty, posx, posy, posz;
	float rotspeed;
	float linearvelocity_z, linearvelocity_y, linearvelocity_x;
	float angularspeedx, angularballspeedx;
	float angularspeedy, angularballspeedy;
	float camera_rot, camera_rotspeed;
	GLUquadricObj *space, *sky;
	float lightpos0[4], ambientLight[4], diffuseLight[4], specular[4], specref[4];

	//BoundBox bbox;
	CheckContact cc;
	DynamicsParameters dp1, dp2, dp3, dp4;

	LARGE_INTEGER timerFrequency;
	LARGE_INTEGER globeTime;	// Start time of the animation
	LARGE_INTEGER currentTime;	// The current time
	double fSeconds;				// Elapsed time in seconds
	double prevfSeconds;				// Previous time in seconds
	double prevfctrlSeconds;
				// Previous controller time in seconds
	void ResetViewport(RECT rect);
	void ChangeSize(int w, int h);
	int LoadGLTextures(char *filename, unsigned int *texture, bool usemipmap);
	AUX_RGBImageRec * LoadBMP(char *Filename);
	void GenerateDisplayList(void);
	void Initialize(void);
	ShootingScene();
	virtual ~ShootingScene();
};

#endif // !defined(AFX_SHOOTINGSCENE_H__90E1A980_7300_11D5_AF70_000021DEDA8A__INCLUDED_)

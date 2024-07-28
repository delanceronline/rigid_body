// ShootingScene.cpp: implementation of the ShootingScene class.
//
//////////////////////////////////////////////////////////////////////
#include "ShootingScene.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ShootingScene::ShootingScene()
{
	pBox1 = NULL;
	pBox2 = NULL;
	pBox3 = NULL;
	pBox4 = NULL;
	pChamber = NULL;
}

ShootingScene::~ShootingScene()
{
	glDeleteLists(Box1_list, 1);
	glDeleteLists(Box2_list, 1);
	glDeleteLists(Box3_list, 1);
	glDeleteLists(Box4_list, 1);
	glDeleteLists(Chamber_list, 1);

	if(pBox1 != NULL)
		delete pBox1;

	if(pBox2 != NULL)
		delete pBox2;

	if(pBox3 != NULL)
		delete pBox3;

	if(pBox4 != NULL)
		delete pBox4;

	if(pChamber != NULL)
		delete pChamber;
}

void ShootingScene::Initialize()
{
	pBox1 = new MilkshapeModel();
	pBox2 = new MilkshapeModel();
	pBox3 = new MilkshapeModel();
	pBox4 = new MilkshapeModel();
	pChamber = new MilkshapeModel();

	SetInitialParameters();

	lightpos0[0] = 0.0f;
	lightpos0[1] = 300.0f;
	lightpos0[2] = 0.0f;
	lightpos0[3] = 0.0f;

	ambientLight[0] = 1.0f;
	ambientLight[1] = 1.0f;
	ambientLight[2] = 1.0f;
	ambientLight[3] = 1.0f;

	diffuseLight[0] = 1.0f;
	diffuseLight[1] = 1.0f;
	diffuseLight[2] = 1.0f;
	diffuseLight[3] = 1.0f;

	specular[0] = 0.6f;
	specular[1] = 0.6f;
	specular[2] = 0.6f;
	specular[3] = 0.6f;

	specref[0] = 1.0f;
	specref[1] = 1.0f;
	specref[2] = 1.0f;
	specref[3] = 1.0f;

	prevfSeconds = 0.0;
	prevfctrlSeconds = 0.0;

	//Set background clearing color to blue
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearStencil(0);

    //
    //Enable depth testing and backface culling.
    //

    glEnable (GL_DEPTH_TEST);    
    glEnable (GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);					//Set blending function for translucency
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	//GenerateDisplayList();

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);
	glLightOn = TRUE;

	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

AUX_RGBImageRec * ShootingScene::LoadBMP(char *Filename)
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;
}

int ShootingScene::LoadGLTextures(char *filename, unsigned int *texture, bool usemipmap)
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP(filename))
	{
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, texture);

		glBindTexture(GL_TEXTURE_2D, *texture);

		if (usemipmap == TRUE)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		}
	}

	if (TextureImage[0])								// If Texture Exists
	{
		if (TextureImage[0]->data)						// If Texture Image Exists
		{
			free(TextureImage[0]->data);				// Free The Texture Image Memory
		}

		free(TextureImage[0]);							// Free The Image Structure
	}

	return Status;										// Return The Status
}

void ShootingScene::ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    fAspect = (float)w/(float)h;
    gluPerspective(45.0, fAspect, 1.0, 4000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ShootingScene::ResetViewport(RECT rect)
{
	ChangeSize(rect.right, rect.bottom);
}

void ShootingScene::LoadModel()
{
	pBox1->loadModelData("models/Box1.ms3d");
	pBox1->reloadTextures();

	pBox2->loadModelData("models/Box2.ms3d");
	pBox2->reloadTextures();

	pBox3->loadModelData("models/Box3.ms3d");
	pBox3->reloadTextures();

	pBox4->loadModelData("models/Box4.ms3d");
	pBox4->reloadTextures();

	pChamber->loadModelData("models/chamber.ms3d");
	pChamber->reloadTextures();
}

unsigned int ShootingScene::GenerateInteger(unsigned int upto)
{
	float var;
	var = ((float)rand() - 0.5f * (float)RAND_MAX)* 0.01f;
	while((unsigned int)var > upto)
		var = ((float)rand() - 0.5f * (float)RAND_MAX)* 0.01f;

	return (unsigned int)var;
}

float ShootingScene::GenerateFloat(float upto)
{
	float var;
	var = ((float)rand() - 0.5f * (float)RAND_MAX)* 0.01f;
	while(var > (float)upto || var < 0.0f)
		var = ((float)rand() - 0.5f * (float)RAND_MAX)* 0.01f;

	return var;
}

void ShootingScene::SetInitialParameters()
{
	stop_movement = FALSE;

	/////////////////////////////////////////////////////////////////////////////////////
	//Box 1.
	//Set bounding box.
	dp1.bbox.Ax.x = 1.0f; dp1.bbox.Ax.y = 0.0f; dp1.bbox.Ax.z = 0.0f;
	dp1.bbox.Ax = ToUnit(dp1.bbox.Ax);
	dp1.bbox.Ay.x = 0.0f; dp1.bbox.Ay.y = 1.0f; dp1.bbox.Ay.z = 0.0f;
	dp1.bbox.Ay = ToUnit(dp1.bbox.Ay);
	dp1.bbox.Az.x = 0.0f; dp1.bbox.Az.y = 0.0f; dp1.bbox.Az.z = 1.0f;	
	dp1.bbox.Az = ToUnit(dp1.bbox.Az);
	dp1.bbox.Ex = 15.0f; dp1.bbox.Ey = 7.5f; dp1.bbox.Ez = 15.0f;
	dp1.bbox.r = dp1.cm_pos;

	//Mass.
	cube_mass = 10.0f;
	dp1.one_over_cube_mass = 1.0f / cube_mass;
	//Postion of centre of mass.
	dp1.cm_pos.x = 0.0f; dp1.cm_pos.y = 30.0f; dp1.cm_pos.z = 0.0f;
	//Linear and angular velocities.
	dp1.lin_velocity.x = 0.0f; dp1.lin_velocity.y = 0.0f; dp1.lin_velocity.z = 0.0f;
	dp1.omega.x = 0.0f; dp1.omega.y = 0.0f; dp1.omega.z = 0.0f;	
	//Linear and angular momentums.
	dp1.lin_mom.x = 0.0f; dp1.lin_mom.y = 0.0f; dp1.lin_mom.z = 0.0f;
	dp1.ang_mom.x = 30.0f; dp1.ang_mom.y = 0.0f; dp1.ang_mom.z = 30.0f;
	//Linear and angular impluses.
	dp1.lin_imp.x = 0.0f; dp1.lin_imp.y = -9.8f * cube_mass * 20.0f; dp1.lin_imp.z = 0.0f;
	dp1.ang_imp.x = 0.0f; dp1.ang_imp.y = 0.0f; dp1.ang_imp.z = 0.0f;
	//Rotational Matrix.
	dp1.Rot_mat[0] = 1.0f; dp1.Rot_mat[1] = 0.0f; dp1.Rot_mat[2] = 0.0f;
	dp1.Rot_mat[3] = 0.0f; dp1.Rot_mat[4] = 1.0f; dp1.Rot_mat[5] = 0.0f;
	dp1.Rot_mat[6] = 0.0f; dp1.Rot_mat[7] = 0.0f; dp1.Rot_mat[8] = 1.0f;
	//Inverse of Ibody.
	dp1.I_body_inv_mat[0] = 1.0f / (cube_mass * (powf(dp1.bbox.Ey * 2.0f, 2.0f) + powf(dp1.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp1.I_body_inv_mat[1] = 0.0f; dp1.I_body_inv_mat[2] = 0.0f;
	dp1.I_body_inv_mat[3] = 0.0f; dp1.I_body_inv_mat[4] = 1.0f / (cube_mass * (powf(dp1.bbox.Ex * 2.0f, 2.0f) + powf(dp1.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp1.I_body_inv_mat[5] = 0.0f;
	dp1.I_body_inv_mat[6] = 0.0f; dp1.I_body_inv_mat[7] = 0.0f; dp1.I_body_inv_mat[8] = 1.0f / (cube_mass * (powf(dp1.bbox.Ex * 2.0f, 2.0f) + powf(dp1.bbox.Ey * 2.0f, 2.0f)) / 12.0f);
	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	//Box 2.
	//Set bounding box.
	dp2.bbox.Ax.x = 1.0f; dp2.bbox.Ax.y = 0.0f; dp2.bbox.Ax.z = 0.0f;
	dp2.bbox.Ax = ToUnit(dp2.bbox.Ax);
	dp2.bbox.Ay.x = 0.0f; dp2.bbox.Ay.y = 1.0f; dp2.bbox.Ay.z = 0.0f;
	dp2.bbox.Ay = ToUnit(dp2.bbox.Ay);
	dp2.bbox.Az.x = 0.0f; dp2.bbox.Az.y = 0.0f; dp2.bbox.Az.z = 1.0f;	
	dp2.bbox.Az = ToUnit(dp2.bbox.Az);
	dp2.bbox.Ex = 20.0f; dp2.bbox.Ey = 20.0f; dp2.bbox.Ez = 20.0f;
	dp2.bbox.r = dp2.cm_pos;

	//Mass.
	cube_mass = 15.0f;
	dp2.one_over_cube_mass = 1.0f / cube_mass;
	//Postion of centre of mass.
	dp2.cm_pos.x = 50.0f; dp2.cm_pos.y = 30.0f; dp2.cm_pos.z = 0.0f;
	dp2.bbox.r = dp2.cm_pos;
	//Linear and angular momentums.
	dp2.lin_mom.x = 0.0f; dp2.lin_mom.y = 0.0f; dp2.lin_mom.z = 0.0f;
	dp2.ang_mom.x = 0.0f; dp2.ang_mom.y = 0.0f; dp2.ang_mom.z = 0.0f;
	//Linear and angular impluses.
	dp2.lin_imp.x = 0.0f; dp2.lin_imp.y = -9.8f * cube_mass * 20.0f; dp2.lin_imp.z = 0.0f;
	dp2.ang_imp.x = 0.0f; dp2.ang_imp.y = 0.0f; dp2.ang_imp.z = 0.0f;
	//Rotational Matrix.
	dp2.Rot_mat[0] = 1.0f; dp2.Rot_mat[1] = 0.0f; dp2.Rot_mat[2] = 0.0f;
	dp2.Rot_mat[3] = 0.0f; dp2.Rot_mat[4] = 1.0f; dp2.Rot_mat[5] = 0.0f;
	dp2.Rot_mat[6] = 0.0f; dp2.Rot_mat[7] = 0.0f; dp2.Rot_mat[8] = 1.0f;
	//Inverse of Ibody.
	dp2.I_body_inv_mat[0] = 1.0f / (cube_mass * (powf(dp2.bbox.Ey * 2.0f, 2.0f) + powf(dp2.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp2.I_body_inv_mat[1] = 0.0f; dp2.I_body_inv_mat[2] = 0.0f;
	dp2.I_body_inv_mat[3] = 0.0f; dp2.I_body_inv_mat[4] = 1.0f / (cube_mass * (powf(dp2.bbox.Ex * 2.0f, 2.0f) + powf(dp2.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp2.I_body_inv_mat[5] = 0.0f;
	dp2.I_body_inv_mat[6] = 0.0f; dp2.I_body_inv_mat[7] = 0.0f; dp2.I_body_inv_mat[8] = 1.0f / (cube_mass * (powf(dp2.bbox.Ex * 2.0f, 2.0f) + powf(dp2.bbox.Ey * 2.0f, 2.0f)) / 12.0f);
	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	//Box 3.
	//Set bounding box.
	dp3.bbox.Ax.x = 1.0f; dp3.bbox.Ax.y = 0.0f; dp3.bbox.Ax.z = 0.0f;
	dp3.bbox.Ax = ToUnit(dp3.bbox.Ax);
	dp3.bbox.Ay.x = 0.0f; dp3.bbox.Ay.y = 1.0f; dp3.bbox.Ay.z = 0.0f;
	dp3.bbox.Ay = ToUnit(dp3.bbox.Ay);
	dp3.bbox.Az.x = 0.0f; dp3.bbox.Az.y = 0.0f; dp3.bbox.Az.z = 1.0f;	
	dp3.bbox.Az = ToUnit(dp3.bbox.Az);
	dp3.bbox.Ex = 20.0f; dp3.bbox.Ey = 10.0f; dp3.bbox.Ez = 20.0f;
	dp3.bbox.r = dp3.cm_pos;

	//Mass.
	cube_mass = 16.0f;
	dp3.one_over_cube_mass = 1.0f / cube_mass;
	//Postion of centre of mass.
	dp3.cm_pos.x = -50.0f; dp3.cm_pos.y = 30.0f; dp3.cm_pos.z = 0.0f;
	//Linear and angular velocities.
	dp3.lin_velocity.x = 0.0f; dp3.lin_velocity.y = 0.0f; dp3.lin_velocity.z = 0.0f;
	dp3.omega.x = 0.0f; dp3.omega.y = 0.0f; dp3.omega.z = 0.0f;	
	//Linear and angular momentums.
	dp3.lin_mom.x = 0.0f; dp3.lin_mom.y = 0.0f; dp3.lin_mom.z = 0.0f;
	dp3.ang_mom.x = 0.0f; dp3.ang_mom.y = 0.0f; dp3.ang_mom.z = 0.0f;
	//Linear and angular impluses.
	dp3.lin_imp.x = 0.0f; dp3.lin_imp.y = -9.8f * cube_mass * 20.0f; dp3.lin_imp.z = 0.0f;
	dp3.ang_imp.x = 0.0f; dp3.ang_imp.y = 0.0f; dp3.ang_imp.z = 0.0f;
	//Rotational Matrix.
	dp3.Rot_mat[0] = 1.0f; dp3.Rot_mat[1] = 0.0f; dp3.Rot_mat[2] = 0.0f;
	dp3.Rot_mat[3] = 0.0f; dp3.Rot_mat[4] = 1.0f; dp3.Rot_mat[5] = 0.0f;
	dp3.Rot_mat[6] = 0.0f; dp3.Rot_mat[7] = 0.0f; dp3.Rot_mat[8] = 1.0f;
	//Inverse of Ibody.
	dp3.I_body_inv_mat[0] = 1.0f / (cube_mass * (powf(dp3.bbox.Ey * 2.0f, 2.0f) + powf(dp3.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp3.I_body_inv_mat[1] = 0.0f; dp3.I_body_inv_mat[2] = 0.0f;
	dp3.I_body_inv_mat[3] = 0.0f; dp3.I_body_inv_mat[4] = 1.0f / (cube_mass * (powf(dp3.bbox.Ex * 2.0f, 2.0f) + powf(dp3.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp3.I_body_inv_mat[5] = 0.0f;
	dp3.I_body_inv_mat[6] = 0.0f; dp3.I_body_inv_mat[7] = 0.0f; dp3.I_body_inv_mat[8] = 1.0f / (cube_mass * (powf(dp3.bbox.Ex * 2.0f, 2.0f) + powf(dp3.bbox.Ey * 2.0f, 2.0f)) / 12.0f);
	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	//Box 4.
	//Set bounding box.
	dp4.bbox.Ax.x = 1.0f; dp4.bbox.Ax.y = 0.0f; dp4.bbox.Ax.z = 0.0f;
	dp4.bbox.Ax = ToUnit(dp4.bbox.Ax);
	dp4.bbox.Ay.x = 0.0f; dp4.bbox.Ay.y = 1.0f; dp4.bbox.Ay.z = 0.0f;
	dp4.bbox.Ay = ToUnit(dp4.bbox.Ay);
	dp4.bbox.Az.x = 0.0f; dp4.bbox.Az.y = 0.0f; dp4.bbox.Az.z = 1.0f;	
	dp4.bbox.Az = ToUnit(dp4.bbox.Az);
	dp4.bbox.Ex = 15.0f; dp4.bbox.Ey = 10.0f; dp4.bbox.Ez = 12.0f;
	dp4.bbox.r = dp4.cm_pos;

	//Mass.
	cube_mass = 32.0f;
	dp4.one_over_cube_mass = 1.0f / cube_mass;
	//Postion of centre of mass.
	dp4.cm_pos.x = 0.0f; dp4.cm_pos.y = 30.0f; dp4.cm_pos.z = 50.0f;
	//Linear and angular velocities.
	dp4.lin_velocity.x = 0.0f; dp4.lin_velocity.y = 0.0f; dp4.lin_velocity.z = 0.0f;
	dp4.omega.x = 0.0f; dp4.omega.y = 0.0f; dp4.omega.z = 0.0f;	
	//Linear and angular momentums.
	dp4.lin_mom.x = 0.0f; dp4.lin_mom.y = 0.0f; dp4.lin_mom.z = 0.0f;
	dp4.ang_mom.x = 0.0f; dp4.ang_mom.y = 0.0f; dp4.ang_mom.z = 0.0f;
	//Linear and angular impluses.
	dp4.lin_imp.x = 0.0f; dp4.lin_imp.y = -9.8f * cube_mass * 20.0f; dp4.lin_imp.z = 0.0f;
	dp4.ang_imp.x = 0.0f; dp4.ang_imp.y = 0.0f; dp4.ang_imp.z = 0.0f;
	//Rotational Matrix.
	dp4.Rot_mat[0] = 1.0f; dp4.Rot_mat[1] = 0.0f; dp4.Rot_mat[2] = 0.0f;
	dp4.Rot_mat[3] = 0.0f; dp4.Rot_mat[4] = 1.0f; dp4.Rot_mat[5] = 0.0f;
	dp4.Rot_mat[6] = 0.0f; dp4.Rot_mat[7] = 0.0f; dp4.Rot_mat[8] = 1.0f;
	//Inverse of Ibody.
	dp4.I_body_inv_mat[0] = 1.0f / (cube_mass * (powf(dp4.bbox.Ey * 2.0f, 2.0f) + powf(dp4.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp4.I_body_inv_mat[1] = 0.0f; dp4.I_body_inv_mat[2] = 0.0f;
	dp4.I_body_inv_mat[3] = 0.0f; dp4.I_body_inv_mat[4] = 1.0f / (cube_mass * (powf(dp4.bbox.Ex * 2.0f, 2.0f) + powf(dp4.bbox.Ez * 2.0f, 2.0f)) / 12.0f); dp4.I_body_inv_mat[5] = 0.0f;
	dp4.I_body_inv_mat[6] = 0.0f; dp4.I_body_inv_mat[7] = 0.0f; dp4.I_body_inv_mat[8] = 1.0f / (cube_mass * (powf(dp4.bbox.Ex * 2.0f, 2.0f) + powf(dp4.bbox.Ey * 2.0f, 2.0f)) / 12.0f);
	/////////////////////////////////////////////////////////////////////////////////////
}

void ShootingScene::SceneLightingOn()
{
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);
	glLightOn = TRUE;
}

void ShootingScene::SceneLightingOff()
{
	glDisable(GL_LIGHTING);
	glLightOn = FALSE;
}


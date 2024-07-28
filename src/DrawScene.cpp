#include "ShootingScene.h"

void ShootingScene::DrawScene()
{
	bool IsCollided;

	QueryPerformanceCounter(&currentTime);
	frame_time = fSeconds - prevfctrlSeconds;
	prevfctrlSeconds = fSeconds;
	
	interval = frame_time * 1.5f;
	//interval = 0.02;

	if(interval > 0.04)
		interval = 0.04;

	time_elapsed += (float)interval;
	fSeconds = (double)(currentTime.QuadPart - globeTime.QuadPart) / (double)timerFrequency.QuadPart;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

	//Camera//
	glTranslatef(0.0f, -80.0f, -300.0f);
	glRotatef(roty, 0.0f, 1.0f, 0.0f);	

	if(!stop_movement)
	{
		dp1.AdvanceParameters((float)interval);
		dp2.AdvanceParameters((float)interval);
		dp3.AdvanceParameters((float)interval);
		dp4.AdvanceParameters((float)interval);

		//Check for contact point with obtain linear and angular impluses applied.
		cc.CollisionDetection(&dp1, &dp2, &IsCollided);
		cc.CollisionDetection(&dp1, &dp3, &IsCollided);
		cc.CollisionDetection(&dp1, &dp4, &IsCollided);
		cc.CollisionDetection(&dp2, &dp3, &IsCollided);
		cc.CollisionDetection(&dp2, &dp4, &IsCollided);
		cc.CollisionDetection(&dp3, &dp4, &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(80.0f, 10.0f, 0.0f), Vector3D(-1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(80.0f, 10.0f, 0.0f), Vector3D(-1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(80.0f, 10.0f, 0.0f), Vector3D(-1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(80.0f, 10.0f, 0.0f), Vector3D(-1.0f, 0.0f, 0.0f), &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(-80.0f, 10.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(-80.0f, 10.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(-80.0f, 10.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(-80.0f, 10.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f), &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(0.0f, 10.0f, 80.0f), Vector3D(0.0f, 0.0f, -1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(0.0f, 10.0f, 80.0f), Vector3D(0.0f, 0.0f, -1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(0.0f, 10.0f, 80.0f), Vector3D(0.0f, 0.0f, -1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(0.0f, 10.0f, 80.0f), Vector3D(0.0f, 0.0f, -1.0f), &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(0.0f, 10.0f, -80.0f), Vector3D(0.0f, 0.0f, 1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(0.0f, 10.0f, -80.0f), Vector3D(0.0f, 0.0f, 1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(0.0f, 10.0f, -80.0f), Vector3D(0.0f, 0.0f, 1.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(0.0f, 10.0f, -80.0f), Vector3D(0.0f, 0.0f, 1.0f), &IsCollided);

		cc.CollisionDetectionFloor(&dp1, Vector3D(0.0f, 160.0f, 0.0f), Vector3D(0.0f, -1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp2, Vector3D(0.0f, 160.0f, 0.0f), Vector3D(0.0f, -1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp3, Vector3D(0.0f, 160.0f, 0.0f), Vector3D(0.0f, -1.0f, 0.0f), &IsCollided);
		cc.CollisionDetectionFloor(&dp4, Vector3D(0.0f, 160.0f, 0.0f), Vector3D(0.0f, -1.0f, 0.0f), &IsCollided);
	}

	//Draw chamber.
	glCallList(Chamber_list);

	//Convert 3x3 rotational matrix and 3x1 displacement matrix to 4x4 matrix.
	Tran_Rot_mat[0] = dp1.Rot_mat[0];
	Tran_Rot_mat[1] = dp1.Rot_mat[3];
	Tran_Rot_mat[2] = dp1.Rot_mat[6];
	Tran_Rot_mat[3] = 0.0f;

	Tran_Rot_mat[4] = dp1.Rot_mat[1];
	Tran_Rot_mat[5] = dp1.Rot_mat[4];
	Tran_Rot_mat[6] = dp1.Rot_mat[7];
	Tran_Rot_mat[7] = 0.0f;

	Tran_Rot_mat[8] = dp1.Rot_mat[2];
	Tran_Rot_mat[9] = dp1.Rot_mat[5];
	Tran_Rot_mat[10] = dp1.Rot_mat[8];
	Tran_Rot_mat[11] = 0.0f;

	Tran_Rot_mat[12] = dp1.cm_pos.x;
	Tran_Rot_mat[13] = dp1.cm_pos.y;
	Tran_Rot_mat[14] = dp1.cm_pos.z;
	Tran_Rot_mat[15] = 1.0f;

	//Box 1.
	glPushMatrix();
		glMultMatrixf(Tran_Rot_mat);
		glCallList(Box1_list);
	glPopMatrix();

	//Convert 3x3 rotational matrix and 3x1 displacement matrix to 4x4 matrix.
	Tran_Rot_mat[0] = dp2.Rot_mat[0];
	Tran_Rot_mat[1] = dp2.Rot_mat[3];
	Tran_Rot_mat[2] = dp2.Rot_mat[6];
	Tran_Rot_mat[3] = 0.0f;

	Tran_Rot_mat[4] = dp2.Rot_mat[1];
	Tran_Rot_mat[5] = dp2.Rot_mat[4];
	Tran_Rot_mat[6] = dp2.Rot_mat[7];
	Tran_Rot_mat[7] = 0.0f;

	Tran_Rot_mat[8] = dp2.Rot_mat[2];
	Tran_Rot_mat[9] = dp2.Rot_mat[5];
	Tran_Rot_mat[10] = dp2.Rot_mat[8];
	Tran_Rot_mat[11] = 0.0f;

	Tran_Rot_mat[12] = dp2.cm_pos.x;
	Tran_Rot_mat[13] = dp2.cm_pos.y;
	Tran_Rot_mat[14] = dp2.cm_pos.z;
	Tran_Rot_mat[15] = 1.0f;

	//Box 2.
	glPushMatrix();
		glMultMatrixf(Tran_Rot_mat);
		glCallList(Box2_list);
	glPopMatrix();

	//Convert 3x3 rotational matrix and 3x1 displacement matrix to 4x4 matrix.
	Tran_Rot_mat[0] = dp3.Rot_mat[0];
	Tran_Rot_mat[1] = dp3.Rot_mat[3];
	Tran_Rot_mat[2] = dp3.Rot_mat[6];
	Tran_Rot_mat[3] = 0.0f;

	Tran_Rot_mat[4] = dp3.Rot_mat[1];
	Tran_Rot_mat[5] = dp3.Rot_mat[4];
	Tran_Rot_mat[6] = dp3.Rot_mat[7];
	Tran_Rot_mat[7] = 0.0f;

	Tran_Rot_mat[8] = dp3.Rot_mat[2];
	Tran_Rot_mat[9] = dp3.Rot_mat[5];
	Tran_Rot_mat[10] = dp3.Rot_mat[8];
	Tran_Rot_mat[11] = 0.0f;

	Tran_Rot_mat[12] = dp3.cm_pos.x;
	Tran_Rot_mat[13] = dp3.cm_pos.y;
	Tran_Rot_mat[14] = dp3.cm_pos.z;
	Tran_Rot_mat[15] = 1.0f;

	//Box 3.
	glPushMatrix();
		glMultMatrixf(Tran_Rot_mat);
		glCallList(Box3_list);
	glPopMatrix();

	//Convert 3x3 rotational matrix and 3x1 displacement matrix to 4x4 matrix.
	Tran_Rot_mat[0] = dp4.Rot_mat[0];
	Tran_Rot_mat[1] = dp4.Rot_mat[3];
	Tran_Rot_mat[2] = dp4.Rot_mat[6];
	Tran_Rot_mat[3] = 0.0f;

	Tran_Rot_mat[4] = dp4.Rot_mat[1];
	Tran_Rot_mat[5] = dp4.Rot_mat[4];
	Tran_Rot_mat[6] = dp4.Rot_mat[7];
	Tran_Rot_mat[7] = 0.0f;

	Tran_Rot_mat[8] = dp4.Rot_mat[2];
	Tran_Rot_mat[9] = dp4.Rot_mat[5];
	Tran_Rot_mat[10] = dp4.Rot_mat[8];
	Tran_Rot_mat[11] = 0.0f;

	Tran_Rot_mat[12] = dp4.cm_pos.x;
	Tran_Rot_mat[13] = dp4.cm_pos.y;
	Tran_Rot_mat[14] = dp4.cm_pos.z;
	Tran_Rot_mat[15] = 1.0f;

	//Box 4.
	glPushMatrix();
		glMultMatrixf(Tran_Rot_mat);
		glCallList(Box4_list);
	glPopMatrix();
}

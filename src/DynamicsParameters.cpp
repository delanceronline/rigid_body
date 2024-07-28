// DynamicsParameters.cpp: implementation of the DynamicsParameters class.
//
//////////////////////////////////////////////////////////////////////

#include "DynamicsParameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void DynamicsParameters::AdvanceParameters(float delta_t)
{
	float m1[9], m2[9];
	Vector3D r;

	//Advance linear and angular momentums.
	AddVector3D(lin_mom, ScalarDotVector(delta_t, lin_imp), &lin_mom);
	AddVector3D(ang_mom, ScalarDotVector(delta_t, ang_imp), &ang_mom);

	//Advance inverse of I.
	MultiMatrix3x3(Rot_mat, I_body_inv_mat, m1);
	MatrixTranspose(Rot_mat, m2);
	MultiMatrix3x3(m1, m2, I_inv_mat);

	//Advance omega(angular velocity).
	MultiMatrix3x1(I_inv_mat, ang_mom, &omega);

	//Advance linear velocity.
	MultiVector3D_f(lin_mom, one_over_cube_mass, &lin_velocity);

	//Advance position of centre of mass.
	MultiVector3D_f(lin_velocity, delta_t, &r);
	AddVector3D(cm_pos, r, &cm_pos);
	cm_pos_copy = cm_pos;
	bbox.r = cm_pos;

	//Advance rotational matrix.
	AdvanceRotationMatrix(Rot_mat, omega, delta_t, m1);
	EqualMatrix(m1, Rot_mat);

	Vector3D Ax, Ay, Az;

	bbox.N_copy[0] = bbox.Nx;
	bbox.N_copy[1] = bbox.Ny;
	bbox.N_copy[2] = bbox.Nz;

	bbox.v_copy[0] = bbox.v[0];
	bbox.v_copy[1] = bbox.v[1];
	bbox.v_copy[2] = bbox.v[2];
	bbox.v_copy[3] = bbox.v[3];
	bbox.v_copy[4] = bbox.v[4];
	bbox.v_copy[5] = bbox.v[5];
	bbox.v_copy[6] = bbox.v[6];
	bbox.v_copy[7] = bbox.v[7];

	//Rotate orth vectors of bounding box.
	MultiMatrix3x1(Rot_mat, bbox.Ax, &Ax);
	bbox.Nx = Ax;
	MultiMatrix3x1(Rot_mat, bbox.Ay, &Ay);
	bbox.Ny = Ay;
	MultiMatrix3x1(Rot_mat, bbox.Az, &Az);
	bbox.Nz = Az;


	//Calculate eight vertexs of bounding box.
	bbox.v[0].x = bbox.r.x + Ax.x * bbox.Ex + Ay.x * bbox.Ey + Az.x * bbox.Ez;
	bbox.v[0].y = bbox.r.y + Ax.y * bbox.Ex + Ay.y * bbox.Ey + Az.y * bbox.Ez;
	bbox.v[0].z = bbox.r.z + Ax.z * bbox.Ex + Ay.z * bbox.Ey + Az.z * bbox.Ez;

	bbox.v[1].x = bbox.r.x + Ax.x * bbox.Ex + Ay.x * bbox.Ey - Az.x * bbox.Ez;
	bbox.v[1].y = bbox.r.y + Ax.y * bbox.Ex + Ay.y * bbox.Ey - Az.y * bbox.Ez;
	bbox.v[1].z = bbox.r.z + Ax.z * bbox.Ex + Ay.z * bbox.Ey - Az.z * bbox.Ez;

	bbox.v[2].x = bbox.r.x - Ax.x * bbox.Ex + Ay.x * bbox.Ey - Az.x * bbox.Ez;
	bbox.v[2].y = bbox.r.y - Ax.y * bbox.Ex + Ay.y * bbox.Ey - Az.y * bbox.Ez;
	bbox.v[2].z = bbox.r.z - Ax.z * bbox.Ex + Ay.z * bbox.Ey - Az.z * bbox.Ez;

	bbox.v[3].x = bbox.r.x - Ax.x * bbox.Ex + Ay.x * bbox.Ey + Az.x * bbox.Ez;
	bbox.v[3].y = bbox.r.y - Ax.y * bbox.Ex + Ay.y * bbox.Ey + Az.y * bbox.Ez;
	bbox.v[3].z = bbox.r.z - Ax.z * bbox.Ex + Ay.z * bbox.Ey + Az.z * bbox.Ez;

	bbox.v[4].x = bbox.r.x + Ax.x * bbox.Ex - Ay.x * bbox.Ey + Az.x * bbox.Ez;
	bbox.v[4].y = bbox.r.y + Ax.y * bbox.Ex - Ay.y * bbox.Ey + Az.y * bbox.Ez;
	bbox.v[4].z = bbox.r.z + Ax.z * bbox.Ex - Ay.z * bbox.Ey + Az.z * bbox.Ez;

	bbox.v[5].x = bbox.r.x + Ax.x * bbox.Ex - Ay.x * bbox.Ey - Az.x * bbox.Ez;
	bbox.v[5].y = bbox.r.y + Ax.y * bbox.Ex - Ay.y * bbox.Ey - Az.y * bbox.Ez;
	bbox.v[5].z = bbox.r.z + Ax.z * bbox.Ex - Ay.z * bbox.Ey - Az.z * bbox.Ez;

	bbox.v[6].x = bbox.r.x - Ax.x * bbox.Ex - Ay.x * bbox.Ey - Az.x * bbox.Ez;
	bbox.v[6].y = bbox.r.y - Ax.y * bbox.Ex - Ay.y * bbox.Ey - Az.y * bbox.Ez;
	bbox.v[6].z = bbox.r.z - Ax.z * bbox.Ex - Ay.z * bbox.Ey - Az.z * bbox.Ez;

	bbox.v[7].x = bbox.r.x - Ax.x * bbox.Ex - Ay.x * bbox.Ey + Az.x * bbox.Ez;
	bbox.v[7].y = bbox.r.y - Ax.y * bbox.Ex - Ay.y * bbox.Ey + Az.y * bbox.Ez;
	bbox.v[7].z = bbox.r.z - Ax.z * bbox.Ex - Ay.z * bbox.Ey + Az.z * bbox.Ez;

}

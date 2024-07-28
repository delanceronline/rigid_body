// heckContact.h: interface for the CheckContact class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HECKCONTACT_H__90F2EC13_6D31_4509_B323_33326719DD57__INCLUDED_)
#define AFX_HECKCONTACT_H__90F2EC13_6D31_4509_B323_33326719DD57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef win32
	#define win32
	#include <windows.h>		// Header File For Windows
	#include <stdio.h>
	#include <time.h>
#endif

#ifndef mathtype
	#define mathtype
	#include "mathtype.h"
#endif

#ifndef boundbox
	#define boundbox
	#include "BoundBox.h"
#endif

#ifndef dynamicsparameters
	#define dynamicsparameters
	#include "DynamicsParameters.h"
#endif

class CheckContact  
{
public:
	void Rebound(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D cp, Vector3D normal, float coef_collision, float coef_friction);
	void EdgeLines(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D line_set1[4][2], Vector3D line_set2[4][2], int iAxis);
	void CollisionDetection(DynamicsParameters *dp1, DynamicsParameters *dp2, bool *IsCollided);
	float ImpluseMagFloor(DynamicsParameters *dp, Vector3D *n, Vector3D r, float coef);
	float ImpluseMag(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D *n, Vector3D r1, Vector3D r2, float coef);
	void CollisionDetectionFloor(DynamicsParameters *dp, Vector3D fp, Vector3D n, bool *IsCollided);
	void MaxProjection(float *proj, int *max_proj_index, float *max_proj);
	void MinProjection(float *proj, int *min_proj_index, float *min_proj);
	void BoundBoxProjections(Vector3D *v, Vector3D fp, Vector3D normal, float *proj);
	bool IsContactFloor(DynamicsParameters *dp, float *penetrated_deep, Vector3D fp, Vector3D normal, int *noContact, Vector3D *ContactPoint, bool Penality);
	bool IsContactFace(Vector3D *v1, Vector3D *v2, float *penetrated_deep, Vector3D fp, Vector3D normal, bool *IsCollideAtLeft);
	void BoundBoxVertexs(float *Rot_mat, BoundBox *bbox);
	CheckContact();
	virtual ~CheckContact();

	Vector3D sep_axis[15];
	int noOverlap_prev;
	bool axis_copy[15];
};

#endif // !defined(AFX_HECKCONTACT_H__90F2EC13_6D31_4509_B323_33326719DD57__INCLUDED_)

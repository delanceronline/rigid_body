// DynamicsParameters.h: interface for the DynamicsParameters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNAMICSPARAMETERS_H__881920DF_D91B_47AF_93D5_56E374A448D5__INCLUDED_)
#define AFX_DYNAMICSPARAMETERS_H__881920DF_D91B_47AF_93D5_56E374A448D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef mathtype
	#define mathtype
	#include "mathtype.h"
#endif

#ifndef boundbox
	#define boundbox
	#include "BoundBox.h"
#endif

#include "Vector3D.h"

class DynamicsParameters  
{
public:
	void AdvanceParameters(float delta_t);
	Vector3D cm_pos, cm_pos_copy;
	Vector3D lin_velocity;
	float I_inv_mat[9];
	Vector3D ang_mom;
	Vector3D lin_mom;
	float I_body_inv_mat[9];
	Vector3D omega;
	float Rot_mat[9];
	Vector3D lin_imp, ang_imp;
	float one_over_cube_mass;
	BoundBox bbox;
};

#endif // !defined(AFX_DYNAMICSPARAMETERS_H__881920DF_D91B_47AF_93D5_56E374A448D5__INCLUDED_)

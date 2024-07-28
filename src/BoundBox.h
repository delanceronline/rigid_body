// BoundBox.h: interface for the BoundBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDBOX_H__32DF943A_0678_4AB0_A663_0FA18E72FDA2__INCLUDED_)
#define AFX_BOUNDBOX_H__32DF943A_0678_4AB0_A663_0FA18E72FDA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef mathtype
	#define mathtype
	#include "mathtype.h"
#endif

class BoundBox  
{
public:
	Vector3D Ax, Ay, Az, r, v[8], v_copy[8], Nx, Ny, Nz, N_copy[3];		//Ax, Ay and Az must be unit vertors.
	float Ex, Ey, Ez;
	BoundBox();
	virtual ~BoundBox();

};

#endif // !defined(AFX_BOUNDBOX_H__32DF943A_0678_4AB0_A663_0FA18E72FDA2__INCLUDED_)

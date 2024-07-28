// heckContact.cpp: implementation of the CheckContact class.
//
//////////////////////////////////////////////////////////////////////

#include "heckContact.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CheckContact::CheckContact()
{

}

CheckContact::~CheckContact()
{

}

/*
This function is to detect whether object of dp collides the floor.
If it does, displacement inside dp will be modified so that it makes 
the bounding box merely touches the surface of the floor.  Also, the
linear and angular momentums of the bounding box will be modified
according to the point of contacts made with the floor.
*/
bool CheckContact::IsContactFloor(DynamicsParameters *dp, float *penetrated_deep, Vector3D fp, Vector3D normal, int *noContact, Vector3D *ContactPoint, bool Penality)
{
	float proj[8];

		float min_proj = 1.0f;
		int min_proj_index[4];

		//Calculate projections of eight vertexs on normal direction of floor.
		BoundBoxProjections(dp->bbox.v, fp, normal, proj);

		//Sort for minimun projection from eight projections
		MinProjection(proj, &min_proj_index[0], &min_proj);

		//If penetration occurs, determine # of vertex involved and point(s) of intersection.
		if(min_proj < 0.0f)
		{
			*noContact = 1;
			for(int i = 0; i < 8; i++)
			{
				if(i != min_proj_index[0])
				{
					if(fabs((double)(proj[min_proj_index[0]] - proj[i])) <= 0.05)
					{
						min_proj_index[*noContact] = i;
						*noContact += 1;
					}
				}
			}

			//Modify position of centre of mass of bounding box so that it touches face merely.
			dp->bbox.r = AddVector(dp->bbox.r, ScalarDotVector((float)fabs((double)min_proj), normal));
			dp->cm_pos = dp->bbox.r;

			*penetrated_deep = min_proj;
			BoundBoxVertexs(dp->Rot_mat, &dp->bbox);

			//Store point(s) of contact(s).
			for(i = 0; i < *noContact; i++)
				ContactPoint[i] = dp->bbox.v[min_proj_index[i]];

			return TRUE;
		}
		else
			return FALSE;

}

/*
This function is for detecting whether two bounding boxs intersect each other.
*/
bool CheckContact::IsContactFace(Vector3D *v1, Vector3D *v2, float *penetrated_deep, Vector3D fp, Vector3D normal, bool *IsCollideAtLeft)
{
	float proj[8], min_proj_dp1 = 1.0f, max_proj_dp1 = 1.0f, min_proj_dp2 = 1.0f, max_proj_dp2 = 1.0f;
	int min_proj_index_dp1[4], max_proj_index_dp1[4], min_proj_index_dp2[4], max_proj_index_dp2[4];

	//Calculate projections of eight vertexs on normal direction of floor.
	BoundBoxProjections(v1, fp, normal, proj);
	//Sort for minimun projection from eight projections.
	MinProjection(proj, &min_proj_index_dp1[0], &min_proj_dp1);
	//Sort for maximun projection from eight projections.
	MaxProjection(proj, &max_proj_index_dp1[0], &max_proj_dp1);

	//Calculate projections of eight vertexs on normal direction of floor.
	BoundBoxProjections(v2, fp, normal, proj);
	//Sort for minimun projection from eight projections.
	MinProjection(proj, &min_proj_index_dp2[0], &min_proj_dp2);
	//Sort for maximun projection from eight projections.
	MaxProjection(proj, &max_proj_index_dp2[0], &max_proj_dp2);

	if(max_proj_dp1 < min_proj_dp2 || max_proj_dp2 < min_proj_dp1)
		return FALSE;
	else
	{
		if(max_proj_dp1 >= min_proj_dp2 && max_proj_dp1 <= max_proj_dp2)
		{
			*penetrated_deep = (float)fabs((double)(max_proj_dp1 - min_proj_dp2));
			*IsCollideAtLeft = FALSE;
		}
		else
		{
			*penetrated_deep = (float)fabs((double)(max_proj_dp2 - min_proj_dp1));
			*IsCollideAtLeft = TRUE;
		}
		return TRUE;
	}
}

void CheckContact::BoundBoxVertexs(float *Rot_mat, BoundBox *bbox)
{

	bbox->N_copy[0] = bbox->Nx;
	bbox->N_copy[1] = bbox->Ny;
	bbox->N_copy[2] = bbox->Nz;

	bbox->v_copy[0] = bbox->v[0];
	bbox->v_copy[1] = bbox->v[1];
	bbox->v_copy[2] = bbox->v[2];
	bbox->v_copy[3] = bbox->v[3];
	bbox->v_copy[4] = bbox->v[4];
	bbox->v_copy[5] = bbox->v[5];
	bbox->v_copy[6] = bbox->v[6];
	bbox->v_copy[7] = bbox->v[7];	
	
	Vector3D Ax, Ay, Az;

	//Rotate orth vectors of bounding box.
	MultiMatrix3x1(Rot_mat, bbox->Ax, &Ax);
	bbox->Nx = Ax;
	MultiMatrix3x1(Rot_mat, bbox->Ay, &Ay);
	bbox->Ny = Ay;
	MultiMatrix3x1(Rot_mat, bbox->Az, &Az);
	bbox->Nz = Az;

	//Calculate eight vertexs of bounding box.
	bbox->v[0].x = bbox->r.x + Ax.x * bbox->Ex + Ay.x * bbox->Ey + Az.x * bbox->Ez;
	bbox->v[0].y = bbox->r.y + Ax.y * bbox->Ex + Ay.y * bbox->Ey + Az.y * bbox->Ez;
	bbox->v[0].z = bbox->r.z + Ax.z * bbox->Ex + Ay.z * bbox->Ey + Az.z * bbox->Ez;

	bbox->v[1].x = bbox->r.x + Ax.x * bbox->Ex + Ay.x * bbox->Ey - Az.x * bbox->Ez;
	bbox->v[1].y = bbox->r.y + Ax.y * bbox->Ex + Ay.y * bbox->Ey - Az.y * bbox->Ez;
	bbox->v[1].z = bbox->r.z + Ax.z * bbox->Ex + Ay.z * bbox->Ey - Az.z * bbox->Ez;

	bbox->v[2].x = bbox->r.x - Ax.x * bbox->Ex + Ay.x * bbox->Ey - Az.x * bbox->Ez;
	bbox->v[2].y = bbox->r.y - Ax.y * bbox->Ex + Ay.y * bbox->Ey - Az.y * bbox->Ez;
	bbox->v[2].z = bbox->r.z - Ax.z * bbox->Ex + Ay.z * bbox->Ey - Az.z * bbox->Ez;

	bbox->v[3].x = bbox->r.x - Ax.x * bbox->Ex + Ay.x * bbox->Ey + Az.x * bbox->Ez;
	bbox->v[3].y = bbox->r.y - Ax.y * bbox->Ex + Ay.y * bbox->Ey + Az.y * bbox->Ez;
	bbox->v[3].z = bbox->r.z - Ax.z * bbox->Ex + Ay.z * bbox->Ey + Az.z * bbox->Ez;

	bbox->v[4].x = bbox->r.x + Ax.x * bbox->Ex - Ay.x * bbox->Ey + Az.x * bbox->Ez;
	bbox->v[4].y = bbox->r.y + Ax.y * bbox->Ex - Ay.y * bbox->Ey + Az.y * bbox->Ez;
	bbox->v[4].z = bbox->r.z + Ax.z * bbox->Ex - Ay.z * bbox->Ey + Az.z * bbox->Ez;

	bbox->v[5].x = bbox->r.x + Ax.x * bbox->Ex - Ay.x * bbox->Ey - Az.x * bbox->Ez;
	bbox->v[5].y = bbox->r.y + Ax.y * bbox->Ex - Ay.y * bbox->Ey - Az.y * bbox->Ez;
	bbox->v[5].z = bbox->r.z + Ax.z * bbox->Ex - Ay.z * bbox->Ey - Az.z * bbox->Ez;

	bbox->v[6].x = bbox->r.x - Ax.x * bbox->Ex - Ay.x * bbox->Ey - Az.x * bbox->Ez;
	bbox->v[6].y = bbox->r.y - Ax.y * bbox->Ex - Ay.y * bbox->Ey - Az.y * bbox->Ez;
	bbox->v[6].z = bbox->r.z - Ax.z * bbox->Ex - Ay.z * bbox->Ey - Az.z * bbox->Ez;

	bbox->v[7].x = bbox->r.x - Ax.x * bbox->Ex - Ay.x * bbox->Ey + Az.x * bbox->Ez;
	bbox->v[7].y = bbox->r.y - Ax.y * bbox->Ex - Ay.y * bbox->Ey + Az.y * bbox->Ez;
	bbox->v[7].z = bbox->r.z - Ax.z * bbox->Ex - Ay.z * bbox->Ey + Az.z * bbox->Ez;

 }

void CheckContact::BoundBoxProjections(Vector3D *v, Vector3D fp, Vector3D normal, float *proj)
{
	proj[0] = DotProduct(normal, SubVector(v[0], fp));
	proj[1] = DotProduct(normal, SubVector(v[1], fp));
	proj[2] = DotProduct(normal, SubVector(v[2], fp));
	proj[3] = DotProduct(normal, SubVector(v[3], fp));
	proj[4] = DotProduct(normal, SubVector(v[4], fp));
	proj[5] = DotProduct(normal, SubVector(v[5], fp));
	proj[6] = DotProduct(normal, SubVector(v[6], fp));
	proj[7] = DotProduct(normal, SubVector(v[7], fp));
}

void CheckContact::MinProjection(float *proj, int *min_proj_index, float *min_proj)
{
	*min_proj_index = 0;
	*min_proj = proj[0];
	
	for(int i = 1; i < 8; i++)
	{
		if(proj[i] < *min_proj)
		{
			*min_proj = proj[i];
			*min_proj_index = i;
		}
	}
}

void CheckContact::MaxProjection(float *proj, int *max_proj_index, float *max_proj)
{
	*max_proj_index = 0;
	*max_proj = proj[0];
	
	for(int i = 1; i < 8; i++)
	{
		if(proj[i] > *max_proj)
		{
			*max_proj = proj[i];
			*max_proj_index = i;
		}
	}
}

void CheckContact::CollisionDetectionFloor(DynamicsParameters *dp, Vector3D fp, Vector3D n, bool *IsCollided)
{
	int noContact;
	Vector3D cp[4];
	float penetrated_deep;
	*IsCollided = FALSE;

	if(IsContactFloor(dp, &penetrated_deep, fp, n, &noContact, cp, true))
	{
		float j;
		Vector3D force, torque, r;

		for(int i = 0; i < noContact; i++)
		{
			r = SubVector(cp[i], dp->bbox.r);
			j = ImpluseMagFloor(dp, &n, r, 0.4f);
				
			force = ScalarDotVector(j, n);
			torque = CrossProduct(r, force);

			//Apply impluse.
			dp->ang_mom = AddVector(dp->ang_mom, torque);
			dp->lin_mom = AddVector(dp->lin_mom, force);

			//Apply friction.
			dp->ang_mom = AddVector(dp->ang_mom, ScalarDotVector(-0.01f, dp->ang_mom));
			dp->lin_mom = AddVector(dp->lin_mom, ScalarDotVector(-0.01f, dp->lin_mom));
		}

		*IsCollided = TRUE;
	}
}

float CheckContact::ImpluseMagFloor(DynamicsParameters *dp, Vector3D *n, Vector3D r, float coef)
{
	//Calculate impluse.
	float Vrel, var, j, numerator, denominator;
	Vector3D temp_v1, temp_v2, pdot;

	temp_v1 = r;
	temp_v2 = CrossProduct(dp->omega, temp_v1);
	pdot = AddVector(dp->lin_velocity, temp_v2);
	Vrel = DotProduct(*n, pdot);

	temp_v1 = CrossProduct(r, *n);
	MultiMatrix3x1(dp->I_inv_mat, temp_v1, &temp_v2);
	temp_v1 = CrossProduct(temp_v2, r);
	var = DotProduct(*n, temp_v1);
			
	numerator = -(1.0f + coef) * Vrel;
	denominator = dp->one_over_cube_mass + var;

	j = numerator / denominator;

	return j;
}

float CheckContact::ImpluseMag(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D *n, Vector3D r1, Vector3D r2, float coef)
{
	//Calculate impluse.
	float Vrel, var1, var2, j, numerator, denominator;
	Vector3D temp_v1, temp_v2, pdot;

	temp_v1 = r1;
	temp_v2 = CrossProduct(dp1->omega, temp_v1);
	pdot = AddVector(dp1->lin_velocity, temp_v2);
	temp_v1 = r2;
	temp_v2 = CrossProduct(dp2->omega, temp_v1);	
	pdot = SubVector(pdot, AddVector(dp2->lin_velocity, temp_v2));
	Vrel = DotProduct(*n, pdot);

	temp_v1 = CrossProduct(r1, *n);
	MultiMatrix3x1(dp1->I_inv_mat, temp_v1, &temp_v2);
	temp_v1 = CrossProduct(temp_v2, r1);
	var1 = DotProduct(*n, temp_v1);

	temp_v1 = CrossProduct(r2, *n);
	MultiMatrix3x1(dp2->I_inv_mat, temp_v1, &temp_v2);
	temp_v1 = CrossProduct(temp_v2, r2);
	var2 = DotProduct(*n, temp_v1);	

	numerator = -(1.0f + coef) * Vrel;
	denominator = dp1->one_over_cube_mass + dp2->one_over_cube_mass + var1 + var2;

	j = numerator / denominator;

	return j;
}

void CheckContact::CollisionDetection(DynamicsParameters *dp1, DynamicsParameters *dp2, bool *IsCollided)
{
	Vector3D facepoint;
	int noOverlap = 0, count;
	float deep, dir_deep[15], dir_deep_prev[15];
	*IsCollided = FALSE;
	bool IsCollideAtLeft, Leftside[15];

	sep_axis[0] = ToUnit(dp1->bbox.Nx);
	sep_axis[1] = ToUnit(dp1->bbox.Ny);
	sep_axis[2] = ToUnit(dp1->bbox.Nz);
	sep_axis[3] = ToUnit(dp2->bbox.Nx);
	sep_axis[4] = ToUnit(dp2->bbox.Ny);
	sep_axis[5] = ToUnit(dp2->bbox.Nz);
	sep_axis[6] = ToUnit(CrossProduct(dp1->bbox.Nx, dp2->bbox.Nx));
	sep_axis[7] = ToUnit(CrossProduct(dp1->bbox.Nx, dp2->bbox.Ny));
	sep_axis[8] = ToUnit(CrossProduct(dp1->bbox.Nx, dp2->bbox.Nz));
	sep_axis[9] = ToUnit(CrossProduct(dp1->bbox.Ny, dp2->bbox.Nx));
	sep_axis[10] = ToUnit(CrossProduct(dp1->bbox.Ny, dp2->bbox.Ny));
	sep_axis[11] = ToUnit(CrossProduct(dp1->bbox.Ny, dp2->bbox.Nz));
	sep_axis[12] = ToUnit(CrossProduct(dp1->bbox.Nz, dp2->bbox.Nx));
	sep_axis[13] = ToUnit(CrossProduct(dp1->bbox.Nz, dp2->bbox.Ny));
	sep_axis[14] = ToUnit(CrossProduct(dp1->bbox.Nz, dp2->bbox.Nz));

	facepoint.x = 0.0f; facepoint.y = 0.0f; facepoint.z = 0.0f;

	for(count = 0; count < 15; count++)
	{
		if(IsContactFace(dp1->bbox.v, dp2->bbox.v, &deep, facepoint, sep_axis[count], &IsCollideAtLeft))
		{
			noOverlap++;
			Leftside[count] = IsCollideAtLeft;
			dir_deep[count] = deep;
		}
		else
		{
			*IsCollided = false;
			return;
		}
	}

	//Check whether two objects do intersect.
	if(noOverlap == 15)
	{
		Vector3D sep_axis_copy[15];

		//Find the last separating axis(s).
		sep_axis_copy[0] = ToUnit(dp1->bbox.N_copy[0]);
		sep_axis_copy[1] = ToUnit(dp1->bbox.N_copy[1]);
		sep_axis_copy[2] = ToUnit(dp1->bbox.N_copy[2]);
		sep_axis_copy[3] = ToUnit(dp2->bbox.N_copy[0]);
		sep_axis_copy[4] = ToUnit(dp2->bbox.N_copy[1]);
		sep_axis_copy[5] = ToUnit(dp2->bbox.N_copy[2]);
		sep_axis_copy[6] = ToUnit(CrossProduct(dp1->bbox.N_copy[0], dp2->bbox.N_copy[0]));
		sep_axis_copy[7] = ToUnit(CrossProduct(dp1->bbox.N_copy[0], dp2->bbox.N_copy[1]));
		sep_axis_copy[8] = ToUnit(CrossProduct(dp1->bbox.N_copy[0], dp2->bbox.N_copy[2]));
		sep_axis_copy[9] = ToUnit(CrossProduct(dp1->bbox.N_copy[1], dp2->bbox.N_copy[0]));
		sep_axis_copy[10] = ToUnit(CrossProduct(dp1->bbox.N_copy[1], dp2->bbox.N_copy[1]));
		sep_axis_copy[11] = ToUnit(CrossProduct(dp1->bbox.N_copy[1], dp2->bbox.N_copy[2]));
		sep_axis_copy[12] = ToUnit(CrossProduct(dp1->bbox.N_copy[2], dp2->bbox.N_copy[0]));
		sep_axis_copy[13] = ToUnit(CrossProduct(dp1->bbox.N_copy[2], dp2->bbox.N_copy[1]));
		sep_axis_copy[14] = ToUnit(CrossProduct(dp1->bbox.N_copy[2], dp2->bbox.N_copy[2]));

		bool overlap_axis[15];
		int last_overlap_axis_index[15], noSep_axis=0;
		noOverlap = 0;

		for(count = 0; count < 15; count++)
		{
			if(IsContactFace(dp1->bbox.v_copy, dp2->bbox.v_copy, &deep, facepoint, sep_axis_copy[count], &IsCollideAtLeft))
			{
				noOverlap++;
				overlap_axis[count] = TRUE;
				dir_deep_prev[count] = deep;
			}
			else
			{
				last_overlap_axis_index[noSep_axis] = count;
				overlap_axis[count] = FALSE;
				noSep_axis++;
			}
		}

		if(noOverlap == 15 || noOverlap < 14)
		{
			float min_deep;

			min_deep = dir_deep_prev[0];
			last_overlap_axis_index[0] = 0;
			overlap_axis[0] = TRUE;
			for(count = 1; count < 15; count++)
			{
				if(min_deep > dir_deep_prev[count])
				{
					min_deep = dir_deep_prev[count];
					last_overlap_axis_index[0] = count;
					overlap_axis[count] = TRUE;
				}
			}
			noOverlap = 14;
		}

		//Handle collision for only one last separating axis.
		if(noOverlap == 14)
		{
			int noContact;
			Vector3D normal, fp, cp[4];

			//Check for whether edge-edge collision occurs.
			bool EdgetoEdge = FALSE;

			if(last_overlap_axis_index[0] >= 6)
				EdgetoEdge = TRUE;

			if(!EdgetoEdge)
			{

				if(last_overlap_axis_index[0] < 3)
				{

					if(!Leftside[last_overlap_axis_index[0]])
					{
						normal = sep_axis[last_overlap_axis_index[0]];
					}
					else
					{
						normal = ScalarDotVector(-1.0f, sep_axis[last_overlap_axis_index[0]]);
					}

					//Face is on dp1.
					float Ext;
					if(last_overlap_axis_index[0] == 0)
						Ext = dp1->bbox.Ex;
					if(last_overlap_axis_index[0] == 1)
						Ext = dp1->bbox.Ey;
					if(last_overlap_axis_index[0] == 2)
						Ext = dp1->bbox.Ez;

					fp = AddVector(dp1->bbox.r, ScalarDotVector(Ext, normal));
					IsContactFloor(dp2, &deep, fp, normal, &noContact, cp, false);

					for(int i = 0; i < noContact; i++)
					{
						Rebound(dp2, dp1, cp[i], normal, 0.2f, -0.01f);
					}

				}
				else
				{
					if(Leftside[last_overlap_axis_index[0]])
					{
						normal = sep_axis[last_overlap_axis_index[0]];
					}
					else
					{
						normal = ScalarDotVector(-1.0f, sep_axis[last_overlap_axis_index[0]]);
					}
					
					//Face is on dp2.
					float Ext;
					if(last_overlap_axis_index[0] == 3)
						Ext = dp2->bbox.Ex;
					if(last_overlap_axis_index[0] == 4)
						Ext = dp2->bbox.Ey;
					if(last_overlap_axis_index[0] == 5)
						Ext = dp2->bbox.Ez;
					
					fp = AddVector(dp2->bbox.r, ScalarDotVector(Ext, normal));
					IsContactFloor(dp1, &deep, fp, normal, &noContact, cp, false);

					for(int i = 0; i < noContact; i++)
					{
						Rebound(dp1, dp2, cp[i], normal, 0.2f, -0.01f);
					}
				}

			}//End of vertex-face detection.
			else
			{
				//Handle edge-edge collision.

				Vector3D n;
				//Adjust two bounding boxs just touching edge by edge.
				if(Leftside[last_overlap_axis_index[0]])
				{
					n = ScalarDotVector(-1.0f * dir_deep[last_overlap_axis_index[0]], sep_axis[last_overlap_axis_index[0]]);
					dp2->bbox.r = AddVector(dp2->bbox.r, n);
				}
				else
				{
					n = ScalarDotVector(dir_deep[last_overlap_axis_index[0]], sep_axis[last_overlap_axis_index[0]]);
					dp2->bbox.r = AddVector(dp2->bbox.r, n);
				}
				dp2->cm_pos = dp2->bbox.r;

				BoundBoxVertexs(dp2->Rot_mat, &dp2->bbox);

				Vector3D line_set1[4][2], line_set2[4][2];
				
				EdgeLines(dp1, dp2, line_set1, line_set2, last_overlap_axis_index[0]);

				int i, j, iSet1, iSet2;
				float a[3], b[3], c[3], det, min;
				bool firstcheck = TRUE;

				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						a[0] = (line_set1[i][0].x - line_set1[i][1].x);
						a[1] = (line_set1[i][0].y - line_set1[i][1].y);
						a[2] = (line_set1[i][0].z - line_set1[i][1].z);

						b[0] = (line_set2[j][1].x - line_set2[j][0].x);
						b[1] = (line_set2[j][1].y - line_set2[j][0].y);
						b[2] = (line_set2[j][1].z - line_set2[j][0].z);
						
						c[0] = (line_set1[i][1].x - line_set2[j][1].x);
						c[1] = (line_set1[i][1].y - line_set2[j][1].y);
						c[2] = (line_set1[i][1].z - line_set2[j][1].z);
	
						det = a[0] * (b[1] * c[2] - b[2] * c[1]) - b[0] * (a[1] * c[2] - a[2] * c[1]) + c[0] * (a[1] * b[2] - a[2] * b[1]);

						if(firstcheck)
						{
							min = det;
							iSet1 = i;
							iSet2 = j;
							firstcheck = FALSE;
						}

						if(det < min)
						{
							min = det;
							iSet1 = i;
							iSet2 = j;
						}
					}
				}

				Vector3D x_axis;
				x_axis.x = 1.0f; x_axis.y = 0.0f; x_axis.z = 0.0f;
				if(DotProduct(x_axis, sep_axis[last_overlap_axis_index[0]]) != 1.0f)
				{
					//Project on XZ-plane (x, z).
					float t1, t2, var;
					float a0, a1, b0, b1, c0, c1;
					Vector3D cp;
					
					a0 = line_set1[iSet1][1].x - line_set1[iSet1][0].x;
					a1 = line_set1[iSet1][1].z - line_set1[iSet1][0].z;
					b0 = line_set2[iSet2][1].x - line_set2[iSet2][0].x;
					b1 = line_set2[iSet2][1].z - line_set2[iSet2][0].z;
					c0 = line_set2[iSet2][0].x - line_set1[iSet1][0].x;
					c1 = line_set2[iSet2][0].z - line_set1[iSet1][0].z;

					var = b1 * a0 - a1 * b0;
					if(var != 0.0f)
						t1 = (c0 * b1 - b0 * c1) / var;
					
					if(b0 != 0.0f)
						t2 = (a0 * t1 - c0) / b0;
					else
						t2 = (a1 * t1 - c1) / b1;

					cp.x = line_set1[iSet1][0].x + t1 * (line_set1[iSet1][1].x - line_set1[iSet1][0].x);
					cp.y = (line_set1[iSet1][0].y + line_set1[iSet1][1].y + line_set2[iSet2][0].y + line_set2[iSet2][1].y) * 0.25f;
					cp.z = line_set1[iSet1][0].z + t1 * (line_set1[iSet1][1].z - line_set1[iSet1][0].z);

					Rebound(dp1, dp2, cp, sep_axis[last_overlap_axis_index[0]], 0.2f, -0.01f);
				}
				else
				{
					Vector3D y_axis;
					y_axis.x = 0.0f; y_axis.y = 1.0f; y_axis.z = 0.0f;
					if(DotProduct(y_axis, sep_axis[last_overlap_axis_index[0]]) != 1.0f)
					{
						//Project on YZ-plane (y, z).
						float t1, t2, var;
						float a0, a1, b0, b1, c0, c1;
						Vector3D cp;
						
						a0 = line_set1[iSet1][1].y - line_set1[iSet1][0].y;
						a1 = line_set1[iSet1][1].z - line_set1[iSet1][0].z;
						b0 = line_set2[iSet2][1].y - line_set2[iSet2][0].y;
						b1 = line_set2[iSet2][1].z - line_set2[iSet2][0].z;
						c0 = line_set2[iSet2][0].y - line_set1[iSet1][0].y;
						c1 = line_set2[iSet2][0].z - line_set1[iSet1][0].z;

						var = b1 * a0 - a1 * b0;
						if(var != 0.0f)
							t1 = (c0 * b1 - b0 * c1) / var;
						
						if(b0 != 0.0f)
							t2 = (a0 * t1 - c0) / b0;
						else
							t2 = (a1 * t1 - c1) / b1;

						cp.x = (line_set1[iSet1][0].x + line_set1[iSet1][1].x + line_set2[iSet2][0].x + line_set2[iSet2][1].x) * 0.25f;
						cp.y = line_set1[iSet1][0].y + t1 * (line_set1[iSet1][1].y - line_set1[iSet1][0].y);
						cp.z = line_set1[iSet1][0].z + t1 * (line_set1[iSet1][1].z - line_set1[iSet1][0].z);

						Rebound(dp1, dp2, cp, sep_axis[last_overlap_axis_index[0]], 0.2f, -0.01f);

					}
					else
					{
						//Project on XY-plane (x, y).
						float t1, t2, var;
						float a0, a1, b0, b1, c0, c1;
						Vector3D cp;
						
						a0 = line_set1[iSet1][1].x - line_set1[iSet1][0].x;
						a1 = line_set1[iSet1][1].y - line_set1[iSet1][0].y;
						b0 = line_set2[iSet2][1].x - line_set2[iSet2][0].x;
						b1 = line_set2[iSet2][1].y - line_set2[iSet2][0].y;
						c0 = line_set2[iSet2][0].x - line_set1[iSet1][0].x;
						c1 = line_set2[iSet2][0].y - line_set1[iSet1][0].y;

						var = b1 * a0 - a1 * b0;
						if(var != 0.0f)
							t1 = (c0 * b1 - b0 * c1) / var;
						
						if(b0 != 0.0f)
							t2 = (a0 * t1 - c0) / b0;
						else
							t2 = (a1 * t1 - c1) / b1;

						cp.x = line_set1[iSet1][0].x + t1 * (line_set1[iSet1][1].x - line_set1[iSet1][0].x);
						cp.y = line_set1[iSet1][0].y + t1 * (line_set1[iSet1][1].y - line_set1[iSet1][0].y);
						cp.z = (line_set1[iSet1][0].z + line_set1[iSet1][1].z + line_set2[iSet2][0].z + line_set2[iSet2][1].z) * 0.25f;

						Rebound(dp1, dp2, cp, sep_axis[last_overlap_axis_index[0]], 0.2f, -0.01f);

					}
				}

			}//End of edge-to-edge.

		}//End of overlapping 14 axis.


		*IsCollided = TRUE;
	}//End of overlapping 15 axis.
	
}

void CheckContact::EdgeLines(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D line_set1[4][2], Vector3D line_set2[4][2], int iAxis)
{
	if(iAxis == 6)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[3];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[4];
		line_set1[2][1] = dp1->bbox.v[7];

		line_set1[3][0] = dp1->bbox.v[5];
		line_set1[3][1] = dp1->bbox.v[6];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[3];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[4];
		line_set2[2][1] = dp2->bbox.v[7];

		line_set2[3][0] = dp2->bbox.v[5];
		line_set2[3][1] = dp2->bbox.v[6];
	}

	if(iAxis == 7)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[3];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[4];
		line_set1[2][1] = dp1->bbox.v[7];

		line_set1[3][0] = dp1->bbox.v[5];
		line_set1[3][1] = dp1->bbox.v[6];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[4];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[5];

		line_set2[2][0] = dp2->bbox.v[2];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[3];
		line_set2[3][1] = dp2->bbox.v[7];
	}

	if(iAxis == 8)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[3];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[4];
		line_set1[2][1] = dp1->bbox.v[7];

		line_set1[3][0] = dp1->bbox.v[5];
		line_set1[3][1] = dp1->bbox.v[6];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[1];

		line_set2[1][0] = dp2->bbox.v[3];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[7];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[4];
		line_set2[3][1] = dp2->bbox.v[5];
	}

	if(iAxis == 9)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[4];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[5];

		line_set1[2][0] = dp1->bbox.v[2];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[3];
		line_set1[3][1] = dp1->bbox.v[7];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[3];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[4];
		line_set2[2][1] = dp2->bbox.v[7];

		line_set2[3][0] = dp2->bbox.v[5];
		line_set2[3][1] = dp2->bbox.v[6];
	}

	if(iAxis == 10)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[4];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[5];

		line_set1[2][0] = dp1->bbox.v[2];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[3];
		line_set1[3][1] = dp1->bbox.v[7];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[4];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[5];

		line_set2[2][0] = dp2->bbox.v[2];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[3];
		line_set2[3][1] = dp2->bbox.v[7];
	}

	if(iAxis == 11)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[4];

		line_set1[1][0] = dp1->bbox.v[1];
		line_set1[1][1] = dp1->bbox.v[5];

		line_set1[2][0] = dp1->bbox.v[2];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[3];
		line_set1[3][1] = dp1->bbox.v[7];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[1];

		line_set2[1][0] = dp2->bbox.v[3];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[7];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[4];
		line_set2[3][1] = dp2->bbox.v[5];
	}

	if(iAxis == 12)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[1];

		line_set1[1][0] = dp1->bbox.v[3];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[7];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[4];
		line_set1[3][1] = dp1->bbox.v[5];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[3];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[5];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[4];
		line_set2[3][1] = dp2->bbox.v[7];
	}

	if(iAxis == 13)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[1];

		line_set1[1][0] = dp1->bbox.v[3];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[7];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[4];
		line_set1[3][1] = dp1->bbox.v[5];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[4];

		line_set2[1][0] = dp2->bbox.v[1];
		line_set2[1][1] = dp2->bbox.v[5];

		line_set2[2][0] = dp2->bbox.v[2];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[3];
		line_set2[3][1] = dp2->bbox.v[7];
	}

	if(iAxis == 14)
	{
		line_set1[0][0] = dp1->bbox.v[0];
		line_set1[0][1] = dp1->bbox.v[1];

		line_set1[1][0] = dp1->bbox.v[3];
		line_set1[1][1] = dp1->bbox.v[2];

		line_set1[2][0] = dp1->bbox.v[7];
		line_set1[2][1] = dp1->bbox.v[6];

		line_set1[3][0] = dp1->bbox.v[4];
		line_set1[3][1] = dp1->bbox.v[5];

		line_set2[0][0] = dp2->bbox.v[0];
		line_set2[0][1] = dp2->bbox.v[1];

		line_set2[1][0] = dp2->bbox.v[3];
		line_set2[1][1] = dp2->bbox.v[2];

		line_set2[2][0] = dp2->bbox.v[7];
		line_set2[2][1] = dp2->bbox.v[6];

		line_set2[3][0] = dp2->bbox.v[4];
		line_set2[3][1] = dp2->bbox.v[5];
	}
}

void CheckContact::Rebound(DynamicsParameters *dp1, DynamicsParameters *dp2, Vector3D cp, Vector3D normal, float coef_collision, float coef_friction)
{
	Vector3D force, torque, r1, r2;
	float J;
	r1 = SubVector(cp, dp1->bbox.r);
	r2 = SubVector(cp, dp2->bbox.r);
	J = ImpluseMag(dp1, dp2, &normal, r1, r2, coef_collision);
		
	force = ScalarDotVector(J, normal);
	torque = CrossProduct(r1, force);

	//Apply impluse.
	dp1->ang_mom = AddVector(dp1->ang_mom, torque);
	dp1->lin_mom = AddVector(dp1->lin_mom, force);

	//Apply friction.
	dp1->ang_mom = AddVector(dp1->ang_mom, ScalarDotVector(coef_friction, dp1->ang_mom));
	dp1->lin_mom = AddVector(dp1->lin_mom, ScalarDotVector(coef_friction, dp1->lin_mom));
					
	force = ScalarDotVector(-J, normal);
	torque = CrossProduct(r2, force);

	//Apply impluse.
	dp2->ang_mom = AddVector(dp2->ang_mom, torque);
	dp2->lin_mom = AddVector(dp2->lin_mom, force);

	//Apply friction.
	dp2->ang_mom = AddVector(dp2->ang_mom, ScalarDotVector(coef_friction, dp2->ang_mom));
	dp2->lin_mom = AddVector(dp2->lin_mom, ScalarDotVector(coef_friction, dp2->lin_mom));

}

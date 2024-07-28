#include <math.h>

#ifndef mathtype
	#define mathtype
	#include "mathtype.h"
#endif

// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
	{
	float length;
	
	// Calculate the length of the vector		
	length = (float)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that mball_a.y calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
	{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
	}

float DotProduct(Vector3D vector1, Vector3D vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

Vector3D ScalarDotVector(float var, Vector3D v)
{
	Vector3D result;

	result.x = var * v.x;
	result.y = var * v.y;
	result.z = var * v.z;

	return result;
}

Vector3D AddVector(Vector3D v1, Vector3D v2)
{
	Vector3D result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

Vector3D SubVector(Vector3D v1, Vector3D v2)
{
	Vector3D result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}	

Vector3D ToUnit(Vector3D v)
{
	Vector3D result;
	float mag;

	mag = (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	
	result.x = v.x / mag;
	result.y = v.y / mag;
	result.z = v.z / mag;

	return result;
}

float VectorMag(Vector3D v)
{
	return (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

bool IsEqual(float var1, float var2)
{
	if(fabs(var1 - var2) <= 1e-8)
		return true;
	else
		return false;
}

bool Greater(float var1, float var2)
{
	if(var1 - var2 > 1e-8)
		return true;
	else
		return false;
}

bool Less(float var1, float var2)
{
	if(var1 - var2 < -1e-8)
		return true;
	else
		return false;
}

bool GreaterOrEqual(float var1, float var2)
{
	if(Greater(var1, var2) || IsEqual(var1, var2))
		return true;
	else
		return false;
}

bool LessOrEqual(float var1, float var2)
{
	if(Less(var1, var2) || IsEqual(var1, var2))
		return true;
	else
		return false;
}

void MultiMatrix3x3(float *m1, float *m2, float *m3)
{
	m3[0] = m1[0] * m2[0] + m1[1] * m2[3] + m1[2] * m2[6];
	m3[1] = m1[0] * m2[1] + m1[1] * m2[4] + m1[2] * m2[7];
	m3[2] = m1[0] * m2[2] + m1[1] * m2[5] + m1[2] * m2[8];

	m3[3] = m1[3] * m2[0] + m1[4] * m2[3] + m1[5] * m2[6];
	m3[4] = m1[3] * m2[1] + m1[4] * m2[4] + m1[5] * m2[7];
	m3[5] = m1[3] * m2[2] + m1[4] * m2[5] + m1[5] * m2[8];

	m3[6] = m1[6] * m2[0] + m1[7] * m2[3] + m1[8] * m2[6];
	m3[7] = m1[6] * m2[1] + m1[7] * m2[4] + m1[8] * m2[7];
	m3[8] = m1[6] * m2[2] + m1[7] * m2[5] + m1[8] * m2[8];
}

void AddMatrix3x3(float *m1, float *m2, float *m3)
{
	m3[0] = m1[0] + m2[0];
	m3[1] = m1[1] + m2[1];
	m3[2] = m1[2] + m2[2];

	m3[3] = m1[3] + m2[3];
	m3[4] = m1[4] + m2[4];
	m3[5] = m1[5] + m2[5];

	m3[6] = m1[6] + m2[6];
	m3[7] = m1[7] + m2[7];
	m3[8] = m1[8] + m2[8];
}

void MultiMatrix_f(float *m, float factor, float *m_out)
{
	m_out[0] = m[0] * factor;
	m_out[1] = m[1] * factor;
	m_out[2] = m[2] * factor;

	m_out[3] = m[3] * factor;
	m_out[4] = m[4] * factor;
	m_out[5] = m[5] * factor;

	m_out[6] = m[6] * factor;
	m_out[7] = m[7] * factor;
	m_out[8] = m[8] * factor;
}

void MatrixStar(Vector3D v, float *m_out)
{
	m_out[0] = 0.0f;
	m_out[1] = -v.z;
	m_out[2] = v.y;

	m_out[3] = v.z;
	m_out[4] = 0.0f;
	m_out[5] = -v.x;

	m_out[6] = -v.y;
	m_out[7] = v.x;
	m_out[8] = 0.0f;
}

void MatrixTranspose(float *m, float *m_out)
{
	m_out[0] = m[0]; m_out[1] = m[3]; m_out[2] = m[6];
	m_out[3] = m[1]; m_out[4] = m[4]; m_out[5] = m[7];
	m_out[6] = m[2]; m_out[7] = m[5]; m_out[8] = m[8];
}

void EqualMatrix(float *m1, float *m2)
{
	m2[0] = m1[0]; m2[1] = m1[1]; m2[2] = m1[2];
	m2[3] = m1[3]; m2[4] = m1[4]; m2[5] = m1[5];
	m2[6] = m1[6]; m2[7] = m1[7]; m2[8] = m1[8];
}

void MultiMatrix3x1(float *m, Vector3D v, Vector3D *v_out)
{
	v_out->x = m[0] * v.x + m[1] * v.y + m[2] * v.z;
	v_out->y = m[3] * v.x + m[4] * v.y + m[5] * v.z;
	v_out->z = m[6] * v.x + m[7] * v.y + m[8] * v.z;
}

void MultiMatrix1x3(float *m, Vector3D v, Vector3D *v_out)
{
	v_out->x = m[0] * v.x + m[3] * v.y + m[6] * v.z;
	v_out->y = m[1] * v.x + m[4] * v.y + m[7] * v.z;
	v_out->z = m[2] * v.x + m[5] * v.y + m[8] * v.z;
}

void AddVector3D(Vector3D v1, Vector3D v2, Vector3D *v_out)
{
	v_out->x = v1.x + v2.x;
	v_out->y = v1.y + v2.y;
	v_out->z = v1.z + v2.z;
}

void MultiVector3D_f(Vector3D v, float factor, Vector3D *v_out)
{
	v_out->x = v.x * factor;
	v_out->y = v.y * factor;
	v_out->z = v.z * factor;
}

void AdvanceRotationMatrix(float *Ri, Vector3D omega, float delta_t, float *Ro)
{
	float r, i, j, k, tr, s;
	float rdot, idot, jdot, kdot;

	//Find r, i, j and k of quaternion.
	tr = Ri[0] + Ri[4] + Ri[8];

	if(tr >= 0.0f)
	{
		s = (float)sqrt((float)(tr + 1.0f));
		r = 0.5f * s;
		s = 0.5f / s;
		i = (Ri[7] - Ri[5]) * s;
		j = (Ri[2] - Ri[6]) * s;
		k = (Ri[3] - Ri[1]) * s;
	}
	else
	{
		int a = 0;

		if(Ri[4] > Ri[0])
			a = 1;
		if(Ri[8] > Ri[a * 3 + a])
			a = 2;

		switch(a)
		{
		case 0:
			s = (float)sqrt((float)((Ri[0] - (Ri[4] + Ri[8])) + 1.0f));
			i = 0.5f * s;
			s = 0.5f / s;
			j = (Ri[1] + Ri[3]) * s;
			k = (Ri[6] + Ri[2]) * s;
			r = (Ri[7] - Ri[5]) * s;
			break;

		case 1:
			s = (float)sqrt((float)((Ri[4] - (Ri[8] + Ri[0])) + 1.0f));
			j = 0.5f * s;
			s = 0.5f / s;
			k = (Ri[5] + Ri[7]) * s;
			i = (Ri[1] + Ri[3]) * s;
			r = (Ri[2] - Ri[6]) * s;
			break;

		case 2:
			s = (float)sqrt((float)((Ri[8] - (Ri[0] + Ri[4])) + 1.0f));
			k = 0.5f * s;
			s = 0.5f / s;
			i = (Ri[6] + Ri[2]) * s;
			j = (Ri[5] + Ri[7]) * s;
			r = (Ri[3] - Ri[1]) * s;
		}
	}

	//Advance quaternion.
	rdot = -0.5f * (omega.x * i + omega.y * j + omega.z * k);
	idot = 0.5f * (omega.y * k - omega.z * j + omega.x * r);
	jdot = 0.5f * (omega.z * i - omega.x * k + omega.y * r);
	kdot = 0.5f * (omega.x * j - omega.y * i + omega.z * r);

	r += rdot * delta_t;
	i += idot * delta_t;
	j += jdot * delta_t;
	k += kdot * delta_t;

	//Normalize quaternion.
	float var;
	var = (float)sqrt((float)(r * r + i * i + j * j + k * k));
	r = r / var;
	i = i / var;
	j = j / var;
	k = k / var;
	
	//Convert quaternion to matrix 3x3.
	Ro[0] = 1.0f - 2.0f * j * j - 2.0f * k * k;
	Ro[1] = 2.0f * i * j - 2.0f * r * k;
	Ro[2] = 2.0f * i * k + 2.0f * r * j;

	Ro[3] = 2.0f * i * j + 2.0f * r * k;
	Ro[4] = 1.0f - 2.0f * i * i - 2.0f * k * k;
	Ro[5] = 2.0f * j * k - 2.0f * r * i;

	Ro[6] = 2.0f * i * k - 2.0f * r * j;
	Ro[7] = 2.0f * j * k + 2.0f * r * i;
	Ro[8] = 1.0f - 2.0f * i * i - 2.0f * j * j;
}

Vector3D CrossProduct(Vector3D v1, Vector3D v2)
{
	Vector3D v3;

	v3.x = v1.y * v2.z - v1.z * v2.y;
	v3.y = v1.z * v2.x - v1.x * v2.z;
	v3.z = v1.x * v2.y - v1.y * v2.x;

	return v3;
}
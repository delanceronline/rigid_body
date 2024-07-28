#include "Vector3D.h"

#define DEGTORAD(x) ((x) * 0.017453292519943296)
#define RADTODEG(x) ((x) * 57.29577951308232088)

struct Vector2D
{
	float x, y;
};

void MultiMatrix1x3(float *m, Vector3D v, Vector3D *v_out);
void AdvanceRotationMatrix(float *Ri, Vector3D omega, float delta_t, float *Ro);
void MultiVector3D_f(Vector3D v, float factor, Vector3D *v_out);
void AddVector3D(Vector3D v1, Vector3D v2, Vector3D *v_out);
void MultiMatrix3x1(float *m, Vector3D v, Vector3D *v_out);
void EqualMatrix(float *m1, float *m2);
void MatrixTranspose(float *m, float *m_out);
void MatrixStar(Vector3D v, float *m_out);
void MultiMatrix_f(float *m, float factor, float *m_out);
void MultiMatrix3x3(float *m1, float *m2, float *m3);
void AddMatrix3x3(float *m1, float *m2, float *m3);
void ReduceToUnit(float vector[3]);
void calcNormal(float v[3][3], float out[3]);
float DotProduct(Vector3D vector1, Vector3D vector2);
Vector3D ScalarDotVector(float var, Vector3D v);
Vector3D AddVector(Vector3D v1, Vector3D v2);
Vector3D SubVector(Vector3D v1, Vector3D v2);
Vector3D ToUnit(Vector3D v);
Vector3D CrossProduct(Vector3D v1, Vector3D v2);
float VectorMag(Vector3D v);
bool IsEqual(float var1, float var2);
bool Greater(float var1, float var2);
bool Less(float var1, float var2);
bool GreaterOrEqual(float var1, float var2);
bool LessOrEqual(float var1, float var2);

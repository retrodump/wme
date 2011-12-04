// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
float MathUtil::Round(float val)
{
	float result = floor(val);
	if (val - result >= 0.5) result += 1.0;
	return result;
}

//////////////////////////////////////////////////////////////////////////
float MathUtil::RoundUp(float val)
{
	float result = floor(val);
	if (val - result > 0) result += 1.0;
	return result;
}

// http://www.ogre3d.org/forums/viewtopic.php?f=5&t=35202
//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 MathUtil::GetBarycentricCoordinates(const Ogre::Vector3& P1, const Ogre::Vector3& P2, const Ogre::Vector3& P3, const Ogre::Vector3& P)
{
	Ogre::Vector3 v;
	Ogre::Vector3 w;
	Ogre::Vector3 u;

	v = P2 - P1;
	w = P3 - P1;
	u = v.crossProduct(w);
	Ogre::Real A = u.length();      //u

	v = P2 - P;
	w = P3 - P;
	u = v.crossProduct(w);
	Ogre::Real A1 = u.length();      //u1

	v = P - P1;
	w = P3 - P1;
	u = v.crossProduct(w);
	Ogre::Real A2 = u.length();      //u2

	//v = P2 - P1;
	//w = P - P1;
	//u = v.crossProduct(w);
	//Ogre::Real A3 = u.length();      //u3

	//we should check dot products of u.u1, u.u2, u.u3 and use the signs of those as the signs of A1/A, A2/A, A3/A below, but since we know the ray intersects the triangle we know the 3 barycentric coordinates are all positive
	Ogre::Vector3 coordinates;
	coordinates.x = A1 / A;
	coordinates.y = A2 / A;
	coordinates.z = 1.0f - coordinates.x - coordinates.y;

	return coordinates;
}

// Calculate the line segment PaPb that is the shortest route between two lines P1P2 and P3P4.
// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/
//////////////////////////////////////////////////////////////////////////
bool MathUtil::LineLineIntersect(const Ogre::Vector3& p1, const Ogre::Vector3& p2, const Ogre::Vector3& p3, const Ogre::Vector3& p4, Ogre::Vector3& pa, Ogre::Vector3& pb, Ogre::Real& mua, Ogre::Real& mub)
{
	const Ogre::Real EPS = 0.0001f;

	Ogre::Vector3 p13, p43, p21;
	Ogre::Real d1343, d4321, d1321, d4343, d2121;
	Ogre::Real numer, denom;

	p13 = p1 - p3;
	p43 = p4 - p3;

	if (Ogre::Math::Abs(p43.x) < EPS && Ogre::Math::Abs(p43.y) < EPS && Ogre::Math::Abs(p43.z) < EPS)
		return false;

	p21 = p2 - p1;

	if (Ogre::Math::Abs(p21.x) < EPS && Ogre::Math::Abs(p21.y) < EPS && Ogre::Math::Abs(p21.z) < EPS)
		return false;

	d1343 = p13.dotProduct(p43);
	d4321 = p43.dotProduct(p21);
	d1321 = p13.dotProduct(p21);
	d4343 = p43.dotProduct(p43);
	d2121 = p21.dotProduct(p21);

	denom = d2121 * d4343 - d4321 * d4321;
	if (Ogre::Math::Abs(denom) < EPS)
		return false;

	numer = d1343 * d4321 - d1321 * d4343;

	mua = numer / denom;
	mub = (d1343 + d4321 * mua) / d4343;

	pa = p1 + mua * p21;
	pb = p3 + mub * p43;

	return true;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real MathUtil::GetPlaneY(const Ogre::Plane& plane, Ogre::Real x, Ogre::Real z)
{
	//Ax + By + Cz + D = 0
	// By = -(Ax + Cz + D)
	// y = -(Ax + Cz + D)/B

	if (plane.normal.y)
	{
		return -(plane.normal.x * x + plane.normal.z * z + plane.d) / plane.normal.y;
	}

	return 0.0f;
}

//////////////////////////////////////////////////////////////////////////
bool MathUtil::PointInRect(const Ogre::Vector2& point, const Ogre::FloatRect& rect)
{
	if (point.x >= rect.left && point.x <= rect.right && point.y >= rect.top && point.y <= rect.bottom) return true;
	else return false;
}


//////////////////////////////////////////////////////////////////////////
float MathUtil::DistanceFromPointToLine(Ogre::Vector3 point, Ogre::Vector3 anchor, Ogre::Vector3 end)
{
	Ogre::Vector3 d = end - anchor;
	float length = d.length();
	if (length == 0) return (point - anchor).length();
	
	d.normalise();
	float intersect = (point - anchor).dotProduct(d);
	if (intersect < 0) return (point - anchor).length();
	else if (intersect > length) return (point - end).length();
	else return (point - (anchor + d * intersect)).length();
}

//////////////////////////////////////////////////////////////////////////
Ogre::Degree MathUtil::NormalizeAngle(const Ogre::Degree& angle)
{
	Ogre::Real val = angle.valueDegrees();
	val = fmod(val, 360.0f);
	if (val < 0) val += 360.0f;

	return Ogre::Degree(val);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Matrix3 MathUtil::MatFromEulerAngles(const Ogre::Radian& roll, const Ogre::Radian& pitch, const Ogre::Radian& yaw)
{
	Ogre::Real cx, sx, cy, sy, cz, sz;
	Ogre::Real cxsy, sxsy;

	cx = Ogre::Math::Cos(roll); //A
	sx = Ogre::Math::Sin(roll); //B
	cy = Ogre::Math::Cos(pitch); //C
	sy = Ogre::Math::Sin(pitch); //D
	cz = Ogre::Math::Cos(yaw); //E
	sz = Ogre::Math::Sin(yaw); //F

	cxsy = cx * sy; //AD
	sxsy = sx * sy; //BD 

	Ogre::Matrix3 mat;

	mat[0][0] =  cy * cz;
	mat[1][0] = -cy * sz;
	mat[2][0] = sy;
	mat[0][1] = sxsy * cz + cx * sz;
	mat[1][1] = -sxsy * sz + cx * cz;
	mat[2][1] = -sx * cy;
	mat[0][2] =  -cxsy * cz + sx * sz;
	mat[1][2] =  cxsy * sz + sx * cz;
	mat[2][2] =  cx * cy;
	
	return mat;
}

//////////////////////////////////////////////////////////////////////////
void MathUtil::EulerAnglesFromMat(const Ogre::Matrix3& mat, Ogre::Radian& roll, Ogre::Radian& pitch, Ogre::Radian& yaw)
{
	Ogre::Radian angle_x, angle_y, angle_z;
	Ogre::Real cx, cy, cz;
	Ogre::Real sx,     sz;

	angle_y = Ogre::Math::ASin(Ogre::Math::Clamp(mat[2][0], -1.f, 1.f));
	cy = Ogre::Math::Cos(angle_y);

	if (fabs(cy) > 0.005)		// non-zero
	{
		// no gimbal lock
		cx = mat[2][2] / cy;
		sx = - mat[2][1] / cy;

		angle_x = Ogre::Math::ATan2(sx, cx);

		cz = mat[0][0] / cy;
		sz = - mat[1][0] / cy;

		angle_z = Ogre::Math::ATan2(sz, cz);
	}
	else
	{
		angle_x = Ogre::Radian(0);

		cz = mat[1][1];
		sz = mat[0][1];

		angle_z = Ogre::Math::ATan2(sz, cz);
	}

	roll = angle_x;
	pitch = angle_y;
	yaw = angle_z;
}

//////////////////////////////////////////////////////////////////////////
void MathUtil::QuaternionGetEulerAngles(const Ogre::Quaternion& q, Ogre::Radian& roll, Ogre::Radian& pitch, Ogre::Radian& yaw)
{
	Ogre::Matrix3 rotMat = QuaternionToMatrix(q);
	rotMat.Orthonormalize();
	EulerAnglesFromMat(rotMat, roll, pitch, yaw);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Quaternion MathUtil::QuaternionSetEulerAngles(const Ogre::Radian& roll, const Ogre::Radian& pitch, const Ogre::Radian& yaw)
{
	Ogre::Matrix3 rotMat = MatFromEulerAngles(roll, pitch, yaw);
	rotMat.Orthonormalize();
	Ogre::Quaternion q = MatrixToQuaternion(rotMat);
	q.normalise();

	return q;
}


//////////////////////////////////////////////////////////////////////////
Ogre::Matrix3 MathUtil::QuaternionToMatrix(const Ogre::Quaternion& q)
{
	Ogre::Matrix3 mat;
	Ogre::Real xx, xy, xz, xw, yy, yz, yw, zz, zw;

	xx      = q.x * q.x;
	xy      = q.x * q.y;
	xz      = q.x * q.z;
	xw      = q.x * q.w;

	yy      = q.y * q.y;
	yz      = q.y * q.z;
	yw      = q.y * q.w;

	zz      = q.z * q.z;
	zw      = q.z * q.w;

	mat[0][0]  = 1.f - 2.f * ( yy + zz );
	mat[0][1]  =	   2.f * ( xy + zw );
	mat[0][2]  =	   2.f * ( xz - yw );

	mat[1][0]  =	   2.f * ( xy - zw );
	mat[1][1]  = 1.f - 2.f * ( xx + zz );
	mat[1][2]  =	   2.f * ( yz + xw );

	mat[2][0]  =	   2.f * ( xz + yw );
	mat[2][1]  =	   2.f * ( yz - xw );
	mat[2][2]  = 1.f - 2.f * ( xx + yy );

	return mat;
}


//////////////////////////////////////////////////////////////////////////
Ogre::Quaternion MathUtil::MatrixToQuaternion(const Ogre::Matrix3& mat)
{
	Ogre::Quaternion quat;
	Ogre::Real tr, s, q[4];
	int i, j, k;
	int nxt[3] = {1, 2, 0};

	tr = mat[0][0] + mat[1][1] + mat[2][2];

	// check the diagonal
	if (tr > 0.f) 
	{
		s = Ogre::Math::Sqrt(tr + 1.f);
		s = 0.5f / s;
		quat.x = (mat[1][2] - mat[2][1]) * s;
		quat.y = (mat[2][0] - mat[0][2]) * s;
		quat.z = (mat[0][1] - mat[1][0]) * s;
	} 
	else
	{		
		// diagonal is negative
		i = 0;
		if (mat[1][1] > mat[0][0]) 
			i = 1;
		if (mat[2][2] > mat[i][i]) 
			i = 2;

		j = nxt[i];
		k = nxt[j];


		s = Ogre::Math::Sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1.f);

		q[i] = s * 0.5f;

		if (s != 0.f) 
			s = 0.5f / s;

		q[3] = (mat[j][k] - mat[k][j]) * s;
		q[j] = (mat[i][j] + mat[j][i]) * s;
		q[k] = (mat[i][k] + mat[k][i]) * s;

		quat.x = q[0];
		quat.y = q[1];
		quat.z = q[2];
		quat.w = q[3];
	}
	return quat;
}


//////////////////////////////////////////////////////////////////////////
bool MathUtil::Vec3Abs(Ogre::Vector3& vec)
{
	bool ret = false;

	if (vec.x < 0.0)
	{
		vec.x = -vec.x;
		ret = true;
	}
	if (vec.y < 0.0)
	{
		vec.y = -vec.y;
		ret = true;
	}
	if (vec.z < 0.0)
	{
		vec.z = -vec.z;
		ret = true;
	}
	return ret;
}


} // namespace Wme

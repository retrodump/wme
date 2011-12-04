// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMathUtil_H__
#define __WmeMathUtil_H__

namespace Wme
{
	class WmeDllExport MathUtil
	{
	public:
		static float Round(float val);
		static float RoundUp(float val);
		static Ogre::Vector3 GetBarycentricCoordinates(const Ogre::Vector3& P1, const Ogre::Vector3& P2, const Ogre::Vector3& P3, const Ogre::Vector3& P);
		static bool LineLineIntersect(const Ogre::Vector3& p1, const Ogre::Vector3& p2, const Ogre::Vector3& p3, const Ogre::Vector3& p4, Ogre::Vector3& pa, Ogre::Vector3& pb, Ogre::Real& mua, Ogre::Real& mub);
		static Ogre::Real GetPlaneY(const Ogre::Plane& plane, Ogre::Real x, Ogre::Real z);
		static bool PointInRect(const Ogre::Vector2& point, const Ogre::FloatRect& rect);
		static float DistanceFromPointToLine(Ogre::Vector3 point, Ogre::Vector3 anchor, Ogre::Vector3 end);
		static Ogre::Degree NormalizeAngle(const Ogre::Degree& angle);

		static Ogre::Matrix3 MatFromEulerAngles(const Ogre::Radian& roll, const Ogre::Radian& pitch, const Ogre::Radian& yaw);
		static void EulerAnglesFromMat(const Ogre::Matrix3& mat, Ogre::Radian& roll, Ogre::Radian& pitch, Ogre::Radian& yaw);

		static void QuaternionGetEulerAngles(const Ogre::Quaternion& q, Ogre::Radian& roll, Ogre::Radian& pitch, Ogre::Radian& yaw);
		static Ogre::Quaternion QuaternionSetEulerAngles(const Ogre::Radian& roll, const Ogre::Radian& pitch, const Ogre::Radian& yaw);
		static Ogre::Matrix3 QuaternionToMatrix(const Ogre::Quaternion& q);
		static Ogre::Quaternion MatrixToQuaternion(const Ogre::Matrix3& mat);

		static bool Vec3Abs(Ogre::Vector3& vec);
	};
}

#endif // __WmeMathUtil_H__

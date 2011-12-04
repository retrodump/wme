// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTriangleElement3D_H__
#define __WmeTriangleElement3D_H__


#include "Object.h"


namespace Wme
{
	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport Triangle3D
	{
	public:
		Triangle3D()
		{
			for (int i = 0; i < 3; i++)
			{
				m_Points[i] = Ogre::Vector3::ZERO;
			}
			m_Color = Ogre::ColourValue::White;
		}
		Triangle3D(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC, const Ogre::ColourValue& color)
		{
			m_Points[0] = pointA;
			m_Points[1] = pointB;
			m_Points[2] = pointC;
			m_Color = color;
		}
		Ogre::Vector3 m_Points[3];
		Ogre::ColourValue m_Color;
	};

	class WmeDllExport TriangleElement3D : public Object, public Ogre::SimpleRenderable
	{
	public:
		TriangleElement3D();
		~TriangleElement3D();

		int GetNumTriangles() const { return m_Triangles.size(); }

		void AddTriangle(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC, const Ogre::ColourValue& color = Ogre::ColourValue::White);
		void ClearTriangles();

		void UpdateGeometry();

		bool GetDepthCheck() const { return m_DepthCheck; }
		void SetDepthCheck(bool depthCheck)
		{
			m_DepthCheck = depthCheck;
			SetMaterial(m_DepthCheck);
		}


		// Ogre::SimpleRenderable
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
		Ogre::Real getBoundingRadius() const;
		void getWorldTransforms (Ogre::Matrix4* xform) const;

	private:
		bool m_IsDirty;
		bool m_DepthCheck;

		int m_MaxTriangles;
		bool m_HWBufferInitialized;

		typedef std::list<Triangle3D> TriangleList;
		TriangleList m_Triangles;

		void SetMaterial(bool depthCheck = true);
	};
}

#endif // __WmeTriangleElement3D_H__
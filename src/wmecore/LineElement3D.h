// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLineElement3D_H__
#define __WmeLineElement3D_H__


#include "Object.h"


namespace Wme
{
	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport Line3D
	{
	public:
		Line3D()
		{
			m_StartPoint = Ogre::Vector3::ZERO;
			m_EndPoint = Ogre::Vector3::ZERO;
			m_Color = Ogre::ColourValue::White;
		}
		Line3D(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, const Ogre::ColourValue& color)
		{
			m_StartPoint = startPoint;
			m_EndPoint = endPoint;
			m_Color = color;
		}
		Ogre::Vector3 m_StartPoint;
		Ogre::Vector3 m_EndPoint;
		Ogre::ColourValue m_Color;
	};

	class WmeDllExport LineElement3D : public Object, public Ogre::SimpleRenderable
	{
	public:
		LineElement3D();
		~LineElement3D();

		int GetNumLines() const { return m_Lines.size(); }

		void AddLine(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, const Ogre::ColourValue& color = Ogre::ColourValue::White);
		void ClearLines();

		void AddBox(const Ogre::Vector3& min, const Ogre::Vector3& max, const Ogre::ColourValue& color = Ogre::ColourValue::White);

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

		int m_MaxLines;
		bool m_HWBufferInitialized;

		typedef std::list<Line3D> LineList;
		LineList m_Lines;

		void SetMaterial(bool depthCheck = true);
	};
}

#endif // __WmeLineElement3D_H__
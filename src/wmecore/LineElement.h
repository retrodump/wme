// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLineElement_H__
#define __WmeLineElement_H__


#include "Object.h"
#include "IRenderableProvider.h"


namespace Wme
{
	class Viewport;
	class Region;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport Line
	{
	public:
		Line()
		{
			m_StartPoint = Ogre::Vector2::ZERO;
			m_EndPoint = Ogre::Vector2::ZERO;
		}
		Line(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint)
		{
			m_StartPoint = startPoint;
			m_EndPoint = endPoint;
		}
		Line(float startX, float startY, float endX, float endY)
		{
			m_StartPoint = Ogre::Vector2(startX, startY);
			m_EndPoint = Ogre::Vector2(endX, endY);
		}
		Ogre::Vector2 m_StartPoint;
		Ogre::Vector2 m_EndPoint;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport LineElement : public Object, public Ogre::Renderable, public IRenderableProvider
	{
	public:
		LineElement();
		~LineElement();

		int GetNumLines() const { return m_Lines.size(); }

		Ogre::ColourValue GetColor() const { return m_Color; }
		void SetColor(const Ogre::ColourValue& color)
		{
			if (color != m_Color) m_IsDirty = true;
			m_Color = color;
		}

		void AddLine(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint);
		void ClearLines();

		void AddRegion(const Region& region);
		void AddRectangle(const Ogre::Vector2& startPoint, const Ogre::Vector2& endPoint);

		void UpdateGeometry(Viewport* viewport, bool absolutePos);

		// IRenderableProvider implementation
		virtual void GetRenderables(IRenderableProvider::RenderableList& renderableList);

		// Ogre::Renderable implementation
		virtual const Ogre::MaterialPtr& getMaterial(void) const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		const Ogre::LightList& getLights(void) const;

	private:
		int m_UpdateId;
		Ogre::ColourValue m_Color;
		bool m_IsDirty;
		Ogre::MaterialPtr m_Material;
		typedef std::list<Line> LineList;
		LineList m_Lines;

		Ogre::HardwareVertexBufferSharedPtr m_HardwareBuffer;
		Ogre::RenderOperation m_RenderOperation;
		bool m_HWBufferInitialized;
		int m_MaxLines;

		void FillLines(Viewport* viewport, bool absolutePos);
	};
}

#endif // __WmeLineElement_H__
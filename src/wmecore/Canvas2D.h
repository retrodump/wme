// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCanvas2D_H__
#define __WmeCanvas2D_H__


#include "Element2D.h"


namespace Wme
{
	class Viewport;
	class SceneNode2D;
	
	class WmeDllExport Canvas2D : public Ogre::MovableObject
	{
	public:
		Canvas2D(const WideString& name, Viewport* viewport = NULL);
		virtual ~Canvas2D();

		Viewport* GetViewport() const { return m_Viewport; }
		void SetViewport(Viewport* viewport);

		float GetWidth() const;
		float GetHeight() const;

		SceneNode2D* GetRootNode();

		void GetElementsAt(float x, float y, Element2DList& elements) const;
		Element2D* GetElementAt(float x, float y) const;


		// Ogre::MovableObject implementation
		virtual const Ogre::String& getMovableType(void) const;
		virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const;
		virtual Ogre::Real getBoundingRadius(void) const;
		virtual void _updateRenderQueue(Ogre::RenderQueue* queue);
		virtual void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false);

	private:
		Viewport* m_Viewport;
		SceneNode2D* m_RootNode;

		bool IsCurrentViewport() const;

		int m_LastViewportWidth;
		int m_LastViewportHeight;

	};
}

#endif // __WmeCanvas2D_H__

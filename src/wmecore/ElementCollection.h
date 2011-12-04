// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteCollection_H__
#define __WmeSpriteCollection_H__

#include "OgreMovableObject.h"


namespace Wme
{
	class IRenderableProvider;

	class WmeDllExport ElementCollection : public Ogre::MovableObject
	{
	public:
		ElementCollection();
		~ElementCollection();

		void ClearQueue();
		void AddElement(IRenderableProvider* element);

		
		// Ogre::MovableObject implementation
		virtual const Ogre::String& getMovableType(void) const;
		virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const;
		virtual Ogre::Real getBoundingRadius(void) const;
		virtual void _updateRenderQueue(Ogre::RenderQueue* queue);
		virtual void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false);

	private:
		Ogre::AxisAlignedBox m_BoundingBox;
		
		typedef std::list<IRenderableProvider*> ElementList;
		ElementList m_Elements;
	};
}

#endif // __WmeSpriteCollection_H__

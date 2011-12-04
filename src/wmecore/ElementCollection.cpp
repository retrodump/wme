// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ElementCollection.h"
#include "IRenderableProvider.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ElementCollection::ElementCollection()
{

}

//////////////////////////////////////////////////////////////////////////
ElementCollection::~ElementCollection()
{
}

//////////////////////////////////////////////////////////////////////////
void ElementCollection::ClearQueue()
{
	m_Elements.clear();
}

//////////////////////////////////////////////////////////////////////////
void ElementCollection::AddElement(IRenderableProvider* element)
{
	m_Elements.push_back(element);
}



//////////////////////////////////////////////////////////////////////////
// Ogre::MovableObject
//////////////////////////////////////////////////////////////////////////
const Ogre::String& ElementCollection::getMovableType(void) const
{
	static Ogre::String movType = "ElementCollection";
	return movType;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::AxisAlignedBox& ElementCollection::getBoundingBox(void) const
{
	static Ogre::AxisAlignedBox box;
	box.setInfinite();

	return box;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real ElementCollection::getBoundingRadius(void) const
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void ElementCollection::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	byte priority = 0;

	IRenderableProvider::RenderableList renderableList;
	foreach (IRenderableProvider* item, m_Elements)
	{
		item->GetRenderables(renderableList);
	}

	foreach (Ogre::Renderable* renderable, renderableList)
	{
		queue->addRenderable(renderable, mRenderQueueID, priority);
		priority++;
	}
}

//////////////////////////////////////////////////////////////////////////
void ElementCollection::visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables)
{
	IRenderableProvider::RenderableList renderableList;
	foreach (IRenderableProvider* item, m_Elements)
	{
		item->GetRenderables(renderableList);
	}

	foreach (Ogre::Renderable* renderable, renderableList)
	{
		visitor->visit(renderable, 0, false);
	}
}


} // namespace Wme

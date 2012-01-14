// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Canvas2D.h"
#include "Viewport.h"
#include "View.h"
#include "Transform2D.h"
#include "SceneNode2D.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Canvas2D::Canvas2D(const WideString& name, Viewport* viewport) : Ogre::MovableObject(StringUtil::WideToUtf8(name))
{
	m_Viewport = viewport;
	m_LastViewportWidth = m_LastViewportHeight = 0;
	
	m_RootNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
Canvas2D::~Canvas2D()
{
	SAFE_DELETE(m_RootNode);
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D* Canvas2D::GetRootNode()
{
	if (!m_RootNode)
	{
		m_RootNode = new SceneNode2D(this);
	}
	return m_RootNode;
}

//////////////////////////////////////////////////////////////////////////
bool Canvas2D::IsCurrentViewport() const
{
	View* view = m_Viewport->GetParentView();
	if (!view) return false;

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	return view->OgreToWmeViewport(rs->_getViewport()) == m_Viewport;
}

//////////////////////////////////////////////////////////////////////////
void Canvas2D::GetElementsAt(float x, float y, Element2DList& elements) const
{
	if (m_RootNode) m_RootNode->GetElementsAt(x, y, elements);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::MovableObject
//////////////////////////////////////////////////////////////////////////
const Ogre::String& Canvas2D::getMovableType(void) const
{
	static Ogre::String movType = "Canvas2D";
	return movType;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::AxisAlignedBox& Canvas2D::getBoundingBox(void) const
{
	static Ogre::AxisAlignedBox box;
	box.setInfinite();

	return box;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real Canvas2D::getBoundingRadius(void) const
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void Canvas2D::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	if (!IsCurrentViewport()) return;

	if (m_LastViewportWidth != m_Viewport->GetOgreViewport()->getActualWidth() || m_LastViewportHeight != m_Viewport->GetOgreViewport()->getActualHeight())
	{
		if (m_RootNode) m_RootNode->SetGeometryDirty(true);

		m_LastViewportWidth = m_Viewport->GetOgreViewport()->getActualWidth();
		m_LastViewportHeight = m_Viewport->GetOgreViewport()->getActualHeight();
	}


	word priority = 0;
	if (m_RootNode) m_RootNode->Render(queue, mRenderQueueID, priority);	
}

//////////////////////////////////////////////////////////////////////////
void Canvas2D::visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables)
{
	if (m_RootNode) m_RootNode->VisitRenderables(visitor);
}


} // namespace Wme

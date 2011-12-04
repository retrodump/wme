// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "View.h"
#include "Viewport.h"
#include "ViewportLayout.h"
#include "OgreViewport.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
View::View()
{
	m_ViewportLayout = NULL;
	m_TopLevelWindow = true;
}

//////////////////////////////////////////////////////////////////////////
View::~View()
{
	SAFE_DELETE(m_ViewportLayout);
}

//////////////////////////////////////////////////////////////////////////
void View::Initialize()
{
	m_ViewportLayout = new ViewportLayout(this);
}

//////////////////////////////////////////////////////////////////////////
void View::Update()
{
	if (!GetRenderTarget()->isAutoUpdated()) return;
	if (m_ViewportLayout) m_ViewportLayout->Update();
}

//////////////////////////////////////////////////////////////////////////
void View::RemoveAllOgreViewports()
{
	GetRenderTarget()->removeAllViewports();
	m_ViewportMap.clear();
}

//////////////////////////////////////////////////////////////////////////
void View::AddViewportToMap(Viewport* viewport, Ogre::Viewport* ogreViewport)
{
	m_ViewportMap[ogreViewport] = viewport;
}

//////////////////////////////////////////////////////////////////////////
Viewport* View::OgreToWmeViewport(Ogre::Viewport* ogreViewport)
{
	ViewportMap::iterator it = m_ViewportMap.find(ogreViewport);
	if (it == m_ViewportMap.end()) return NULL;
	else return it->second;
}


//////////////////////////////////////////////////////////////////////////
// Ogre::RenderTargetListener implementation
//////////////////////////////////////////////////////////////////////////
void View::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
	Viewport* viewport = OgreToWmeViewport(evt.source);
	if (viewport) viewport->PreDisplay();
}

//////////////////////////////////////////////////////////////////////////
void View::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
	Viewport* viewport = OgreToWmeViewport(evt.source);
	if (viewport) viewport->PostDisplay();
}

//////////////////////////////////////////////////////////////////////////
void View::OnRenderQueueStart(Ogre::uint8 renderQueueId, Ogre::Viewport* viewport)
{
	Viewport* vp = OgreToWmeViewport(viewport);
	if (vp) vp->OnRenderQueueStart(renderQueueId);
}


} // namespace Wme

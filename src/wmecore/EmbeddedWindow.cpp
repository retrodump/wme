// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "EmbeddedWindow.h"
#include "EmbeddedWindowBridge.h"
#include "ViewportLayout.h"
#include "Exception.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
EmbeddedWindow::EmbeddedWindow(EmbeddedWindowBridge* bridge)
{
	m_Bridge = bridge;
	m_OgreWindow = NULL;
}

//////////////////////////////////////////////////////////////////////////
EmbeddedWindow::~EmbeddedWindow()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////
void EmbeddedWindow::Create(const WideString& name)
{
	if (m_OgreWindow)
	{
		throw Exception(L"Window is already created");
	}

	size_t width, height;
	m_Bridge->GetWindowSize(width, height);

	Ogre::NameValuePairList params;
	params["externalWindowHandle"] = m_Bridge->GetWindowId();
	params["FSAA"] = "2"; // !!!

	m_OgreWindow = Ogre::Root::getSingleton().createRenderWindow(
		StringUtil::WideToUtf8(name),
		width,
		height,
		false,
		&params);

	m_OgreWindow->addListener(this);
	m_Name = name;
}

//////////////////////////////////////////////////////////////////////////
void EmbeddedWindow::Destroy()
{
	if (m_OgreWindow)
	{
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
		rs->destroyRenderWindow(m_OgreWindow->getName());
		m_OgreWindow = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void EmbeddedWindow::OnResize()
{
	if (!m_OgreWindow) return;

	/*
	size_t width, height;
	m_Bridge->GetWindowSize(width, height);
	m_OgreWindow->resize(width, height);
	*/
		
	m_OgreWindow->windowMovedOrResized();
	GetViewportLayout()->InvalidateViewports();
	GetViewportLayout()->OnResizeViewports();
}

//////////////////////////////////////////////////////////////////////////
void EmbeddedWindow::GetMousePos(int& x, int& y) const
{
	m_Bridge->GetMousePos(x, y);
}

//////////////////////////////////////////////////////////////////////////
void EmbeddedWindow::SetMousePos(int x, int y)
{
	m_Bridge->SetMousePos(x, y);
}

//////////////////////////////////////////////////////////////////////////
Ogre::RenderTarget* EmbeddedWindow::GetRenderTarget()
{
	return m_OgreWindow;
}

//////////////////////////////////////////////////////////////////////////
Ogre::RenderWindow* EmbeddedWindow::GetOgreWindow()
{
	return m_OgreWindow;
}

//////////////////////////////////////////////////////////////////////////
bool EmbeddedWindow::IsShiftDown() const
{
	return m_Bridge->IsShiftDown();
}

//////////////////////////////////////////////////////////////////////////
bool EmbeddedWindow::IsControlDown() const
{
	return m_Bridge->IsControlDown();
}

//////////////////////////////////////////////////////////////////////////
bool EmbeddedWindow::IsAltDown() const
{
	return m_Bridge->IsAltDown();
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MainWindow.h"
#include "Exception.h"
#include "OgreRenderWindow.h"
#include "GameStartupSettings.h"
#include "StringUtil.h"
#include "InputManager.h"
#include "Game.h"
#include "ContentManager.h"
#include "ViewportLayout.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
{
	m_OgreWindow = NULL;
	m_InputMgr = NULL;

#ifdef WIN32
	m_Win32Window = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
	Destroy();
}


//////////////////////////////////////////////////////////////////////////
Ogre::RenderTarget* MainWindow::GetRenderTarget()
{
	return m_OgreWindow;
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::Create(const WideString& name, const GameStartupSettings& startupSettings)
{
	if (m_OgreWindow)
	{
		throw Exception(L"Window is already created");
	}

	Ogre::NameValuePairList params;
	params["FSAA"] = StringUtil::WideToUtf8(StringUtil::ToString(startupSettings.MultisampleLevel));
	params["title"] = StringUtil::WideToUtf8(startupSettings.Title);
	params["colourDepth"] = startupSettings.ColorDepth16bit ? "16" : "32";
	params["useNVPerfHUD"] = "true";

	m_OgreWindow = Ogre::Root::getSingleton().createRenderWindow(
		StringUtil::WideToUtf8(name),
		startupSettings.ResolutionWidth,
		startupSettings.ResolutionHeight,
		startupSettings.FullScreen,
		&params);

	Ogre::WindowEventUtilities::addWindowEventListener(m_OgreWindow, this);
	m_OgreWindow->addListener(this);
	m_Name = name;

#ifdef WIN32
	m_Win32Window = new Win32WindowExt();
	m_Win32Window->AttachToWindow(m_OgreWindow);
#endif

	m_InputMgr = new InputManager();
	m_InputMgr->Initialize(this);
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::Destroy()
{
	SAFE_DELETE(m_InputMgr);

#ifdef WIN32
	if (m_Win32Window)
	{
		SAFE_DELETE(m_Win32Window);
	}
#endif

	if (m_OgreWindow)
	{
		// the following code kills ogre viewports, so we must tell our viewports first
		m_ViewportLayout->ReleaseOgreViewports();

		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
		rs->destroyRenderWindow(m_OgreWindow->getName());
		m_OgreWindow = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::ProcessFrame()
{
	if (m_InputMgr) m_InputMgr->ProcessFrame();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::GetMousePos(int& x, int& y) const
{
#ifdef WIN32
	if (m_Win32Window) m_Win32Window->GetMousePos(x, y);
#else
	if (m_InputMgr) return m_InputMgr->GetMousePos(x, y);
#endif
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::SetMousePos(int x, int y)
{
#ifdef WIN32
	if (m_Win32Window) m_Win32Window->SetMousePos(x, y);
#else
	if (m_InputMgr) m_InputMgr->SetMousePos(x, y);
#endif
}

//////////////////////////////////////////////////////////////////////////
bool MainWindow::IsShiftDown() const
{
	if (m_InputMgr) return m_InputMgr->IsShiftDown();
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool MainWindow::IsControlDown() const
{
	if (m_InputMgr) return m_InputMgr->IsControlDown();
	else return false;

}

//////////////////////////////////////////////////////////////////////////
bool MainWindow::IsAltDown() const
{
	if (m_InputMgr) return m_InputMgr->IsAltDown();
	else return false;
}

//////////////////////////////////////////////////////////////////////////
// Ogre::WindowEventListener
//////////////////////////////////////////////////////////////////////////
void MainWindow::windowClosed(Ogre::RenderWindow* rw)
{

}

//////////////////////////////////////////////////////////////////////////
void MainWindow::windowResized(Ogre::RenderWindow* rw)
{
	// TODO only handle affected viewports!
	Game::GetInstance()->GetContentMgr()->InvalidateViewports(); 
	Game::GetInstance()->GetContentMgr()->OnResizeViewports(); 
}



} // namespace Wme

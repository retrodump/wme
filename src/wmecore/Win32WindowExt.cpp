// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Game.h"
#include "Win32WindowExt.h"
#include "InputManager.h"
#include "ContentManager.h"
#include "OgreRenderWindow.h"
#include "MouseEvent.h"


#ifdef WIN32
#include <windowsx.h>

namespace Wme
{


FARPROC Win32WindowExt::s_OldWindowProc = NULL;
int Win32WindowExt::s_CaptureCount = 0;

//////////////////////////////////////////////////////////////////////////
Win32WindowExt::Win32WindowExt()
{
	m_OgreWindow = NULL;
}

//////////////////////////////////////////////////////////////////////////
Win32WindowExt::~Win32WindowExt()
{
	DetachFromWindow();
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::AttachToWindow(Ogre::RenderWindow* window)
{
	DetachFromWindow();

	m_OgreWindow = window;
	s_OldWindowProc = (FARPROC)::SetWindowLongPtr(GetWindowHandle(), GWL_WNDPROC, (DWORD)WindowProc);
	Ogre::WindowEventUtilities::addWindowEventListener(m_OgreWindow, this);

	// make sure the window handles double-click
	DWORD classStyle = ::GetClassLong(GetWindowHandle(), GCL_STYLE);
	classStyle |= CS_DBLCLKS;
	::SetClassLong(GetWindowHandle(), GCL_STYLE, classStyle);

	// set icon (try to find first icon resource in the exe file)
	::EnumResourceNames(::GetModuleHandle(NULL), MAKEINTRESOURCE(RT_GROUP_ICON), EnumIconNameProc, (LONG_PTR)GetWindowHandle());
}

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK Win32WindowExt::EnumIconNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
	HICON hIcon = ::LoadIcon(hModule, lpszName);
	if (hIcon)
	{
		::SendMessage((HWND)lParam, WM_SETICON, ICON_BIG,   (LPARAM)hIcon);
		::SendMessage((HWND)lParam, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::DetachFromWindow()
{
	if (s_OldWindowProc)
	{
		::SetWindowLongPtr(GetWindowHandle(), GWL_WNDPROC, (DWORD)s_OldWindowProc);
		s_OldWindowProc = NULL;
	}

	if (m_OgreWindow)
	{
		// ogre automatically removes the listeners on window close
		//Ogre::WindowEventUtilities::removeWindowEventListener(m_OgreWindow, this);

		m_OgreWindow = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::windowClosed(Ogre::RenderWindow* rw)
{
	DetachFromWindow();
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::windowResized(Ogre::RenderWindow* rw)
{
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::GetMousePos(int& x, int& y) const
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(GetWindowHandle(), &p);

	x = p.x;
	y = p.y;
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::SetMousePos(int x, int y)
{
	POINT p;
	p.x = x;
	p.y = y;

	::ClientToScreen(GetWindowHandle(), &p);
	::SetCursorPos(p.x, p.y);
}

//////////////////////////////////////////////////////////////////////////
HWND Win32WindowExt::GetWindowHandle() const
{
	if (!m_OgreWindow) return NULL;

	size_t windowHnd = 0;
	m_OgreWindow->getCustomAttribute("WINDOW", &windowHnd);
	return (HWND)windowHnd;
}

//////////////////////////////////////////////////////////////////////////
LRESULT FAR PASCAL Win32WindowExt::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Ogre::RenderWindow* win = (Ogre::RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!win) return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

	int x, y;

	switch (uMsg)
	{
	case WM_SETCURSOR:
		if (win->isActive() && LOWORD(lParam) == HTCLIENT)
		{
			::SetCursor(NULL);
			return TRUE;
		}
		break;

	case WM_LBUTTONDOWN:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_PRESS, MouseEvent::BUTTON_LEFT, x, y));
		CaptureMouse(hWnd);
		break;

	case WM_LBUTTONUP:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_RELEASE, MouseEvent::BUTTON_LEFT, x, y));
		ReleaseMouse();
		break;

	case WM_LBUTTONDBLCLK:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_DOUBLE_CLICK, MouseEvent::BUTTON_LEFT, x, y));
		break;

	case WM_RBUTTONDOWN:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_PRESS, MouseEvent::BUTTON_RIGHT, x, y));
		CaptureMouse(hWnd);
		break;

	case WM_RBUTTONUP:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_RELEASE, MouseEvent::BUTTON_RIGHT, x, y));
		ReleaseMouse();
		break;

	case WM_RBUTTONDBLCLK:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_DOUBLE_CLICK, MouseEvent::BUTTON_RIGHT, x, y));
		break;

	case WM_MBUTTONDOWN:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_PRESS, MouseEvent::BUTTON_MIDDLE, x, y));
		CaptureMouse(hWnd);
		break;

	case WM_MBUTTONUP:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_RELEASE, MouseEvent::BUTTON_MIDDLE, x, y));
		ReleaseMouse();
		break;

	case WM_MBUTTONDBLCLK:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(MouseEvent::EVENT_DOUBLE_CLICK, MouseEvent::BUTTON_MIDDLE, x, y));
		break;

	case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		
			POINT p;
			p.x = GET_X_LPARAM(lParam); 
			p.y = GET_Y_LPARAM(lParam); 

			::ScreenToClient(hWnd, &p);
			Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(win, MouseEvent(delta < 0 ? MouseEvent::EVENT_WHEEL_DOWN : MouseEvent::EVENT_WHEEL_UP, MouseEvent::BUTTON_NONE, p.x, p.y));
		}
		break;

	case WM_CAPTURECHANGED:
		if (hWnd != (HWND)lParam) s_CaptureCount = 0;
		break;

	}

	if (s_OldWindowProc) return CallWindowProc((WNDPROC)s_OldWindowProc, hWnd, uMsg, wParam, lParam);
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::CaptureMouse(HWND win)
{
	s_CaptureCount++;
	::SetCapture(win);
}

//////////////////////////////////////////////////////////////////////////
void Win32WindowExt::ReleaseMouse()
{
	s_CaptureCount--;
	if (s_CaptureCount <= 0)
	{
		s_CaptureCount = 0;
		::ReleaseCapture();
	}
}


} // namespace Wme

#endif // WIN32

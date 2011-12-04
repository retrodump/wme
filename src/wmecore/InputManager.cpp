// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "InputManager.h"
#include "MainWindow.h"
#include "Game.h"
#include "ContentManager.h"


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment (lib, "OIS_static_d.lib")
#	else
#		pragma comment (lib, "OIS_static.lib")
#	endif
#endif


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
InputManager::InputManager()
{
	m_Window = NULL;
	m_InputManager = NULL;
	m_Mouse = NULL;
	m_Keyboard = NULL;
}

//////////////////////////////////////////////////////////////////////////
InputManager::~InputManager()
{
}

//////////////////////////////////////////////////////////////////////////
void InputManager::Initialize(MainWindow* window)
{

	m_Window = window;

	OIS::ParamList params;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	Ogre::WindowEventUtilities::addWindowEventListener(window->GetOgreWindow(), this);

	window->GetOgreWindow()->getCustomAttribute("WINDOW", &windowHnd);
	params.insert(std::make_pair(std::string("WINDOW"), Ogre::StringConverter::toString(windowHnd)));

	m_InputManager = OIS::InputManager::createInputSystem(params);
	m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, true));
	m_Keyboard->setEventCallback(this);

#ifndef WIN32
	m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, true));
	m_Mouse->setEventCallback(this);
	
	const OIS::MouseState& ms = m_Mouse->getMouseState();
	ms.width = m_Window->GetOgreWindow()->getWidth();
	ms.height = m_Window->GetOgreWindow()->getHeight();
#endif

}

//////////////////////////////////////////////////////////////////////////
void InputManager::ProcessFrame()
{
	if (m_InputManager)
	{
		if (m_Keyboard) m_Keyboard->capture();
		if (m_Mouse) m_Mouse->capture();
	}
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::IsShiftDown()
{
	if (!m_Keyboard) return false;
	else return m_Keyboard->isModifierDown(OIS::Keyboard::Shift);
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::IsControlDown()
{
	if (!m_Keyboard) return false;
	else return m_Keyboard->isModifierDown(OIS::Keyboard::Ctrl);
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::IsAltDown()
{
	if (!m_Keyboard) return false;
	else return m_Keyboard->isModifierDown(OIS::Keyboard::Alt);
}

//////////////////////////////////////////////////////////////////////////
void InputManager::GetMousePos(int& x, int& y)
{
	if (!m_Mouse)
	{
		x = y = 0;
		return;
	}
	const OIS::MouseState& state = m_Mouse->getMouseState();
	x = state.X.abs;
	y = state.Y.abs;
}

//////////////////////////////////////////////////////////////////////////
void InputManager::SetMousePos(int x, int y)
{
	if (!m_Mouse) return;

	// FIXME
	OIS::MouseState& mutableMouseState = const_cast<OIS::MouseState&>(m_Mouse->getMouseState());
	mutableMouseState.X.abs = x;
	mutableMouseState.Y.abs = y;
}

//////////////////////////////////////////////////////////////////////////
// OIS::MouseListener
//////////////////////////////////////////////////////////////////////////
bool InputManager::mouseMoved(const OIS::MouseEvent& arg)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if (!m_Mouse) return false;
	else
	{
		MouseEvent event(MouseEvent::EVENT_PRESS, OisToWmeButton(id), arg.state.X.abs, arg.state.Y.abs);
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_Window, event);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if (!m_Mouse) return false;
	else
	{
		MouseEvent event(MouseEvent::EVENT_RELEASE, OisToWmeButton(id), arg.state.X.abs, arg.state.Y.abs);
		Game::GetInstance()->GetContentMgr()->InvokeMouseEvent(m_Window, event);
		return true;
	}
	// TODO doubleclick
}

//////////////////////////////////////////////////////////////////////////
MouseEvent::MouseButton InputManager::OisToWmeButton(OIS::MouseButtonID buttonId)
{
	MouseEvent::MouseButton button;

	switch (buttonId)
	{
	case OIS::MB_Left:
		button = MouseEvent::BUTTON_LEFT;
		break;
	case OIS::MB_Right:
		button = MouseEvent::BUTTON_RIGHT;
		break;
	case OIS::MB_Middle:
		button = MouseEvent::BUTTON_MIDDLE;
		break;
	default:
		button = MouseEvent::BUTTON_NONE;
	}
	return button;
}

//////////////////////////////////////////////////////////////////////////
// OIS::KeyListener
//////////////////////////////////////////////////////////////////////////
bool InputManager::keyPressed(const OIS::KeyEvent& arg)
{
	KeyboardEvent event(KeyboardEvent::EVENT_PRESS, m_Keyboard, arg);
	Game::GetInstance()->GetContentMgr()->InvokeKeyboardEvent(m_Window, event);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool InputManager::keyReleased(const OIS::KeyEvent& arg)
{
	KeyboardEvent event(KeyboardEvent::EVENT_RELEASE, m_Keyboard, arg);
	Game::GetInstance()->GetContentMgr()->InvokeKeyboardEvent(m_Window, event);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Ogre::WindowEventListener
//////////////////////////////////////////////////////////////////////////
void InputManager::windowClosed(Ogre::RenderWindow* rw)
{
	if (m_InputManager)
	{
		m_InputManager->destroyInputObject(m_Mouse);
		m_InputManager->destroyInputObject(m_Keyboard);

		OIS::InputManager::destroyInputSystem(m_InputManager);
		m_InputManager = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void InputManager::windowResized(Ogre::RenderWindow* rw)
{
	if (m_InputManager)
	{
		if (m_Mouse)
		{
			const OIS::MouseState& ms = m_Mouse->getMouseState();
			ms.width = m_Window->GetOgreWindow()->getWidth();
			ms.height = m_Window->GetOgreWindow()->getHeight();
		}
	}
}


} // namespace Wme

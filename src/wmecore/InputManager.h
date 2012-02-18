// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeInputManager_H__
#define __WmeInputManager_H__

#include "Object.h"
#include "MouseEvent.h"

#ifdef __GNUC__
#include <OIS.h>
#else
#include <ois.h>
#endif

namespace Wme
{
	class MainWindow;

	class WmeDllExport InputManager : public Object, public OIS::MouseListener, public OIS::KeyListener, public Ogre::WindowEventListener
	{
	public:
		InputManager();
		~InputManager();

		void Initialize(MainWindow* window);
		void ProcessFrame();

		bool IsShiftDown();
		bool IsControlDown();
		bool IsAltDown();

		void GetMousePos(int& x, int& y);
		void SetMousePos(int x, int y);

		static MouseEvent::MouseButton OisToWmeButton(OIS::MouseButtonID buttonId);

		// OIS::MouseListener implementation
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
		

		// OIS::KeyListener implementation
		bool keyPressed(const OIS::KeyEvent& arg);
		bool keyReleased(const OIS::KeyEvent& arg);


		// Ogre::WindowEventListener
		void windowClosed(Ogre::RenderWindow* rw);
		void windowResized(Ogre::RenderWindow* rw);

	private:
		MainWindow* m_Window;
		OIS::InputManager* m_InputManager;
		OIS::Mouse* m_Mouse;
		OIS::Keyboard* m_Keyboard;
	};

}


#endif // __WmeInputManager_H__

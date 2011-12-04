// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMainWindow_H__
#define __WmeMainWindow_H__


#include "View.h"
#include "Win32WindowExt.h"


namespace Wme
{
	class Ogre::RenderWindow;
	class GameStartupSettings;
	class InputManager;

	class WmeDllExport MainWindow : public View, public Ogre::WindowEventListener
	{
	public:
		MainWindow();
		~MainWindow();

		virtual Ogre::RenderWindow* GetOgreWindow() { return m_OgreWindow; }
		virtual Ogre::RenderTarget* GetRenderTarget();

		void ProcessFrame();

		void Create(const WideString& name, const GameStartupSettings& startupSettings);
		void Destroy();

		virtual void GetMousePos(int& x, int& y) const;
		virtual void SetMousePos(int x, int y);

		virtual bool IsShiftDown() const;
		virtual bool IsControlDown() const;
		virtual bool IsAltDown() const;

		// Ogre::WindowEventListener
		void windowClosed(Ogre::RenderWindow* rw);
		void windowResized(Ogre::RenderWindow* rw);

	private:
		Ogre::RenderWindow* m_OgreWindow;
		InputManager* m_InputMgr;

#ifdef WIN32
		Win32WindowExt* m_Win32Window;
#endif

	};
}

#endif // __WmeMainWindow_H__
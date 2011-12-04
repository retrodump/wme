// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeWin32WindowExt_H__
#define __WmeWin32WindowExt_H__

#ifdef WIN32

namespace Wme
{
	class Ogre::RenderWindow;

	class WmeDllExport Win32WindowExt : public Ogre::WindowEventListener
	{
	public:
		Win32WindowExt();
		~Win32WindowExt();

		void AttachToWindow(Ogre::RenderWindow* window);
		void DetachFromWindow();

		void GetMousePos(int& x, int& y) const;
		void SetMousePos(int x, int y);

		// Ogre::WindowEventListener
		void windowClosed(Ogre::RenderWindow* rw);
		void windowResized(Ogre::RenderWindow* rw);
		
	private:
		HWND GetWindowHandle() const;
		Ogre::RenderWindow* m_OgreWindow;

		static FARPROC s_OldWindowProc;
		static LRESULT FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK EnumIconNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);

		static int s_CaptureCount;
		static void CaptureMouse(HWND win);
		static void ReleaseMouse();
	};
}

#endif // WIN32

#endif // __WmeWin32WindowExt_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEmbeddedWindow_H__
#define __WmeEmbeddedWindow_H__


#include "View.h"


namespace Wme
{
	class EmbeddedWindowBridge;
	class Ogre::RenderWindow;

	class WmeDllExport EmbeddedWindow :	public View
	{
	public:
		EmbeddedWindow(EmbeddedWindowBridge* bridge);
		~EmbeddedWindow();

		void Create(const WideString& name);
		void Destroy();
		void OnResize();

		void GetMousePos(int& x, int& y) const;
		void SetMousePos(int x, int y);
		Ogre::RenderTarget* GetRenderTarget();
		Ogre::RenderWindow* GetOgreWindow();

		bool IsShiftDown() const;
		bool IsControlDown() const;
		bool IsAltDown() const;

	private:
		EmbeddedWindowBridge* m_Bridge;
		Ogre::RenderWindow* m_OgreWindow;
	};
}

#endif // __WmeEmbeddedWindow_H__

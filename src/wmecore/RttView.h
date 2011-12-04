// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRttView_H__
#define __WmeRttView_H__


#include "View.h"


namespace Wme
{
	class WmeDllExport RttView : public View
	{
	public:
		RttView();
		~RttView();

		void Create(const WideString& name, size_t width, size_t height);
		Ogre::RenderTarget* GetRenderTarget();
		Ogre::RenderWindow* GetOgreWindow() { return NULL; }

		void GetMousePos(int& x, int& y) const;
		void SetMousePos(int x, int y);

		void InjectMousePos(float relativeX, float relativeY);

	private:
		Ogre::TexturePtr m_Texture;

		int m_InjectedMouseX;
		int m_InjectedMouseY;
	};
}

#endif // __WmeRttView_H__

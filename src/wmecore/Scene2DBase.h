// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene2DBase_H__
#define __WmeScene2DBase_H__


#include "Scene.h"


namespace Wme
{
	class Viewport;
	class Canvas2D;

	// temp
	class TextureElement2D;
	class Sprite;

	class WmeDllExport Scene2DBase : public Scene
	{
	public:
		Scene2DBase();
		virtual ~Scene2DBase();

		virtual void Create();
		virtual void Update();
		virtual void Shutdown();

		virtual void RegisterActiveSpots(Viewport* viewport, Camera* camera, Ogre::uint8 renderQueueId);

		Viewport* GetViewport() const { return m_Viewport; }
		void SetViewport(Viewport* viewport);


	protected:
		Canvas2D* m_Canvas;
		Viewport* m_Viewport;

		// temp
		TextureElement2D* m_Tex;
		Sprite* m_TestSprite;
	};
}

#endif // __WmeScene2DBase_H__

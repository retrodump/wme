// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene2DBase_H__
#define __WmeScene2DBase_H__


#include "Scene.h"


namespace Wme
{
	class Viewport;
	class Canvas2D;
	class SceneNode2D;

	// temp
	class TextureElement2D;
	class Sprite;
	class SpriteEntity;
	class Font;
	class TextElement2D;
	class RectElement2D;
	class ResizableImage;
	class ResizableElement2D;
	class UiWindow;
	class UiBorder;

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

		Canvas2D* GetCanvas() const { return m_Canvas; }

		virtual void OnSceneNodeCreated(SceneNode2D* node);
		virtual void OnSceneNodeDestroyed(SceneNode2D* node);
		virtual void OnSceneNodeChanged(SceneNode2D* node);
		virtual void OnSceneGraphDirty();

		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);

	protected:
		Canvas2D* m_Canvas;
		Viewport* m_Viewport;

		// temp
		TextureElement2D* m_Tex;
		Sprite* m_TestSprite;
		SpriteEntity* m_TestEntity;
		Font* m_Font;
		TextElement2D* m_Text;
		SceneNode2D* m_TestNode;
		SceneNode2D* m_RectNode;
		RectElement2D* m_Rect;
		ResizableImage* m_ResizableImage;
		ResizableElement2D* m_Resizable;
		SceneNode2D* m_ResizableNode;
		UiBorder* m_Border;

		UiWindow* m_Window1;
		UiWindow* m_Window2;
	};
}

#endif // __WmeScene2DBase_H__

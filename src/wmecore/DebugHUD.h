// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDebugHUD_H__
#define __WmeDebugHUD_H__


#include "GuiStage.h"


namespace Wme
{
	class Entity3D;
	class Viewport;
	class Canvas2D;
	class LineElement2D;

	class WmeDllExport DebugHUD : public GuiStage
	{
	public:
		DebugHUD();
		~DebugHUD();

		virtual void Create();

		virtual void Render(Viewport* viewport, Camera* camera);
		virtual void Update();

		void CreateCanvas(Viewport* viewport);

	private:
		ElementCollection* m_ElementCol;
		TextElement* m_QuickMsg;


		SpriteSubFrame* m_Arrow;
		TextElement* m_Fps;
		TextElement* m_Info;
		LineElement2D* m_Lines;
		Font* m_Font;

		Entity3D* m_TestObj;
		Sprite* m_TestSprite;
		Sprite* m_Back;
		Region* m_TestRegion;
		UiWindow* m_Window;

		Canvas2D* m_Canvas;

	};
}


#endif // __WmeDebugHUD_H__
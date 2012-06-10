// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGuiStage_H__
#define __WmeGuiStage_H__


#include "Stage.h"
#include "MouseEvent.h"


namespace Wme
{
	class ElementCollection;
	class SpriteSubFrame;
	class TextElement;
	class LineElement;
	class Viewport;	
	class Actor3D;
	class Sprite;
	class Region;
	class Camera;
	class UiWindowOld;
	class Font;

	class WmeDllExport GuiStage : public Stage
	{
	public:

		GuiStage();
		virtual ~GuiStage();

		virtual void Create();

		virtual void Render(Viewport* viewport, Camera* camera);
		virtual void Update();
		virtual void OnViewportCreated(Viewport* viewport);

		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		
        // ScriptableObject
		RTTI(GuiStage);
		virtual WideString ConvertToString() { return L"Gui layer object"; }
		virtual WideString GetTypeName() const { return L"GuiLayer"; }

	private:
	};
}


#endif // __WmeGuiStage_H__
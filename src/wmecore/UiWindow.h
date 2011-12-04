// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiWindow_H__
#define __WmeUiWindow_H__


#include "UiObject.h"


namespace Wme
{
	class GuiStage;
	class ResizableImage;
	class Sprite;
	class ElementCollection;
	class SpriteDrawingParams;
	class RenderModifier;
	class UiObjectStyle;

	class UiWindow : public UiObject
	{
	public:

		enum WindowState
		{
			STATE_NORMAL,
			STATE_DISABLED,
			STATE_INACTIVE
		};


		UiWindow(GuiStage* parentStage);
		~UiWindow();

		static UiObject* CreateInstance(GuiStage* parentStage) { return new UiWindow(parentStage); }


		void Display(ElementCollection* elementCol, const SpriteDrawingParams& params);
		void Update();
		void Create();

		bool GetClickThrough() const { return m_ClickThrough; }
		void SetClickThrough(bool val) { m_ClickThrough = val; }

		bool GetClipContent() const { return m_ClipContent; }
		void SetClipContent(bool val) { m_ClipContent = val; }

		int GetRealWidth() const;
		int GetRealHeight() const;

		// events
		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);

        // ScriptableObject
		RTTI(UiWindow);
		virtual WideString ConvertToString() { return L"Window object"; }
		virtual WideString GetTypeName() const { return L"Window"; }

		// DocumentAwareObject
		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);
		Utf8String GetDocRootName() { return "WmeWindow"; }

	private:
		bool m_ClickThrough;
		bool m_ClipContent;

		RenderModifier* m_Clipper;
		RenderModifier* m_UnClipper;

		UiObjectStyle* m_NormalStyle;
		UiObjectStyle* m_DisabledStyle;
		UiObjectStyle* m_InactiveStyle;

		ResizableImage* GetImage() const;
		Sprite* GetSprite() const;
		UiObjectStyle* GetCurrentStyle() const;
		UiObjectStyle* GetStyle(WindowState state) const;

		bool m_IsDragging;
		Viewport* m_DragViewport;
		int m_DragStartX;
		int m_DragStartY;
	};
}

#endif // __WmeUiWindow_H__

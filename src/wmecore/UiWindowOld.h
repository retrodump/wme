// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiWindowOld_H__
#define __WmeUiWindowOld_H__


#include "UiObjectOld.h"


namespace Wme
{
	class GuiStage;
	class ResizableImageOld;
	class Sprite;
	class ElementCollection;
	class SpriteDrawingParams;
	class RenderModifier;
	class UiObjectStyleOld;

	class UiWindowOld : public UiObjectOld
	{
	public:

		enum WindowState
		{
			STATE_NORMAL,
			STATE_DISABLED,
			STATE_INACTIVE
		};


		UiWindowOld(GuiStage* parentStage);
		~UiWindowOld();

		static UiObjectOld* CreateInstance(GuiStage* parentStage) { return new UiWindowOld(parentStage); }


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
		RTTI(UiWindowOld);
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

		UiObjectStyleOld* m_NormalStyle;
		UiObjectStyleOld* m_DisabledStyle;
		UiObjectStyleOld* m_InactiveStyle;

		ResizableImageOld* GetImage() const;
		Sprite* GetSprite() const;
		UiObjectStyleOld* GetCurrentStyle() const;
		UiObjectStyleOld* GetStyle(WindowState state) const;

		bool m_IsDragging;
		Viewport* m_DragViewport;
		int m_DragStartX;
		int m_DragStartY;
	};
}

#endif // __WmeUiWindowOld_H__

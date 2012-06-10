// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiButtonOld_H__
#define __WmeUiButtonOld_H__


#include "UiObjectOld.h"
#include "Alignment.h"


namespace Wme
{
	class UiObjectStyleOld;
	class ResizableImageOld;
	class Sprite;
	class Font;
	class TextElement;

	class UiButtonOld : public UiObjectOld
	{
	public:

		enum ButtonState
		{
			STATE_NORMAL,
			STATE_DISABLED,
			STATE_PRESSED,
			STATE_HOVER
		};

		UiButtonOld(GuiStage* parentStage);
		~UiButtonOld();

		static UiObjectOld* CreateInstance(GuiStage* parentStage) { return new UiButtonOld(parentStage); }

		void Display(ElementCollection* elementCol, const SpriteDrawingParams& params);
		void Update();
		void Create();

		int GetPressOffsetX() const { return m_PressOffsetX; }
		void SetPressOffsetX(int val) { m_PressOffsetX = val; }

		int GetPressOffsetY() const { return m_PressOffsetY; }
		void SetPressOffsetY(int val) { m_PressOffsetY = val; }

		bool IsPressed() const { return m_IsPressed; }
		void SetPressed(bool val) { m_IsPressed = val; }

		bool IsTwoStateButton() const { return m_TwoStateButton; }
		void SetTwoStateButton(bool val) { m_TwoStateButton = val; }

		Alignment GetAlignment() const { return m_Alignment; }
		void SetAlignment(Alignment val) { m_Alignment = val; }


		// events
		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);

        // ScriptableObject
		RTTI(UiButtonOld);
		virtual WideString ConvertToString() { return L"Button object"; }
		virtual WideString GetTypeName() const { return L"Button"; }

		// DocumentAwareObject
		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);
		Utf8String GetDocRootName() { return "WmeButton"; }


	private:
		bool m_IsBeingPressed;

		bool m_TwoStateButton;
		bool m_IsPressed;

		Alignment m_Alignment;

		int m_PressOffsetX;
		int m_PressOffsetY;

		UiObjectStyleOld* m_NormalStyle;
		UiObjectStyleOld* m_DisabledStyle;
		UiObjectStyleOld* m_PressedStyle;
		UiObjectStyleOld* m_HoverStyle;

		UiObjectStyleOld* GetCurrentStyle() const;
		UiObjectStyleOld* GetStyle(ButtonState state) const;

		void GetPressedAndHover(bool& pressedDown, bool& hover) const;
		void GetSpritePos(Sprite* sprite, bool pressedDown, int& posX, int& posY) const;

		ResizableImageOld* GetImage() const;
		Sprite* GetSprite() const;
		Font* GetFont() const;
		Ogre::ColourValue GetTextColor() const;

		TextElement* m_TextElement;
	};
}

#endif // __WmeUiButtonOld_H__

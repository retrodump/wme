// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiButton_H__
#define __WmeUiButton_H__


#include "UiObject.h"
#include "Alignment.h"


namespace Wme
{
	class UiObjectStyle;
	class ResizableImage;
	class Sprite;
	class Font;
	class TextElement;

	class UiButton : public UiObject
	{
	public:

		enum ButtonState
		{
			STATE_NORMAL,
			STATE_DISABLED,
			STATE_PRESSED,
			STATE_HOVER
		};

		UiButton(GuiStage* parentStage);
		~UiButton();

		static UiObject* CreateInstance(GuiStage* parentStage) { return new UiButton(parentStage); }

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
		RTTI(UiButton);
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

		UiObjectStyle* m_NormalStyle;
		UiObjectStyle* m_DisabledStyle;
		UiObjectStyle* m_PressedStyle;
		UiObjectStyle* m_HoverStyle;

		UiObjectStyle* GetCurrentStyle() const;
		UiObjectStyle* GetStyle(ButtonState state) const;

		void GetPressedAndHover(bool& pressedDown, bool& hover) const;
		void GetSpritePos(Sprite* sprite, bool pressedDown, int& posX, int& posY) const;

		ResizableImage* GetImage() const;
		Sprite* GetSprite() const;
		Font* GetFont() const;
		Ogre::ColourValue GetTextColor() const;

		TextElement* m_TextElement;
	};
}

#endif // __WmeUiButton_H__

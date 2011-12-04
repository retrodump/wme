// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiButton.h"
#include "UiObjectStyle.h"
#include "FontManager.h"
#include "Sprite.h"
#include "ResizableImage.h"
#include "Font.h"
#include "Viewport.h"
#include "ActiveSpot.h"
#include "ContentManager.h"
#include "TextElement.h"
#include "ElementCollection.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiButton::UiButton(GuiStage* parentStage) : UiObject(parentStage)
{
	m_NormalStyle = NULL;
	m_DisabledStyle = NULL;
	m_PressedStyle = NULL;
	m_HoverStyle = NULL;

	m_IsBeingPressed = false;
	
	m_TwoStateButton = false;
	m_IsPressed = false;

	m_PressOffsetX = m_PressOffsetY = 1;

	m_Alignment = Alignment(Alignment::HA_CENTER, Alignment::VA_CENTER);

	m_TextElement = NULL;
}

//////////////////////////////////////////////////////////////////////////
UiButton::~UiButton()
{
	SAFE_DELETE(m_NormalStyle);
	SAFE_DELETE(m_DisabledStyle);
	SAFE_DELETE(m_PressedStyle);
	SAFE_DELETE(m_HoverStyle);

	SAFE_DELETE(m_TextElement);
}

//////////////////////////////////////////////////////////////////////////
void UiButton::Create()
{
	m_NormalStyle = new UiObjectStyle(this);
	m_DisabledStyle = new UiObjectStyle(this);
	m_PressedStyle = new UiObjectStyle(this);
	m_HoverStyle = new UiObjectStyle(this);

	m_TextElement = new TextElement;

	UiObject::Create();
}

//////////////////////////////////////////////////////////////////////////
void UiButton::Display(ElementCollection* elementCol, const SpriteDrawingParams& params)
{
	if (!m_Visible) return;

	Rect buttonRect = GetAbsoluteRect();

	bool pressedDown, hover;
	GetPressedAndHover(pressedDown, hover);


	Sprite* sprite = GetSprite();
	ResizableImage* image = GetImage();
	Font* font = GetFont();
	Ogre::ColourValue textColor = GetTextColor();

	// render content
	if (image) image->Display(elementCol, params);
	if (sprite)
	{
		SpriteDrawingParams newParams = params;
		if (m_PixelPerfect && !m_Disabled)
			newParams.Parent = this;
		else
			newParams.Parent = NULL;

		int spriteX, spriteY;
		GetSpritePos(sprite, pressedDown, spriteX, spriteY);

		sprite->Display(elementCol, spriteX, spriteY, newParams);
	}

	if (!m_Text.empty())
	{
		Rect textRect = buttonRect;
		if (pressedDown)
		{
			textRect.x += m_PressOffsetX;
			textRect.y += m_PressOffsetY;
		}

		m_TextElement->SetText(m_Text);
		m_TextElement->SetFont(font);

		m_TextElement->SetRect(textRect);
		m_TextElement->SetColor(textColor);
		m_TextElement->SetAlignment(m_Alignment);
				
		m_TextElement->Display(elementCol, params);
	}


	// no sprite or not pixel perfect -> add rectangular active spot
	if (!m_Disabled && (!m_PixelPerfect || !sprite))
	{
		params.Viewport->AddActiveSpot(new ActiveSpotRectangle(params.Viewport, this, buttonRect, params.AbsolutePos));
	}

	// render children
	UiObject::Display(elementCol, params);
}

//////////////////////////////////////////////////////////////////////////
void UiButton::Update()
{
	if (!m_Visible) return;

	Sprite* sprite = GetSprite();
	ResizableImage* image = GetImage();


	if (sprite) sprite->Update();

	if (image)
	{
		image->SetPos(GetAbsoluteX(), GetAbsoluteY());
		image->SetSize(GetWidth(), GetHeight());
	}

	UiObject::Update();
}

//////////////////////////////////////////////////////////////////////////
void UiButton::GetPressedAndHover(bool& pressedDown, bool& hover) const
{
	hover = Game::GetInstance()->GetActiveObject() == this;
	pressedDown = (m_IsPressed || (m_IsBeingPressed && hover));
}

//////////////////////////////////////////////////////////////////////////
void UiButton::GetSpritePos(Sprite* sprite, bool pressedDown, int& posX, int& posY) const
{
	Rect spriteRect;
	sprite->GetBoundingRect(spriteRect);

	switch (m_Alignment.GetHorizontal())
	{
	case Alignment::HA_LEFT:
		posX = GetAbsoluteX();
		break;
	case Alignment::HA_RIGHT:
		posX = (int)(GetAbsoluteX() + GetRealWidth() - spriteRect.width);
		break;
	case Alignment::HA_CENTER:
		posX = (int)(GetAbsoluteX() + GetRealWidth() / 2 - spriteRect.width / 2);
		break;
	}

	switch (m_Alignment.GetVertical())
	{
	case Alignment::VA_TOP:
		posY = GetAbsoluteY();
		break;
	case Alignment::VA_BOTTOM:
		posY = (int)(GetAbsoluteY() + GetRealHeight() - spriteRect.height);
		break;
	case Alignment::VA_CENTER:
		posY = (int)(GetAbsoluteY() + GetRealHeight() / 2 - spriteRect.height / 2);
		break;
	}

	if (pressedDown)
	{
		posX += m_PressOffsetX;
		posY += m_PressOffsetY;
	}

}

//////////////////////////////////////////////////////////////////////////
UiObjectStyle* UiButton::GetCurrentStyle() const
{
	bool pressedDown, hover;
	GetPressedAndHover(pressedDown, hover);

	if (m_Disabled) return m_DisabledStyle;
	if (pressedDown) return m_PressedStyle;
	if (hover) return m_HoverStyle;

	return m_NormalStyle;
}

//////////////////////////////////////////////////////////////////////////
UiObjectStyle* UiButton::GetStyle(ButtonState state) const
{
	switch (state)
	{
	case STATE_DISABLED:
		return m_DisabledStyle;
	case STATE_HOVER:
		return m_HoverStyle;
	case STATE_PRESSED:
		return m_PressedStyle;
	default:
		return m_NormalStyle;
	}
}

//////////////////////////////////////////////////////////////////////////
ResizableImage* UiButton::GetImage() const
{
	ResizableImage* ret;

	UiObjectStyle* style = GetCurrentStyle();
	ret = style->GetImage();
	if (!ret) ret = m_NormalStyle->GetImage();
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Sprite* UiButton::GetSprite() const
{
	Sprite* ret;

	UiObjectStyle* style = GetCurrentStyle();
	ret = style->GetSprite();
	if (!ret) ret = m_NormalStyle->GetSprite();
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Font* UiButton::GetFont() const
{
	Font* ret;

	UiObjectStyle* style = GetCurrentStyle();
	ret = style->GetFont();
	if (!ret) ret = m_NormalStyle->GetFont();
	if (!ret) ret = Game::GetInstance()->GetFontMgr()->GetFallbackFont();
	return ret;

	// TODO inherit font from parent
}

//////////////////////////////////////////////////////////////////////////
Ogre::ColourValue UiButton::GetTextColor() const
{
	Ogre::ColourValue ret;

	UiObjectStyle* style = GetCurrentStyle();
	ret = style->GetTextColor();
	if (ret == Ogre::ColourValue::ZERO) ret = m_NormalStyle->GetTextColor();
	if (ret == Ogre::ColourValue::ZERO) ret = Ogre::ColourValue::Black;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool UiButton::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	if (m_Disabled) return false;

	event.TranslateToViewport(viewport);

	int x = event.GetPosX() - GetAbsoluteX();
	int y = event.GetPosY() - GetAbsoluteY();

	if (event.GetButton() == MouseEvent::BUTTON_LEFT)
	{
		if (event.GetType() == MouseEvent::EVENT_PRESS)
		{
			Game::GetInstance()->GetContentMgr()->CaptureMouse(this, viewport);
			m_IsBeingPressed = true;
		}
		else if (event.GetType() == MouseEvent::EVENT_RELEASE)
		{
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(this);
			if (m_IsBeingPressed)
			{
				bool isHover = Game::GetInstance()->GetActiveObject() == this;
				if (isHover)
				{
					if (m_TwoStateButton) m_IsPressed = !m_IsPressed;
					InvokeEvent(L"press");
				}
			}
			m_IsBeingPressed = false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool UiButton::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "PressOffset")
		{
			m_PressOffsetX = XmlUtil::AttrToInt(elem, "X", 0);
			m_PressOffsetY = XmlUtil::AttrToInt(elem, "Y", 0);
		}
		else if (elem->ValueStr() == "TwoStateButton")
		{
			m_TwoStateButton = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "Pressed")
		{
			m_IsPressed = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "Alignment")
		{
			m_Alignment.LoadFromXml(elem);
		}
		// styles
		else if (elem->ValueStr() == "NormalStyle")
		{
			m_NormalStyle->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "HoverStyle")
		{
			m_HoverStyle->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "DisabledStyle")
		{
			m_DisabledStyle->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "PressedStyle")
		{
			m_PressedStyle->LoadFromXml(elem);
		}
	}

	return UiObject::LoadFromXml(rootNode);
}

//////////////////////////////////////////////////////////////////////////
bool UiButton::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("PressOffset", rootNode);
	XmlUtil::SetAttr(elem, "X", m_PressOffsetX);
	XmlUtil::SetAttr(elem, "Y", m_PressOffsetY);

	elem = XmlUtil::AddElem("TwoStateButton", rootNode);
	XmlUtil::SetText(elem, m_TwoStateButton);

	elem = XmlUtil::AddElem("Pressed", rootNode);
	XmlUtil::SetText(elem, m_IsPressed);

	elem = XmlUtil::AddElem("Alignment", rootNode);
	m_Alignment.SaveToXml(elem);

	// styles
	elem = XmlUtil::AddElem("NormalStyle", rootNode);
	m_NormalStyle->SaveToXml(elem);

	elem = XmlUtil::AddElem("HoverStyle", rootNode);
	m_HoverStyle->SaveToXml(elem);

	elem = XmlUtil::AddElem("DisabledStyle", rootNode);
	m_DisabledStyle->SaveToXml(elem);

	elem = XmlUtil::AddElem("PressedStyle", rootNode);
	m_PressedStyle->SaveToXml(elem);


	return UiObject::SaveToXml(rootNode);
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObjectStyle.h"
#include "Sprite.h"
#include "ResizableImage.h"
#include "FontManager.h"
#include "Font.h"
#include "XmlUtil.h"
#include "UiObject.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiObjectStyle::UiObjectStyle(UiObject* owner)
{
	m_Owner = owner;

	m_Sprite = NULL;
	m_Image = NULL;
	m_Font = NULL;
	m_TextColor = Ogre::ColourValue::ZERO;
}

//////////////////////////////////////////////////////////////////////////
UiObjectStyle::~UiObjectStyle()
{
	SAFE_DELETE(m_Sprite);
	SAFE_DELETE(m_Image);
	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);
}

//////////////////////////////////////////////////////////////////////////
void UiObjectStyle::SetSprite(Sprite* sprite)
{
	if (sprite == m_Sprite) return;
	
	SAFE_DELETE(m_Sprite);
	m_Sprite = sprite;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectStyle::SetImage(ResizableImage* image)
{
	if (image == m_Image) return;

	SAFE_DELETE(m_Image);
	m_Image = image;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectStyle::SetFont(Font* font)
{
	if (font == m_Font) return;

	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);
	m_Font = font;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectStyle::LoadSprite(const WideString& fileName)
{
	bool success;
	SAFE_DELETE(m_Sprite);
	m_Sprite = new Sprite(m_Owner);
	try
	{
		success = m_Sprite->LoadFromFile(fileName);
	}
	catch (...)
	{
		success = false;
	}
	if (!success)
	{
		SAFE_DELETE(m_Sprite);
	}
	return success;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectStyle::LoadFont(const WideString& fileName)
{
	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);
	m_Font = Game::GetInstance()->GetFontMgr()->GetFont(fileName, false);

	return m_Font != NULL;
}


//////////////////////////////////////////////////////////////////////////
bool UiObjectStyle::LoadFromXml(TiXmlElement* node)
{
	SAFE_DELETE(m_Image);
	SAFE_DELETE(m_Sprite);
	Game::GetInstance()->GetFontMgr()->ReleaseFont(m_Font);
	m_TextColor = Ogre::ColourValue::ZERO;


	for (TiXmlElement* elem = node->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Image")
		{			
			m_Image = new ResizableImage;
			m_Image->LoadFromXml(elem);
		}
		else if (elem->ValueStr() == "Sprite")
		{
			LoadSprite(XmlUtil::TextToString(elem));
		}
		else if (elem->ValueStr() == "Font")
		{
			LoadFont(XmlUtil::TextToString(elem));
		}
		else if (elem->ValueStr() == "TextColor")
		{
			m_TextColor = XmlUtil::GetColor(elem);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectStyle::SaveToXml(TiXmlElement* node)
{
	TiXmlElement* elem;

	if (m_Image)
	{
		elem = XmlUtil::AddElem("Image", node);
		m_Image->SaveToXml(elem);
	}

	if (m_Sprite)
	{
		elem = XmlUtil::AddElem("Sprite", node);
		XmlUtil::SetFileName(elem, m_Sprite->GetFileName());
	}

	if (m_Font)
	{
		elem = XmlUtil::AddElem("Font", node);
		XmlUtil::SetFileName(elem, m_Font->GetFileName());
	}

	if (m_TextColor != Ogre::ColourValue::ZERO)
	{
		elem = XmlUtil::AddElem("TextColor", node);
		XmlUtil::SetColor(elem, m_TextColor);
	}

	return true;
}


} // namespace Wme

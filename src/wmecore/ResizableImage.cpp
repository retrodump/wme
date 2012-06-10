// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ResizableImage.h"
#include "SpriteTexture.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ResizableImage::ResizableImage()
{
	m_Texture = NULL;

	m_FrameTopHeight = m_FrameBottomHeight = m_FrameLeftWidth = m_FrameRightWidth = 0.0f;

	m_StretchHorizontal = m_StretchVertical = true;
	m_PaintMiddlePart = true;
}

//////////////////////////////////////////////////////////////////////////
ResizableImage::~ResizableImage()
{
	foreach (Listener* listener, m_Listeners) listener->OnDestroy();

	SAFE_DELETE(m_Texture);
}

//////////////////////////////////////////////////////////////////////////
bool ResizableImage::SetTexture(const WideString& fileName, Ogre::ColourValue colorKey)
{
	SAFE_DELETE(m_Texture);

	bool ret;

	m_Texture = new SpriteTexture();
	if (m_Texture->SetTexture(fileName, colorKey)) ret = true;
	else
	{
		SAFE_DELETE(m_Texture);
		ret = false;
	}
	InvokeChanged();
	return ret;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetFrameTopHeight(float val)
{
	if (val != m_FrameTopHeight)
	{
		m_FrameTopHeight = val;
		InvokeChanged();
	}	
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetFrameBottomHeight(float val)
{
	if (val != m_FrameBottomHeight)
	{
		m_FrameBottomHeight = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetFrameLeftWidth(float val)
{
	if (val != m_FrameLeftWidth)
	{
		m_FrameLeftWidth = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetFrameRightWidth(float val)
{
	if (val != m_FrameRightWidth)
	{
		m_FrameRightWidth = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetStretchVertical(bool val)
{
	if (val != m_StretchVertical)
	{
		m_StretchVertical = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetStretchHorizontal(bool val)
{
	if (val != m_StretchHorizontal)
	{
		m_StretchHorizontal = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetPaintMiddlePart(bool val)
{
	if (val != m_PaintMiddlePart)
	{
		m_PaintMiddlePart = val;
		InvokeChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::AddListener(Listener* listener)
{
	if (std::find(m_Listeners.begin(), m_Listeners.end(), listener) == m_Listeners.end())
		m_Listeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::RemoveListener(Listener* listener)
{
	ListenerList::iterator it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if (it != m_Listeners.end()) m_Listeners.erase(it);
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::InvokeChanged() const
{
	foreach (Listener* listener, m_Listeners) listener->OnChanged();
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool ResizableImage::LoadFromXml(TiXmlElement* rootNode)
{
	WideString imageName = L"";
	Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Image")
		{
			imageName = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "ColorKey")
		{
			colorKey = XmlUtil::GetColor(elem);
		}
		else if (elem->ValueStr() == "FrameSize")
		{
			m_FrameLeftWidth = XmlUtil::AttrToFloat(elem, "Left");
			m_FrameRightWidth = XmlUtil::AttrToFloat(elem, "Right");
			m_FrameTopHeight = XmlUtil::AttrToFloat(elem, "Top");
			m_FrameBottomHeight = XmlUtil::AttrToFloat(elem, "Bottom");
		}
		else if (elem->ValueStr() == "StretchVertical")
		{
			m_StretchVertical = XmlUtil::TextToBool(elem);
		}
		else if (elem->ValueStr() == "StretchHorizontal")
		{
			m_StretchHorizontal = XmlUtil::TextToBool(elem);
		}
		else if (elem->ValueStr() == "PaintMiddlePart")
		{
			m_PaintMiddlePart = XmlUtil::TextToBool(elem);
		}
	}

	if (imageName.empty() || !SetTexture(imageName, colorKey)) return false;

	InvokeChanged();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ResizableImage::SaveToXml(TiXmlElement* rootNode)
{
	if (!m_Texture) return false;

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Image", rootNode);
	XmlUtil::SetFileName(elem, m_Texture->GetFileName());

	if (m_Texture->GetColorKey() != Ogre::ColourValue::ZERO)
	{
		elem = XmlUtil::AddElem("ColorKey", rootNode);
		XmlUtil::SetColor(elem, m_Texture->GetColorKey());
	}

	elem = XmlUtil::AddElem("FrameSize", rootNode);
	XmlUtil::SetAttr(elem, "Left", m_FrameLeftWidth);
	XmlUtil::SetAttr(elem, "Right", m_FrameRightWidth);
	XmlUtil::SetAttr(elem, "Top", m_FrameTopHeight);
	XmlUtil::SetAttr(elem, "Bottom", m_FrameBottomHeight);

	elem = XmlUtil::AddElem("StretchVertical", rootNode);
	XmlUtil::SetText(elem, m_StretchVertical);

	elem = XmlUtil::AddElem("StretchHorizontal", rootNode);
	XmlUtil::SetText(elem, m_StretchHorizontal);

	elem = XmlUtil::AddElem("PaintMiddlePart", rootNode);
	XmlUtil::SetText(elem, m_PaintMiddlePart);


	return true;
}


} // namespace Wme

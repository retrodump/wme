// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteFrame.h"
#include "SpriteSubFrame.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SpriteFrame::SpriteFrame()
{
	m_Delay = 0;
}

//////////////////////////////////////////////////////////////////////////
SpriteFrame::~SpriteFrame()
{
	foreach (SpriteSubFrame* subFrame, m_SubFrames)
	{
		SAFE_DELETE(subFrame);
	}
	m_SubFrames.clear();
}

//////////////////////////////////////////////////////////////////////////
void SpriteFrame::Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params) const
{
	SubFrameCollection::const_iterator it;
	for (it = m_SubFrames.begin(); it != m_SubFrames.end(); it++)
	{
		(*it)->Display(elementCol, targetX, targetY, params);
	}
}

//////////////////////////////////////////////////////////////////////////
void SpriteFrame::Interpolate(SpriteFrame* nextFrame, float lerpValue)
{

}

//////////////////////////////////////////////////////////////////////////
void SpriteFrame::GetBoundingRect(Rect& rect) const
{	
	foreach (SpriteSubFrame* subFrame, m_SubFrames)
	{
		Rect subFrameRect;
		subFrame->GetBoundingRect(subFrameRect);
		rect.UnionWith(subFrameRect);
	}
}


//////////////////////////////////////////////////////////////////////////
bool SpriteFrame::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Delay")
		{
			m_Delay = XmlUtil::TextToInt(elem);
		}
		else if (elem->ValueStr() == "SubFrame")
		{
			SpriteSubFrame* subFrame = new SpriteSubFrame();
			if (subFrame->LoadFromXml(elem))
			{
				m_SubFrames.push_back(subFrame);
			}
			else
			{
				SAFE_DELETE(subFrame);
				return false;
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SpriteFrame::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	TiXmlElement* frameNode = XmlUtil::AddElem("Frame", rootNode);

	elem = XmlUtil::AddElem("Delay", frameNode);
	XmlUtil::SetText(elem, (int)m_Delay);

	foreach (SpriteSubFrame* subFrame, m_SubFrames)
	{
		subFrame->SaveToXml(frameNode);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SpriteFrame::LoadFromImage(const WideString& fileName)
{
	SpriteSubFrame* subFrame = new SpriteSubFrame();
	if (!subFrame->LoadFromImage(fileName))
	{
		SAFE_DELETE(subFrame);
		return false;
	}
	m_SubFrames.push_back(subFrame);
	return true;
}


} // namespace Wme

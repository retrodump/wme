// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Sprite.h"
#include "SpriteFrame.h"
#include "InteractiveObject.h"
#include "XmlUtil.h"
#include "PathUtil.h"
#include "SystemInfo.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Sprite::Sprite(InteractiveObject* ownerObject)
{
	m_OwnerObject = ownerObject;

	m_InitialFrame = 0;
	m_IsLooping = false;
	m_IsReversed = false;
	m_IsFinished = false;
	m_FrameStartTime = 0;
	m_CurrentFrame = -1;

	m_IsLoadedFromImage = false;
}

//////////////////////////////////////////////////////////////////////////
Sprite::~Sprite()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
void Sprite::Clear()
{
	FrameCollection::iterator it;
	foreach (SpriteFrame* frame, m_Frames)
	{
		SAFE_DELETE(frame);
	}
	m_Frames.clear();

	m_IsFinished = false;
	m_CurrentFrame = -1;
}

//////////////////////////////////////////////////////////////////////////
SpriteFrame* Sprite::GetCurrentFrame() const
{
	if (m_CurrentFrame >= 0 && m_CurrentFrame < (int)m_Frames.size()) return m_Frames[m_CurrentFrame];
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void Sprite::Restart()
{
	m_IsFinished = false;
	m_CurrentFrame = -1;
}

//////////////////////////////////////////////////////////////////////////
void Sprite::Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params) const
{
	SpriteFrame* currentFrame = GetCurrentFrame();

	if (currentFrame)
	{
		SpriteDrawingParams newParams = params;
		currentFrame->Display(elementCol, targetX, targetY, newParams);
	}
}

//////////////////////////////////////////////////////////////////////////
void Sprite::Update()
{
	if (m_Frames.size() == 0)
	{
		m_CurrentFrame = -1;
		return;
	}

	SpriteFrame* currentFrame = GetCurrentFrame();
	unsigned long elapsedTime = 0;

	// starting up
	if (currentFrame == NULL)
	{
		if (m_InitialFrame >= 0 && m_InitialFrame < m_Frames.size())
			SwitchToFrame(m_InitialFrame);
		else
			SwitchToFrame(0);
	}
	else
	{
		elapsedTime = m_OwnerObject->GetTier()->GetCurrentTime() - m_FrameStartTime;
		if (elapsedTime >= currentFrame->GetDelay())
		{
			SwitchToNextFrame();
			elapsedTime = 0;
		}
	}

	currentFrame = GetCurrentFrame();
	if (currentFrame && currentFrame->GetDelay() > 0)
	{
		// interpolate between current and next frame
		float lerpValue = (float)elapsedTime / (float)currentFrame->GetDelay();
		SpriteFrame* nextFrame = m_Frames[GetNextFrameIndex()];

		currentFrame->Interpolate(nextFrame, lerpValue);
	}
}

//////////////////////////////////////////////////////////////////////////
void Sprite::SwitchToFrame(int frameIndex)
{
	m_CurrentFrame = frameIndex;
	m_FrameStartTime = m_OwnerObject->GetTier()->GetCurrentTime();

	// TODO trigger script event, pre-display action etc.
}

//////////////////////////////////////////////////////////////////////////
void Sprite::SwitchToNextFrame()
{
	// TODO trigger post-display action
	
	bool finished;
	int nextFrame = GetNextFrameIndex(finished);

	if (finished) OnSpriteFinished();
	else SwitchToFrame(nextFrame);
}

//////////////////////////////////////////////////////////////////////////
int Sprite::GetNextFrameIndex()
{
	bool finished;
	return GetNextFrameIndex(finished);
}

//////////////////////////////////////////////////////////////////////////
int Sprite::GetNextFrameIndex(bool& finished)
{
	finished = false;
	int nextFrame = m_CurrentFrame;

	if (m_IsReversed)
	{
		nextFrame--;
		if (nextFrame < 0)
		{
			if (m_IsLooping) nextFrame = m_Frames.size() - 1;
			else
			{
				finished = true;
				nextFrame = m_CurrentFrame;
			}
		}
	}
	else
	{
		nextFrame++;
		if (nextFrame >= (int)m_Frames.size())
		{
			if (m_IsLooping) nextFrame = 0;
			else
			{
				nextFrame = m_CurrentFrame;
				finished = true;
			}
		}
	}
	return nextFrame;
}

//////////////////////////////////////////////////////////////////////////
void Sprite::OnSpriteFinished()
{
	m_IsFinished = true;
}

//////////////////////////////////////////////////////////////////////////
void Sprite::GetBoundingRect(Rect& rect) const
{	
	foreach (SpriteFrame* frame, m_Frames)
	{
		Rect frameRect;
		frame->GetBoundingRect(frameRect);
		rect.UnionWith(frameRect);
	}
}

//////////////////////////////////////////////////////////////////////////
bool Sprite::LoadFromFile(const WideString& fileName)
{
	// is fileName a plain image?
	WideString ext = PathUtil::GetExtension(fileName);
	StringUtil::ToLowerCase(ext);

	const WideStringSet& imageExtList = SystemInfo::GetSupportedImageFormats();
	if (imageExtList.find(ext) != imageExtList.end())
	{
		return LoadFromImage(fileName);
	}
	else
	{
		return DocumentAwareObject::LoadFromFile(fileName);
	}
}

//////////////////////////////////////////////////////////////////////////
bool Sprite::LoadFromImage(const WideString& fileName)
{
	Clear();
	
	SpriteFrame* frame = new SpriteFrame();
	if (!frame->LoadFromImage(fileName))
	{
		SAFE_DELETE(frame);
		return false;
	}

	m_Frames.push_back(frame);
	m_IsLooping = false;

	m_IsLoadedFromImage = true;
	m_FileName = fileName;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Sprite::LoadFromXml(TiXmlElement* rootNode)
{
	Clear();
	m_IsLoadedFromImage = false;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Looping")
		{
			m_IsLooping = XmlUtil::TextToBool(elem);
		}
		else if (elem->ValueStr() == "Frame")
		{
			SpriteFrame* frame = new SpriteFrame();
			if (frame->LoadFromXml(elem))
			{
				m_Frames.push_back(frame);
			}
			else
			{
				SAFE_DELETE(frame);
				return false;
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Sprite::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Looping", rootNode);
	XmlUtil::SetText(elem, m_IsLooping);

	foreach (SpriteFrame* frame, m_Frames)
	{
		frame->SaveToXml(rootNode);
	}

	return true;
}


} // namespace Wme

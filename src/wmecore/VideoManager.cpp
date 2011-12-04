// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "VideoManager.h"
#include "VideoClip.h"
#include "TheoraVideoManager.h"
#include "TheoraAudioBridge.h"

// temp
#include "Game.h"
#include "GameTier.h"
#include "Timer.h"


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment (lib, "libtheoraplayer_static_d.lib")
#		pragma comment (lib, "libtheora_static_d.lib")
#	else
#		pragma comment (lib, "libtheoraplayer_static.lib")
#		pragma comment (lib, "libtheora_static.lib")
#	endif
#endif


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
VideoManager::VideoManager()
{
	m_TheoraMgr = NULL;
	m_AudioBridgeFact = NULL;
}

//////////////////////////////////////////////////////////////////////////
VideoManager::~VideoManager()
{
	foreach (VideoClip* videoClip, m_VideoClips)
	{
		SAFE_DELETE(videoClip);
	}
	m_VideoClips.clear();

	SAFE_DELETE(m_TheoraMgr);
	SAFE_DELETE(m_AudioBridgeFact);
}

//////////////////////////////////////////////////////////////////////////
void VideoManager::Initialize()
{
	m_TheoraMgr = new TheoraVideoManager;

	m_AudioBridgeFact = new TheoraAudioBridgeFactory();
	m_TheoraMgr->setAudioInterfaceFactory(m_AudioBridgeFact);
}

//////////////////////////////////////////////////////////////////////////
void VideoManager::ProcessFrame()
{
	m_TheoraMgr->update((float)Game::GetInstance()->GetLiveTier()->GetTimer()->GetElapsedTime() / 1000);

	foreach (VideoClip* videoClip, m_VideoClips)
	{
		videoClip->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
VideoClip* VideoManager::AddVideoClip(const WideString& fileName, bool looping)
{
	VideoClip* newClip = new VideoClip;
	if (!newClip->Initialize(fileName, looping))
	{
		SAFE_DELETE(newClip);
	}
	else
		m_VideoClips.push_back(newClip);

	return newClip;
}

//////////////////////////////////////////////////////////////////////////
void VideoManager::RemoveVideoClip(VideoClip*& videoClip)
{
	VideoClipList::iterator it;

	it = std::find(m_VideoClips.begin(), m_VideoClips.end(), videoClip);
	if (it != m_VideoClips.end())
	{
		delete (*it);
		m_VideoClips.erase(it);

		videoClip = NULL;
	}
}


} // namespace Wme

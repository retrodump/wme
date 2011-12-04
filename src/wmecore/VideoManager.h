// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeVideoManager_H__
#define __WmeVideoManager_H__


#include "Object.h"


class TheoraVideoManager;

namespace Wme
{
	class TheoraAudioBridgeFactory;
	class VideoClip;

	class WmeDllExport VideoManager : public Object
	{
	public:
		VideoManager();
		~VideoManager();

		void Initialize();
		void ProcessFrame();

		VideoClip* AddVideoClip(const WideString& fileName, bool looping);
		void RemoveVideoClip(VideoClip*& videoClip);


		TheoraVideoManager* GetTheoraMgr() const { return m_TheoraMgr; }

	private:
		TheoraVideoManager* m_TheoraMgr;
		TheoraAudioBridgeFactory* m_AudioBridgeFact;

		typedef std::list<VideoClip*> VideoClipList;
		VideoClipList m_VideoClips;
	};
}

#endif // __WmeVideoManager_H__

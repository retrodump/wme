// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTheoraAudioBridge_H__
#define __WmeTheoraAudioBridge_H__


#include "SoundManager.h"
#include "TheoraAudioInterface.h"
#include "TheoraTimer.h"


class TheoraVideoClip;

namespace Wme
{
	class WmeDllExport TheoraAudioBridge : public TheoraAudioInterface, TheoraTimer
	{
	public:
		TheoraAudioBridge(TheoraVideoClip* owner, int nChannels, int freq);
		~TheoraAudioBridge();

		void insertData(float** data, int nSamples);
		void destroy();

		void update(float time_increase);

		void pause();
		void play();
		void seek(float time);

	private:
		int m_MaxBuffSize;
		int m_BuffSize;
		short* m_TempBuffer;
		float m_TimeOffset;

		struct OpenAL_Buffer
		{
			ALuint id;
			int nSamples;
		} m_Buffers[1000];

		std::queue<OpenAL_Buffer> m_BufferQueue;

		ALuint m_Source;
		int m_NumProcessedSamples;
		int m_NumPlayedSamples;

		static short float2short(float f);
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport TheoraAudioBridgeFactory : public TheoraAudioInterfaceFactory
	{
	public:
		TheoraAudioBridgeFactory();
		~TheoraAudioBridgeFactory();
		TheoraAudioBridge* createInstance(TheoraVideoClip* owner, int nChannels, int freq);
	};

}

#endif // __WmeTheoraAudioBridge_H__

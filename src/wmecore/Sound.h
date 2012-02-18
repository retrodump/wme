// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSound_H__
#define __WmeSound_H__


#include "Object.h"

#ifdef WIN32
#	include <al.h>
#endif

#ifdef __GNUC__ // might need editing to not screw Mac building
#	include <al.h>
#else
#	include <OpenAL/al.h>
#endif


namespace Wme
{
	class SoundDataSource;

	class WmeDllExport Sound : public Object
	{
	public:
		Sound();
		~Sound();

		enum SoundType
		{
			SOUND_EFFECT,
			SOUND_MUSIC,
			SOUND_SPEECH
		};

		enum SoundPriority
		{
			PRIORITY_AUTO = -1,
			PRIORITY_LOW = 0,
			PRIORITY_NORMAL = 1,
			PRIORITY_HIGH = 2
		};

		enum StreamingType
		{
			STREAMING_AUTO = -1,
			STREAMING_NO = 0,
			STREAMING_YES = 1
		};

		WideString GetFileName() const { return m_FileName; }
		bool Initialize(const WideString& fileName, bool looping, SoundType type, StreamingType streamingType, SoundPriority priority);

		void Play();
		void Stop();
		void Pause();

		bool IsPlaying();
		bool IsPaused();
		bool IsStopped();
		bool IsFinished();

		void Update();

		bool GetLooping() const { return m_Looping; }
		void SetLooping(bool val);

		void SetPosition(float pos);
		float GetPosition() const;

		ALuint GiveUpSource();
		bool HasSource() const { return m_Source != AL_NONE; }
		bool SetSource(ALuint newSource = AL_NONE);

		SoundPriority GetPriority() const { return m_Priority; }

		float GetVolume() const { return m_Volume; }
		void SetVolume(float volume);

		float GetMinVolume() const { return m_MinVolume; }
		void SetMinVolume(float volume);

		float GetMaxVolume() const { return m_MaxVolume; }
		void SetMaxVolume(float volume);

		float GetPitch() const { return m_Pitch; }
		void SetPitch(float pitch);

	private:
		void Shutdown();
		void PrepareStaticData();
		void PrepareStreamingData();

		bool StreamBuffer(ALuint buffer);
		void UnqueueBuffers();
		void UpdateBuffers();
		
		bool InitialState();

		WideString m_FileName;
		SoundDataSource* m_DataSource;
		bool m_Streaming;
		bool m_Looping;
		float m_LoopStartPoint;
		SoundType m_Type;
		SoundPriority m_Priority;
		float m_ScheduledStartPos;

		static const size_t NUM_BUFFERS = 4;
		ALuint m_Source;
		ALuint m_Buffers[NUM_BUFFERS];
		float m_BufStartTime[NUM_BUFFERS];

		byte* m_ReadBuffer;
		std::vector<byte> m_DynBuffer;

		bool m_StreamEOF;
		bool m_IsPlaying;
		bool m_ManualStop;

		float m_Volume;
		float m_MinVolume;
		float m_MaxVolume;

		float m_Pitch;

		size_t m_QueueOffset;
	};
}

#endif // __WmeSound_H__

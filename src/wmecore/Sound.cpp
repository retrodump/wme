// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Sound.h"
#include "Game.h"
#include "SoundManager.h"
#include "SoundDataSource.h"
#include "SoundDataSourceFactory.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Sound::Sound()
{
	m_DataSource = NULL;
	m_Streaming = false;
	m_Looping = false;
	m_LoopStartPoint = 0.0f;
	m_ScheduledStartPos = 0.0f;

	m_Type = SOUND_EFFECT;
	m_Priority = PRIORITY_NORMAL;

	m_Source = AL_NONE;
	for (size_t i = 0; i < NUM_BUFFERS; i++)
	{
		m_Buffers[i] = AL_NONE;
		m_BufStartTime[i] = 0.0f;
	}

	m_ReadBuffer = NULL;
	m_StreamEOF = true;
	m_IsPlaying = false;
	m_ManualStop = true;
	m_QueueOffset = 0;

	m_Volume = 1.0f;
	m_MinVolume = 0.0f;
	m_MaxVolume = 1.0f;

	m_Pitch = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
Sound::~Sound()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////
void Sound::Shutdown()
{
	SoundManager* soundMgr = Game::GetInstance()->GetSoundMgr();

	if (m_Source)
	{
		alSourceStop(m_Source);
		soundMgr->ReclaimSoundSource(m_Source);
		m_Source = AL_NONE;
	}

	alDeleteBuffers(NUM_BUFFERS, m_Buffers);
	for (size_t i = 0; i < NUM_BUFFERS; i++)
	{
		m_Buffers[i] = AL_NONE;
		m_BufStartTime[i] = 0.0f;
	}

	SAFE_DELETE(m_DataSource);

	SAFE_DELETE_ARRAY(m_ReadBuffer);
	m_DynBuffer.clear();

	m_StreamEOF = true;
	m_IsPlaying = false;
	m_ManualStop = true;
	m_QueueOffset = 0;
	m_ScheduledStartPos = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
bool Sound::Initialize(const WideString& fileName, bool looping, SoundType type, StreamingType streamingType, SoundPriority priority)
{
	SoundManager* soundMgr = Game::GetInstance()->GetSoundMgr();


	Shutdown();

	// determine priority
	if (priority == PRIORITY_AUTO)
	{
		switch (type)
		{
		case SOUND_SPEECH:
		case SOUND_MUSIC:
			m_Priority = PRIORITY_HIGH;
			break;
		default:
			m_Priority = PRIORITY_NORMAL;
		}
	}
	else m_Priority = priority;

	// obtain data source
	m_DataSource = SoundDataSourceFactory::GetInstance(fileName);
	if (!m_DataSource) return false;
	

	m_ReadBuffer = new byte[m_DataSource->GetBufferSize()];
	m_DynBuffer.reserve(m_DataSource->GetBufferSize());

	// are we going to stream this sound?
	switch (streamingType)
	{
	case STREAMING_YES:
		m_Streaming = true;
		break;
	case STREAMING_NO:
		m_Streaming = false;
		break;
	default:
		if (m_DataSource->GetTotalTime() > soundMgr->GetMaxNonStreamingLength()) m_Streaming = true;
		else m_Streaming = false;
	}


	// other properties
	m_Type = type;
	m_LoopStartPoint = 0.0f;
	m_Looping = looping;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Sound::SetSource(ALuint newSource)
{
	SoundManager* soundMgr = Game::GetInstance()->GetSoundMgr();

	if (m_Source != AL_NONE) return true;

	m_Source = newSource;

	// grab source if necessary
	if (m_Source == AL_NONE) m_Source = soundMgr->GetSoundSource(this);
	if (m_Source == AL_NONE) return false;

	// prepare buffer(s)
	alGetError();
	alGenBuffers(m_Streaming ? NUM_BUFFERS : 1, m_Buffers);
	if (alGetError() != AL_NO_ERROR)
	{
		return false;
	}

	if (!m_Streaming)
	{
		PrepareStaticData();
		alSourcei(m_Source, AL_BUFFER, m_Buffers[0]);
	}
	else
	{
		m_StreamEOF = false;
	}

	SetLooping(m_Looping);
	SetPosition(m_ScheduledStartPos);
	SetMinVolume(m_MinVolume);
	SetMaxVolume(m_MaxVolume);
	SetVolume(m_Volume);
	SetPitch(m_Pitch);

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Sound::Update()
{
	if (!m_Source) return;

	if (m_Streaming) UpdateBuffers();
}

//////////////////////////////////////////////////////////////////////////
void Sound::Play()
{
	if (IsPlaying()) return;

	if (!m_Source)
	{
		if (!SetSource()) return;
	}

	m_ManualStop = false;
	alSourcePlay(m_Source);
	m_IsPlaying = true;
}

//////////////////////////////////////////////////////////////////////////
void Sound::Stop()
{
	if (!m_Source || IsStopped()) return;

	m_IsPlaying = false;
	m_ManualStop = true;

	if (!m_Streaming)
	{
		alSourceStop(m_Source);
		alSourceRewind(m_Source);
	}
	else
	{
		UnqueueBuffers();
		m_DataSource->SkipToTime(0);
		PrepareStreamingData();
	}
}

//////////////////////////////////////////////////////////////////////////
void Sound::Pause()
{
	if (!m_Source) return;

	m_IsPlaying = false;	
	alSourcePause(m_Source);
}

//////////////////////////////////////////////////////////////////////////
bool Sound::IsPlaying()
{
	if(m_Source)
	{
		ALenum state;
		alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
		return (state == AL_PLAYING);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Sound::IsPaused()
{
	if(m_Source)
	{
		ALenum state;
		alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
		return (state == AL_PAUSED);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Sound::IsStopped()
{
	if(m_Source != AL_NONE)
	{
		ALenum state;
		alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
		return (!m_IsPlaying && (state == AL_STOPPED));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Sound::IsFinished()
{
	if(m_Source != AL_NONE)
	{
		ALenum state;
		alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
		if (!m_ManualStop && (state == AL_STOPPED))
		{
			if (m_Streaming) return m_StreamEOF;
			else return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void Sound::PrepareStaticData()
{
	if (m_DynBuffer.empty())
	{
		m_DataSource->SkipToTime(0);
		while (!m_DataSource->IsEof())
		{
			size_t bytesRead = m_DataSource->ReadData(m_DataSource->GetBufferSize(), m_ReadBuffer);
			if (bytesRead > 0) m_DynBuffer.insert(m_DynBuffer.end(), m_ReadBuffer, m_ReadBuffer + bytesRead);
		}
	}

	if (!m_DynBuffer.empty())
	{
		alGetError();
		alBufferData(m_Buffers[0], m_DataSource->GetFormat(), &m_DynBuffer[0], m_DynBuffer.size(), m_DataSource->GetFrequency());
	}
}

//////////////////////////////////////////////////////////////////////////
void Sound::PrepareStreamingData()
{
	for (size_t i = 0; i < NUM_BUFFERS; i++)
	{
		if (StreamBuffer(m_Buffers[i]))
			alSourceQueueBuffers(m_Source, 1, &m_Buffers[i]);
		else
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Sound::StreamBuffer(ALuint buffer)
{
	// remember buffer start pos
	for (int i = 0; i < NUM_BUFFERS; i++)
	{
		if (m_Buffers[i] == buffer)
		{
			m_BufStartTime[i] = m_DataSource->GetCurrentTime();
			break;
		}
	}


	m_DynBuffer.clear();

	while(m_DynBuffer.size() < m_DataSource->GetBufferSize())
	{
		size_t bytesRead = m_DataSource->ReadData(m_DataSource->GetBufferSize(), m_ReadBuffer);
		
		if (m_DataSource->IsEof())
		{
			if (m_Looping)
			{
				m_DataSource->SkipToTime(m_LoopStartPoint);
			}
			else
			{
				m_StreamEOF = true;
				break;
			}
		}
		if (bytesRead > 0) m_DynBuffer.insert(m_DynBuffer.end(), m_ReadBuffer, m_ReadBuffer + bytesRead);
	}

	if (m_DynBuffer.empty()) return false;

	alGetError();
	alBufferData(buffer, m_DataSource->GetFormat(), &m_DynBuffer[0], m_DynBuffer.size(), m_DataSource->GetFrequency());
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Sound::UpdateBuffers()
{
	ALenum state;
	alGetSourcei(m_Source, AL_SOURCE_STATE, &state);

	if (state == AL_PAUSED) return;

	// stopped state means we're out of data
	// so either the stream is at EOF or we stalled
	if (state == AL_STOPPED)
	{
		if (m_ManualStop) return;
		
		if (m_StreamEOF)
		{
			Stop();
			m_ManualStop = false;
			return;
		}
		else
		{
			UnqueueBuffers();
			PrepareStreamingData();

			alSourcePlay(m_Source);
		}
	}

	int processed;
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &processed);

	while (processed)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(m_Source, 1, &buffer);
		m_QueueOffset++;
		if (m_QueueOffset >= NUM_BUFFERS) m_QueueOffset = 0;

		if (StreamBuffer(buffer)) alSourceQueueBuffers(m_Source, 1, &buffer);

		processed--;
	}
}

//////////////////////////////////////////////////////////////////////////
void Sound::UnqueueBuffers()
{
	if(!m_Source) return;

	alSourceStop(m_Source);

	int processed = 0;
	alGetError();
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &processed);

	while (processed)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(m_Source, 1, &buffer);

		processed--;
	}
	m_QueueOffset = 0;
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetLooping(bool val)
{
	m_Looping = val;

	if (!m_Streaming && m_Source)
	{
		alSourcei(m_Source, AL_LOOPING, m_Looping);
	}
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetPosition(float pos)
{
	if (!m_Source)
	{
		m_ScheduledStartPos = pos;
		return;
	}

	if (!m_Streaming)
	{
		alSourcef(m_Source, AL_SEC_OFFSET, pos);
	}
	else
	{
		bool playing = IsPlaying();
		if (pos > m_DataSource->GetTotalTime()) pos = m_DataSource->GetTotalTime() - 0.1f;

		UnqueueBuffers();
		m_DataSource->SkipToTime(pos);
		PrepareStreamingData();
		if (playing) Play();

		// !!! paused sound will be switched to stopped state because of UnqueueBuffers
	}
}

//////////////////////////////////////////////////////////////////////////
bool Sound::InitialState()
{
	ALenum state;
	alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
	return (state == AL_INITIAL);
}

//////////////////////////////////////////////////////////////////////////
float Sound::GetPosition() const
{
	float pos = 0.0f;
	if (!m_Source) return pos;

	
	if (!m_Streaming)
	{
		alGetSourcef(m_Source, AL_SEC_OFFSET, &pos);
	}
	else
	{		
		// buffer offset
		alGetSourcef(m_Source, AL_SEC_OFFSET, &pos);

		// stream time offset of the buffer we're playing
		pos += m_BufStartTime[m_QueueOffset];

		// did this buffer wrap over?
		if (pos > m_DataSource->GetTotalTime())
		{
			pos -= m_DataSource->GetTotalTime();
			pos += m_LoopStartPoint;
		}
	}

	return pos;
}

//////////////////////////////////////////////////////////////////////////
ALuint Sound::GiveUpSource()
{
	if (!m_Source) return AL_NONE;

	ALuint ret = m_Source;
	m_ScheduledStartPos = GetPosition();

	if (m_Streaming) UnqueueBuffers();
	else alSourceStop(m_Source);

	alDeleteBuffers(NUM_BUFFERS, m_Buffers);
	for (size_t i = 0; i < NUM_BUFFERS; i++)
	{
		m_Buffers[i] = AL_NONE;
	}

	m_Source = AL_NONE;
	
	return ret;
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetVolume(float volume)
{
	m_Volume = Ogre::Math::Clamp(volume, 0.0f, 1.0f);

	if(m_Source) alSourcef(m_Source, AL_GAIN, m_Volume);	
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetMinVolume(float volume)
{
	m_MinVolume = Ogre::Math::Clamp(volume, 0.0f, 1.0f);

	if(m_Source) alSourcef(m_Source, AL_MIN_GAIN, m_MinVolume);
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetMaxVolume(float volume)
{
	m_MaxVolume = Ogre::Math::Clamp(volume, 0.0f, 1.0f);

	if(m_Source) alSourcef(m_Source, AL_MAX_GAIN, m_MaxVolume);	
}

//////////////////////////////////////////////////////////////////////////
void Sound::SetPitch(float pitch)
{
	if (pitch <= 0.0f) return;

	m_Pitch = pitch;
	if(m_Source) alSourcef(m_Source, AL_PITCH, m_Pitch);
}


} // namespace Wme

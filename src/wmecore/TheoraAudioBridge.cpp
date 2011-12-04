// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TheoraAudioBridge.h"
#include "TheoraVideoClip.h"
#include "Game.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TheoraAudioBridge::TheoraAudioBridge(TheoraVideoClip* owner, int nChannels, int freq) : TheoraAudioInterface(owner, nChannels, freq), TheoraTimer()
{
	m_MaxBuffSize = freq * mNumChannels * 2;
	m_BuffSize = 0;
	m_NumProcessedSamples = 0;
	m_TimeOffset = 0;

	m_TempBuffer = new short[m_MaxBuffSize];
	
	m_Source = AL_NONE;
	alGenSources(1, &m_Source);

	owner->setTimer(this);
	m_NumPlayedSamples = 0;
}

//////////////////////////////////////////////////////////////////////////
TheoraAudioBridge::~TheoraAudioBridge()
{
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::destroy()
{
	SoundManager* soundMgr = Game::GetInstance()->GetSoundMgr();

	if (m_Source)
	{
		alSourceStop(m_Source);
		soundMgr->ReclaimSoundSource(m_Source);
		m_Source = AL_NONE;
	}

	while (!m_BufferQueue.empty())
	{
		OpenAL_Buffer buff = m_BufferQueue.front();
		m_BufferQueue.pop();
		alDeleteBuffers(1, &buff.id);
	}

	if (m_TempBuffer)
	{
		SAFE_DELETE_ARRAY(m_TempBuffer);
	}
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::insertData(float** data, int nSamples)
{
	for (int i = 0; i < nSamples; i++)
	{
		if (m_BuffSize < m_MaxBuffSize)
		{
			m_TempBuffer[m_BuffSize++] = float2short(data[0][i]);
			if (mNumChannels == 2)
				m_TempBuffer[m_BuffSize++] = float2short(data[1][i]);
		}
		if (m_BuffSize == mFreq * mNumChannels / 4)
		{
			OpenAL_Buffer buff;
			alGenBuffers(1, &buff.id);

			ALuint format = (mNumChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			alBufferData(buff.id, format, m_TempBuffer, m_BuffSize * 2, mFreq);
			alSourceQueueBuffers(m_Source, 1, &buff.id);
			buff.nSamples = m_BuffSize / mNumChannels;
			m_NumProcessedSamples += m_BuffSize / mNumChannels;
			m_BufferQueue.push(buff);

			m_BuffSize = 0;

			int state;
			alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
			if (state != AL_PLAYING)
			{
				alSourcef(m_Source, AL_SAMPLE_OFFSET, (float)m_NumProcessedSamples - mFreq / 4);
				alSourcePlay(m_Source);
			}

		}
	}
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::update(float time_increase)
{
	int i, state, nProcessed;
	OpenAL_Buffer buff;

	// process played buffers
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &nProcessed);
	for (i = 0; i < nProcessed; i++)
	{
		buff = m_BufferQueue.front();
		m_BufferQueue.pop();
		m_NumPlayedSamples += buff.nSamples;
		alSourceUnqueueBuffers(m_Source, 1, &buff.id);
		alDeleteBuffers(1, &buff.id);
	}

	// control playback and return time position
	alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING)
	{
		alGetSourcef(m_Source, AL_SEC_OFFSET, &mTime);
		mTime += (float)m_NumPlayedSamples / mFreq;
		m_TimeOffset=0;
	}
	else
	{
		mTime = (float)m_NumProcessedSamples / mFreq + m_TimeOffset;
		m_TimeOffset += time_increase;
	}

	float duration = mClip->getDuration();
	if (mTime > duration) mTime = duration;
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::pause()
{
	alSourcePause(m_Source);
	TheoraTimer::pause();
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::play()
{
	alSourcePlay(m_Source);
	TheoraTimer::play();
}

//////////////////////////////////////////////////////////////////////////
void TheoraAudioBridge::seek(float time)
{
	OpenAL_Buffer buff;

	alSourceStop(m_Source);
	
	while (!m_BufferQueue.empty())
	{
		buff = m_BufferQueue.front();
		m_BufferQueue.pop();
		alSourceUnqueueBuffers(m_Source, 1, &buff.id);
		alDeleteBuffers(1, &buff.id);
	}

	m_BuffSize = 0;
	m_TimeOffset = 0;

	m_NumPlayedSamples = m_NumProcessedSamples = (int)time*mFreq;
}

//////////////////////////////////////////////////////////////////////////
short TheoraAudioBridge::float2short(float f)
{
	if      (f >  1) f =  1;
	else if (f < -1) f = -1;
	return (short) (f * 32767);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TheoraAudioBridgeFactory::TheoraAudioBridgeFactory()
{

}

//////////////////////////////////////////////////////////////////////////
TheoraAudioBridgeFactory::~TheoraAudioBridgeFactory()
{

}

//////////////////////////////////////////////////////////////////////////
TheoraAudioBridge* TheoraAudioBridgeFactory::createInstance(TheoraVideoClip* owner, int nChannels, int freq)
{
	return new TheoraAudioBridge(owner, nChannels, freq);
}


} // namespace Wme

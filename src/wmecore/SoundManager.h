// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSoundManager_H__
#define __WmeSoundManager_H__


#include "Object.h"
#include "StringUtil.h"
#include "Sound.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#	include <al.h>
#	include <alc.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif


namespace Wme
{
	class SoundDevice;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport SoundManager : public Object
	{
	public:
		SoundManager();
		~SoundManager();

		typedef std::list<SoundDevice*> SoundDeviceList;

		void Initialize(const AnsiString& preferredDevice = "");
		void ProcessFrame();

		Sound* AddSound(const WideString& fileName, bool looping,	Sound::SoundType type = Sound::SOUND_EFFECT,
																	Sound::StreamingType streamingType = Sound::STREAMING_AUTO,
																	Sound::SoundPriority priority = Sound::PRIORITY_AUTO);
		void RemoveSound(Sound*& sound);

		ALuint GetSoundSource(Sound* requester);
		void ReclaimSoundSource(ALuint source);

		void ScanDevices();
		SoundDeviceList& GetDevices();
		SoundDevice* GetDefaultDevice();
		SoundDevice* GetDeviceByName(const AnsiString& name);

		float GetMaxNonStreamingLength() const { return m_MaxNonStreamingLength; }
		void SetMaxNonStreamingLength(float val) { m_MaxNonStreamingLength = val; }

		float GetMasterVolume() const;
		void SetMasterVolume(float volume);

	private:
		void PostponeSound(Sound* sound);
		size_t GetMaxNumSources() const;
		void ClearDevices();
		void Shutdown();
		SoundDeviceList m_Devices;
		SoundDevice* m_SelectedDevice;

		float m_MaxNonStreamingLength;
		
		ALCdevice* m_Device;
		ALCcontext* m_Context;

		typedef std::list<Sound*> SoundList;
		SoundList m_Sounds;
		SoundList m_PostponedSounds;

		typedef std::list<ALuint> SourceList;
		SourceList m_SourcePool;
	};
}

#endif // __WmeSoundManager_H__

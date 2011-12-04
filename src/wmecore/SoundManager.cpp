// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SoundManager.h"
#include "SoundDevice.h"
#include "Game.h"
#include "StringUtil.h"


#if defined(_MSC_VER)
#	pragma comment(lib, "OpenAL32.lib")
#endif



namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager()
{
	m_SelectedDevice = NULL;
	m_Device = NULL;
	m_Context = NULL;

	m_MaxNonStreamingLength = 10;
}

//////////////////////////////////////////////////////////////////////////
SoundManager::~SoundManager()
{
	Shutdown();
	ClearDevices();
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::Initialize(const AnsiString& preferredDevice)
{
	m_SelectedDevice = NULL;
	m_Device = NULL;
	m_Context = NULL;

	if (!preferredDevice.empty()) m_SelectedDevice = GetDeviceByName(preferredDevice);
	if (!m_SelectedDevice) m_SelectedDevice = GetDefaultDevice();

	const char* deviceName = NULL;	
	if (m_SelectedDevice) deviceName = m_SelectedDevice->GetName().c_str();


	m_Device = alcOpenDevice(deviceName);
	if (m_Device)
	{
		m_Context = alcCreateContext(m_Device, NULL);
		if (m_Context)
		{
			alcMakeContextCurrent(m_Context);
		}
		else
		{
			alcCloseDevice(m_Device);
			m_SelectedDevice = NULL;
			m_Device = NULL;
		}
	}

	if (m_Device)
	{
		if (m_SelectedDevice) Game::GetInstance()->Log(L"Sound initialized for device '" + StringUtil::AnsiToWide(m_SelectedDevice->GetName()) + L"'.");
		else Game::GetInstance()->Log(L"Sound initialized for default device.");
	}
	else Game::GetInstance()->Log(L"Sound system not initialized.");
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::Shutdown()
{
	foreach (Sound* sound, m_Sounds)
	{
		SAFE_DELETE(sound);
	}
	m_Sounds.clear();

	m_PostponedSounds.clear();

	foreach (ALuint source, m_SourcePool)
	{
		alDeleteSources(1, &source);
	}
	m_SourcePool.clear();


	ALCcontext* context = alcGetCurrentContext();
	if (context)
	{
		ALCdevice* device = alcGetContextsDevice(context);

		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		if (device) alcCloseDevice(device);
	}
	m_SelectedDevice = NULL;
	m_Device = NULL;
	m_Context = NULL;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::ProcessFrame()
{
	// reactivate postponed sounds if we have free sources
	while (!m_PostponedSounds.empty())
	{
		if (!m_SourcePool.empty())
		{
			ALuint newSource = m_SourcePool.front();
			m_SourcePool.pop_front();

			Sound* sound = m_PostponedSounds.front();
			m_PostponedSounds.pop_front();

			sound->SetSource(newSource);
			sound->Play();
		}
		else break;
	}

	// process sounds
	foreach (Sound* sound, m_Sounds)
	{
		sound->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
Sound* SoundManager::AddSound(const WideString& fileName, bool looping, Sound::SoundType type, Sound::StreamingType streamingType, Sound::SoundPriority priority)
{
	Sound* newSound = new Sound;
	if (!newSound->Initialize(fileName, looping, type, streamingType, priority))
	{
		SAFE_DELETE(newSound);
	}
	else m_Sounds.push_back(newSound);

	return newSound;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::RemoveSound(Sound*& sound)
{
	SoundList::iterator it;
	
	it = std::find(m_PostponedSounds.begin(), m_PostponedSounds.end(), sound);
	if (it != m_PostponedSounds.end()) m_PostponedSounds.erase(it);

	it = std::find(m_Sounds.begin(), m_Sounds.end(), sound);
	if (it != m_Sounds.end())
	{
		delete (*it);
		m_Sounds.erase(it);

		sound = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
ALuint SoundManager::GetSoundSource(Sound* requester)
{
	ALuint newSource = AL_NONE;

	if (!m_SourcePool.empty())
	{
		newSource = m_SourcePool.front();
		m_SourcePool.pop_front();
	}
	else
	{
		alGenSources(1, &newSource);
	}

	if (newSource == AL_NONE)
	{
		Sound* bestStopped = NULL;
		Sound* bestPlaying = NULL;

		foreach (Sound* sound, m_Sounds)
		{
			if (!sound->HasSource()) continue;
			if (sound->GetPriority() > requester->GetPriority()) continue;

			if (sound->IsPlaying())
			{
				if (!bestPlaying || bestPlaying->GetPriority() > sound->GetPriority()) bestPlaying = sound;
			}
			else if (sound->IsStopped())
			{
				if (!bestStopped || bestStopped->GetPriority() > sound->GetPriority()) bestStopped = sound;
			}
		}
		if (bestStopped) newSource = bestStopped->GiveUpSource();
		else if (bestPlaying)
		{
			newSource = bestPlaying->GiveUpSource();
			PostponeSound(bestPlaying);
			m_PostponedSounds.push_back(bestPlaying);
		}
	}

	if (newSource == AL_NONE) PostponeSound(requester);

	return newSource;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::ReclaimSoundSource(ALuint source)
{	
	m_SourcePool.push_back(source);
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::PostponeSound(Sound* sound)
{
	if (std::find(m_PostponedSounds.begin(), m_PostponedSounds.end(), sound) == m_PostponedSounds.end())
		m_PostponedSounds.push_back(sound);
}

//////////////////////////////////////////////////////////////////////////
float SoundManager::GetMasterVolume() const
{
	float vol = 0.0f;
	alGetListenerf(AL_GAIN, &vol);
	return vol;
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::SetMasterVolume(float volume)
{
	volume = Ogre::Math::Clamp(volume, 0.0f, 1.0f);
	alListenerf(AL_GAIN, volume);
}


//////////////////////////////////////////////////////////////////////////
SoundDevice* SoundManager::GetDefaultDevice()
{
	if (m_Devices.empty()) ScanDevices();

	foreach (SoundDevice* device, m_Devices)
	{
		if (device->IsDefault()) return device;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
SoundDevice* SoundManager::GetDeviceByName(const AnsiString& name)
{
	if (m_Devices.empty()) ScanDevices();
	
	foreach (SoundDevice* device, m_Devices)
	{
		if (device->GetName() == name) return device;
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
void SoundManager::ScanDevices()
{
	ClearDevices();

	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
	{
		char* devices = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		char* defaultDeviceName = (char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		while (*devices != NULL)
		{
			bool isDefault = (strcmp(defaultDeviceName, devices) == 0);


			ALCdevice* device = alcOpenDevice(devices);
			if (device)
			{
				ALCcontext* context = alcCreateContext(device, NULL);
				if (context)
				{
					alcMakeContextCurrent(context);
					// if new actual device name isn't already in the list, then add it...
					const char* actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
					bool newName = true;
					foreach (SoundDevice* device, m_Devices)
					{
						if (device->GetName() == actualDeviceName)
						{
							newName = false;
							break;
						}
					}

					if ((newName) && (actualDeviceName != NULL) && (strlen(actualDeviceName) > 0))
					{
						SoundDevice* newDevice = new SoundDevice(actualDeviceName);
						m_Devices.push_back(newDevice);


						newDevice->m_MaxSources = GetMaxNumSources();
						newDevice->m_IsDefault = isDefault;
						alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(int), &newDevice->m_VerMajor);
						alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(int), &newDevice->m_VerMinor);

						// Check for ALC Extensions
						if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_TRUE)
							newDevice->m_Extensions.push_back("ALC_EXT_CAPTURE");
						if (alcIsExtensionPresent(device, "ALC_EXT_EFX") == AL_TRUE)
							newDevice->m_Extensions.push_back("ALC_EXT_EFX");

						// Check for AL Extensions
						if (alIsExtensionPresent("AL_EXT_OFFSET") == AL_TRUE)
							newDevice->m_Extensions.push_back("AL_EXT_OFFSET");

						if (alIsExtensionPresent("AL_EXT_LINEAR_DISTANCE") == AL_TRUE)
							newDevice->m_Extensions.push_back("AL_EXT_LINEAR_DISTANCE");
						if (alIsExtensionPresent("AL_EXT_EXPONENT_DISTANCE") == AL_TRUE)
							newDevice->m_Extensions.push_back("AL_EXT_EXPONENT_DISTANCE");

						if (alIsExtensionPresent("EAX2.0") == AL_TRUE)
							newDevice->m_Extensions.push_back("EAX2.0");
						if (alIsExtensionPresent("EAX3.0") == AL_TRUE)
							newDevice->m_Extensions.push_back("EAX3.0");
						if (alIsExtensionPresent("EAX4.0") == AL_TRUE)
							newDevice->m_Extensions.push_back("EAX4.0");
						if (alIsExtensionPresent("EAX5.0") == AL_TRUE)
							newDevice->m_Extensions.push_back("EAX5.0");

						if (alIsExtensionPresent("EAX-RAM") == AL_TRUE)
							newDevice->m_Extensions.push_back("EAX-RAM");
					}
					alcMakeContextCurrent(NULL);
					alcDestroyContext(context);
				}
				alcCloseDevice(device);
			}

			devices += strlen(devices) + 1;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SoundManager::ClearDevices()
{
	foreach (SoundDevice* device, m_Devices)
	{
		SAFE_DELETE(device);
	}
	m_Devices.clear();

	m_SelectedDevice = NULL;
}

//////////////////////////////////////////////////////////////////////////
SoundManager::SoundDeviceList& SoundManager::GetDevices()
{
	if (m_Devices.empty()) ScanDevices();
	return m_Devices;
}


//////////////////////////////////////////////////////////////////////////
size_t SoundManager::GetMaxNumSources() const
{
	ALuint uiSources[256];
	size_t sourceCount = 0;

	// Clear AL Error Code
	alGetError();

	// Generate up to 256 Sources, checking for any errors
	for (sourceCount = 0; sourceCount < 256; sourceCount++)
	{
		alGenSources(1, &uiSources[sourceCount]);
		if (alGetError() != AL_NO_ERROR)
			break;
	}

	// Release the Sources
	alDeleteSources(sourceCount, uiSources);
	if (alGetError() != AL_NO_ERROR)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			alDeleteSources(1, &uiSources[i]);
		}
	}

	return sourceCount;
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "VorbisDataSource.h"
#include "Game.h"
#include "FileManager.h"
#include "File.h"
#include "StringUtil.h"


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment(lib, "libogg_static_d.lib")
#		pragma comment(lib, "libvorbis_static_d.lib")
#		pragma comment(lib, "libvorbisfile_d.lib")
#	else
#		pragma comment(lib, "libogg_static.lib")
#		pragma comment(lib, "libvorbis_static.lib")
#		pragma comment(lib, "libvorbisfile.lib")
#	endif
#endif


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
VorbisDataSource::VorbisDataSource()
{
	m_File = NULL;
	m_VorbisInfo = NULL;
	m_BufferSize = 0;
	m_NumChannels = 0;
	m_IsEof = false;

	memset(&m_VorbisStream, 0, sizeof(m_VorbisStream));
}

//////////////////////////////////////////////////////////////////////////
VorbisDataSource::~VorbisDataSource()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
void VorbisDataSource::Cleanup()
{
	ov_clear(&m_VorbisStream);
	m_VorbisInfo = NULL;
	m_BufferSize = 0;
	m_NumChannels = 0;
	m_IsEof = false;

	if (m_File) Game::GetInstance()->GetFileMgr()->CloseFile(m_File);

	m_Initialized = false;
}

//////////////////////////////////////////////////////////////////////////
bool VorbisDataSource::InitializeFromFile(const WideString& fileName)
{
	Cleanup();

	m_File = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!m_File) return false;


	ov_callbacks callbacks;
	callbacks.read_func  = VorbisDataSource::FileRead;
	callbacks.seek_func  = VorbisDataSource::FileSeek;
	callbacks.close_func = VorbisDataSource::FileClose;
	callbacks.tell_func  = VorbisDataSource::FileTell;



	// open ogg vorbis stream
	int result = ov_open_callbacks(m_File, &m_VorbisStream, NULL, 0, callbacks);
	if (result != 0)
	{
		Game::GetInstance()->Log(fileName + L": ov_open_callbacks failed, result=", StringUtil::ToString(result));
		return false;
	}

	// calculate stream type
	m_VorbisInfo = ov_info(&m_VorbisStream, -1);
	if(!m_VorbisInfo)
	{
		Game::GetInstance()->Log(fileName + L": ov_info failed");
		return false;
	}
	ReadSoundInfo();

	if (m_BufferSize == 0)
	{
		Game::GetInstance()->Log(fileName + L": Unrecognized vorbis format.");
		return false;
	}


	m_Initialized = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void VorbisDataSource::ReadSoundInfo()
{
	m_NumChannels = m_VorbisInfo->channels;
	
	switch (m_VorbisInfo->channels)
	{
	case 1:
		m_Format = AL_FORMAT_MONO16;
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		m_BufferSize -= (m_BufferSize % 2);
		break;

	case 2:
		m_Format = AL_FORMAT_STEREO16;
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate;
		m_BufferSize -= (m_BufferSize % 4);
		break;

	case 4:
		m_Format = alGetEnumValue("AL_FORMAT_QUAD16");
		if (!m_Format) return;

		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate * 2;
		m_BufferSize -= (m_BufferSize % 8);
		break;

	case 6:
		m_Format = alGetEnumValue("AL_FORMAT_51CHN16");
		if (!m_Format) return;

		// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate * 3;
		m_BufferSize -= (m_BufferSize % 12);
		break;

	case 7:
		m_Format = alGetEnumValue("AL_FORMAT_61CHN16");
		if (!m_Format) return;
				
		// Set BufferSize to 250ms (Frequency * 16 (16bit 7-channel) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate * 4;
		m_BufferSize -= (m_BufferSize % 16);
		break;

	case 8:
		m_Format = alGetEnumValue("AL_FORMAT_71CHN16");
		if (!m_Format) return;
				
		// Set BufferSize to 250ms (Frequency * 20 (16bit 8-channel) divided by 4 (quarter of a second))
		m_BufferSize = m_VorbisInfo->rate * 5;
		m_BufferSize -= (m_BufferSize % 20);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
size_t VorbisDataSource::ReadData(size_t size, byte* buffer)
{
	if (!m_Initialized) return 0;

	size_t dataRead = 0;
	bool eof = false;
	int bitstream;

	while (dataRead < size && !eof)
	{
		int result = ov_read(&m_VorbisStream, (char*)buffer + dataRead, size - dataRead, 0, 2, 1, &bitstream);
		if(result > 0) dataRead += result;
		else eof = true; // we treat errors as eof
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// more channel files need to be re-ordered
	// http://www.xiph.org/vorbis/doc/Vorbis_I_spec.html#x1-800004.3.9
	if (m_NumChannels == 6)
	{		
		short* samples = (short*)buffer;
		for (unsigned long sample = 0; sample < (m_BufferSize >> 1); sample += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR, RL,  RR, LFE
			std::swap<short>(samples[sample + 1], samples[sample + 2]);
			std::swap<short>(samples[sample + 3], samples[sample + 5]);
			std::swap<short>(samples[sample + 4], samples[sample + 5]);
		}
	}
	else if (m_NumChannels == 7)
	{		
		short* samples = (short*)buffer;
		for (unsigned long sample = 0; sample < (m_BufferSize >> 1); sample += 7)
		{
			// WAVEFORMATEXTENSIBLE Order : FL,  FR,  FC,  LFE, RL,  RR,  RC
			// OggVorbis Order            : FL,  FC,  FR,  RL,  RR,  RC,  LFE 
			std::swap<short>(samples[sample + 1], samples[sample + 2]);
			std::swap<short>(samples[sample + 3], samples[sample + 6]);
			std::swap<short>(samples[sample + 4], samples[sample + 6]);
			std::swap<short>(samples[sample + 5], samples[sample + 6]);
		}
	}
	else if (m_NumChannels == 8)
	{		
		short* samples = (short*)buffer;
		for (unsigned long sample = 0; sample < (m_BufferSize >> 1); sample += 8)
		{
			// WAVEFORMATEXTENSIBLE Order : FL,  FR,  FC,  LFE, RL,  RR,  LS,  RS
			// OggVorbis Order            : FL,  FC,  FR,  LS,  RS,  RL,  RR,  LFE 
			std::swap<short>(samples[sample + 1], samples[sample + 2]);
			std::swap<short>(samples[sample + 3], samples[sample + 7]);
			std::swap<short>(samples[sample + 4], samples[sample + 5]);
			std::swap<short>(samples[sample + 5], samples[sample + 6]);
			std::swap<short>(samples[sample + 6], samples[sample + 7]);
		}
	}

	m_IsEof = eof;

	return dataRead;
}

//////////////////////////////////////////////////////////////////////////
bool VorbisDataSource::IsEof() const
{
	if (!m_Initialized) return true;
	else return m_IsEof;
}

//////////////////////////////////////////////////////////////////////////
void VorbisDataSource::SkipToTime(float seconds)
{
	if (!m_Initialized) return;

	if (ov_time_seek(&m_VorbisStream, seconds) == 0) m_IsEof = false;
	else  m_IsEof = true;
}

//////////////////////////////////////////////////////////////////////////
float VorbisDataSource::GetTotalTime() const
{
	if (m_Initialized) return static_cast<float>(ov_time_total(&m_VorbisStream, -1));
	else return 0.0f;
}

//////////////////////////////////////////////////////////////////////////
float VorbisDataSource::GetCurrentTime() const
{
	if (m_Initialized) return static_cast<float>(ov_time_tell(&m_VorbisStream));
	else return 0.0f;
}

//////////////////////////////////////////////////////////////////////////
int VorbisDataSource::GetFrequency() const
{
	if (m_Initialized) return m_VorbisInfo->rate;
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
// Vorbis filesystem bridge
//////////////////////////////////////////////////////////////////////////
size_t VorbisDataSource::FileRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
	File* file = reinterpret_cast<File*>(datasource);
	return file->Read(ptr, size);
}

//////////////////////////////////////////////////////////////////////////
int VorbisDataSource::FileSeek(void* datasource, ogg_int64_t offset, int whence)
{
	File* file = reinterpret_cast<File*>(datasource);

	bool success = false;

	switch(whence)
	{
	case SEEK_SET:
		success = file->Seek((size_t)offset, File::SEEK_FROM_BEGIN);
		break;
	case SEEK_END:
		success = file->Seek((size_t)offset, File::SEEK_FROM_END);
		break;
	case SEEK_CUR:
		success = file->Seek((size_t)offset, File::SEEK_FROM_CURRENT);
		break;
	}
	
	return success ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////
int VorbisDataSource::FileClose(void* datasource)
{
	// we close the file ourselves
	return 0;
}

//////////////////////////////////////////////////////////////////////////
long VorbisDataSource::FileTell(void* datasource)
{
	File* file = reinterpret_cast<File*>(datasource);
	return file->GetPos();
}


} // namespace Wme

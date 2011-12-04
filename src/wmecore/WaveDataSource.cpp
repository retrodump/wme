// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "WaveDataSource.h"
#include "Game.h"
#include "FileManager.h"
#include "File.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
WaveDataSource::WaveDataSource()
{
	m_File = NULL;
	m_WaveDataOffset = 0;
	m_TotalDataLength = 0;
	m_BufferSize = 0;
	m_Format = 0;
}

//////////////////////////////////////////////////////////////////////////
WaveDataSource::~WaveDataSource()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
void WaveDataSource::Cleanup()
{
	if (m_File) Game::GetInstance()->GetFileMgr()->CloseFile(m_File);
	memset(&m_Header, 0, sizeof(WaveFormatEx));

	m_WaveDataOffset = 0;
	m_TotalDataLength = 0;
	m_BufferSize = 0;
	m_Format = 0;

	m_Initialized = false;
}

//////////////////////////////////////////////////////////////////////////
bool WaveDataSource::InitializeFromFile(const WideString& fileName)
{
	Cleanup();

	m_File = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!m_File) return false;


	RiffChunk chunk;

	m_File->Read(&chunk, sizeof(chunk));

	char buf[4];
	m_File->Read(&buf, 4);

	if(strncmp(chunk.chunkID, "RIFF", 4) != 0 || strncmp(buf, "WAVE", 4) != 0)
	{
		Game::GetInstance()->Log(fileName + L": wrong file header");
		return false;
	}

	m_File->Read(&chunk, sizeof(chunk));
	if(strncmp(chunk.chunkID, "fmt ", 4) != 0)
	{
		Game::GetInstance()->Log(fileName + L": wrong file header");
		return false;
	}

	if (chunk.length > sizeof(WaveFormatEx))
	{
		Game::GetInstance()->Log(fileName + L": wrong file header");
		return false;
	}

	m_File->Read(&m_Header, chunk.length);
	
	// 0x0001 = WAVE_FORMAT_PCM, 0xFFFE = WAVE_FORMAT_EXTENSIBLE
	if (m_Header.wFormatTag != 0x0001 && m_Header.wFormatTag != 0xFFFE)
	{
		Game::GetInstance()->Log(fileName + L": only uncompressed WAV files are supported");
		return false;
	}

	if ((m_Header.wBitsPerSample != 8) && (m_Header.wBitsPerSample != 16))
	{
		Game::GetInstance()->Log(fileName + L": only 8/16 bits-per-sample WAV files are supported");
		return false;
	}

	if (!ReadSoundInfo())
	{
		Game::GetInstance()->Log(fileName + L": unsupported file format");
		return false;
	}

	// seek to 'data' chunk
	do
	{
		m_File->Read(&chunk, sizeof(chunk));
		if(strncmp(chunk.chunkID, "data", 4) == 0) break;

		m_File->Seek(chunk.length, File::SEEK_FROM_CURRENT);
	} while (!m_File->IsEOF());

	if (m_File->IsEOF())
	{
		Game::GetInstance()->Log(fileName + L": file contains no data");
		return false;
	}

	// store data position and length
	m_WaveDataOffset = m_File->GetPos();
	m_TotalDataLength = chunk.length;


	m_Initialized = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool WaveDataSource::ReadSoundInfo()
{
	switch (m_Header.nChannels)
	{
	case 1:
		{
			if (m_Header.wBitsPerSample == 8)
			{
				// 8-bit mono
				m_Format = AL_FORMAT_MONO8;

				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				m_BufferSize = m_Header.nSamplesPerSec / 4;
			}
			else
			{
				// 16-bit mono
				m_Format = AL_FORMAT_MONO16;

				// Queue 250ms of audio data
				m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
			}
		}
		break;
	case 2:
		{
			if (m_Header.wBitsPerSample == 8)
			{
				// 8-bit stereo
				m_Format = AL_FORMAT_STEREO8;

				// Set BufferSize to 250ms (Frequency * 2 (8bit stereo) divided by 4 (quarter of a second))
				m_BufferSize = m_Header.nSamplesPerSec >> 1;

				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				m_BufferSize -= (m_BufferSize % 2);
			}
			else
			{
				// 16-bit stereo
				m_Format = AL_FORMAT_STEREO16;

				// Queue 250ms of audio data
				m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
			}
		}
		break;
	case 4:
		{
			// 16-bit Quad surround
			m_Format = alGetEnumValue("AL_FORMAT_QUAD16");
			if (!m_Format) return false;

			// Queue 250ms of audio data
			m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
		}
		break;
	case 6:
		{
			// 16-bit 5.1 surround
			m_Format = alGetEnumValue("AL_FORMAT_51CHN16");
			if (!m_Format) return false;

			// Queue 250ms of audio data
			m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
		}
		break;
	case 7:
		{
			// 16-bit 7.1 surround
			m_Format = alGetEnumValue("AL_FORMAT_61CHN16");
			if (!m_Format) return false;

			// Queue 250ms of audio data
			m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
		}
		break;
	case 8:
		{
			// 16-bit 8.1 surround
			m_Format = alGetEnumValue("AL_FORMAT_71CHN16");
			if (!m_Format) return false;

			// Queue 250ms of audio data
			m_BufferSize = m_Header.nAvgBytesPerSec >> 2;

			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			m_BufferSize -= (m_BufferSize % m_Header.nBlockAlign);
		}
		break;
		
	default:
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
size_t WaveDataSource::ReadData(size_t size, byte* buffer)
{
	if (!m_Initialized) return 0;
	else
	{
		size_t dataLeft = m_TotalDataLength - (m_File->GetPos() - m_WaveDataOffset);
		if (size > dataLeft) size = dataLeft;
		return m_File->Read(buffer, size);
	}
}

//////////////////////////////////////////////////////////////////////////
bool WaveDataSource::IsEof() const
{
	if (!m_Initialized) return true;
	else return m_File->GetPos() >= m_WaveDataOffset + m_TotalDataLength;
}

//////////////////////////////////////////////////////////////////////////
void WaveDataSource::SkipToTime(float seconds)
{
	if (!m_Initialized) return;

	m_File->Seek(TimeToOffset(seconds), File::SEEK_FROM_BEGIN);
}

//////////////////////////////////////////////////////////////////////////
float WaveDataSource::GetTotalTime() const
{
	if (!m_Initialized) return 0.0f;

	return OffsetToTime(m_WaveDataOffset + m_TotalDataLength);
}

//////////////////////////////////////////////////////////////////////////
float WaveDataSource::GetCurrentTime() const
{
	if (!m_Initialized) return 0.0f;

	return OffsetToTime(m_File->GetPos());
}

//////////////////////////////////////////////////////////////////////////
int WaveDataSource::GetNumChannels() const
{
	if (!m_Initialized) return 0;
	else return m_Header.nChannels;
}

//////////////////////////////////////////////////////////////////////////
size_t WaveDataSource::GetBufferSize() const
{
	if (!m_Initialized) return 0;
	else return m_BufferSize;
}

//////////////////////////////////////////////////////////////////////////
float WaveDataSource::GetBufferLengthSec() const
{
	if (!m_Initialized) return 0;
	else return (float)(m_BufferSize * 8 / (m_Header.nSamplesPerSec * m_Header.nChannels * m_Header.wBitsPerSample));

}

//////////////////////////////////////////////////////////////////////////
ALenum WaveDataSource::GetFormat() const
{
	if (!m_Initialized) return 0;
	else return m_Format;
}

//////////////////////////////////////////////////////////////////////////
int WaveDataSource::GetFrequency() const
{
	if (!m_Initialized) return 0;
	else return m_Header.nSamplesPerSec;
}

//////////////////////////////////////////////////////////////////////////
float WaveDataSource::OffsetToTime(size_t offset) const
{
	offset -= m_WaveDataOffset;
	return (float)(((offset) * 8) /(m_Header.nSamplesPerSec * m_Header.nChannels * m_Header.wBitsPerSample));
}

//////////////////////////////////////////////////////////////////////////
size_t WaveDataSource::TimeToOffset(float time) const
{
	size_t offset = (size_t)(time * (m_Header.nSamplesPerSec * m_Header.nChannels * m_Header.wBitsPerSample) / 8);
	offset += m_WaveDataOffset;

	return offset;
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeWaveDataSource_H__
#define __WmeWaveDataSource_H__


#include "SoundDataSource.h"


namespace Wme
{
	class File;

	class WmeDllExport WaveDataSource : public SoundDataSource
	{
	public:
		WaveDataSource();
		~WaveDataSource();

		bool InitializeFromFile(const WideString& fileName);
		size_t ReadData(size_t size, byte* buffer);
		bool IsEof() const;
		void SkipToTime(float seconds);
		float GetTotalTime() const;
		float GetCurrentTime() const;
		int GetNumChannels() const;
		size_t GetBufferSize() const;
		float GetBufferLengthSec() const;
		ALenum GetFormat() const;
		int GetFrequency() const;

	private:
		
#pragma pack(push, 4)
		typedef struct
		{
			char chunkID[4];
			dword length;
		} RiffChunk;

		typedef struct
		{
			word wFormatTag;
			word nChannels;
			dword nSamplesPerSec;
			dword nAvgBytesPerSec;
			word nBlockAlign;
			word wBitsPerSample;
			word cbSize;
			word wReserved;
			dword dwChannelMask;
			byte SubFormat[16];
		} WaveFormatEx;
#pragma pack(pop)

		File* m_File;
		WaveFormatEx m_Header;
		size_t m_WaveDataOffset;
		dword m_TotalDataLength;

		size_t m_BufferSize;
		ALenum m_Format;

		bool ReadSoundInfo();
		float OffsetToTime(size_t offset) const;
		size_t TimeToOffset(float time) const;

		void Cleanup();


	};
}

#endif // __WmeWaveDataSource_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeVorbisDataSource_H__
#define __WmeVorbisDataSource_H__


#include "SoundDataSource.h"
#include <vorbis/vorbisfile.h>


namespace Wme
{
	class File;

	class WmeDllExport VorbisDataSource : public SoundDataSource
	{
	public:
		VorbisDataSource();
		~VorbisDataSource();

		bool InitializeFromFile(const WideString& fileName);
		size_t ReadData(size_t size, byte* buffer);
		bool IsEof() const;
		void SkipToTime(float seconds);
		float GetTotalTime() const;
		float GetCurrentTime() const;
		int GetNumChannels() const { return m_NumChannels; }
		size_t GetBufferSize() const { return m_BufferSize; }
		float GetBufferLengthSec() const { return 0.25f; }
		ALenum GetFormat() const { return m_Format; }
		int GetFrequency() const;

	private:
		File* m_File;

		int m_NumChannels;
		size_t m_BufferSize;
		ALenum m_Format;
		bool m_IsEof;
		
		mutable OggVorbis_File m_VorbisStream;
		vorbis_info* m_VorbisInfo;

		void Cleanup();
		void ReadSoundInfo();

		static size_t FileRead(void* ptr, size_t size, size_t nmemb, void* datasource);
		static int FileSeek(void* datasource, ogg_int64_t offset, int whence);
		static int FileClose(void* datasource);
		static long FileTell(void* datasource);
	};
}

#endif // __WmeVorbisDataSource_H__

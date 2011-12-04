// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeZlibStream_H__
#define __WmeZlibStream_H__

#include <zlib.h>
#include "File.h"


#define COMPRESSED_BUFFER_SIZE 4096

namespace Wme
{
	class WmeDllExport ZlibStream
	{
	public:
		ZlibStream(std::ifstream* fileStream, size_t compressedSize, size_t uncompressedSize, size_t streamStart);
		virtual ~ZlibStream();

		bool SkipToPos(size_t pos);
		size_t Read(void* buffer, size_t size);


	private:
		std::ifstream* m_FileStream;
		size_t m_CompressedSize;
		size_t m_UncompressedSize;
		size_t m_StreamStart;

		z_stream m_Stream;
		byte m_CompBuffer[COMPRESSED_BUFFER_SIZE];
		bool m_InflateRunning;

	};
}


#endif // __WmeZlibStream_H__
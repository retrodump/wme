// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ZlibStream.h"
#include "Game.h"
#include "StringUtil.h"
#include <algorithm>


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment (lib, "zlib_d.lib")
#	else
#		pragma comment (lib, "zlib.lib")
#	endif
#endif


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
ZlibStream::ZlibStream(std::ifstream* fileStream, size_t compressedSize, size_t uncompressedSize, size_t streamStart)
{
	m_FileStream = fileStream;
	m_CompressedSize = compressedSize;
	m_UncompressedSize = uncompressedSize;
	m_StreamStart = streamStart;
	m_InflateRunning = false;

	memset(&m_Stream, 0, sizeof(z_stream));
	

	SkipToPos(0);
}


//////////////////////////////////////////////////////////////////////////
ZlibStream::~ZlibStream()
{
	if (m_InflateRunning) inflateEnd(&m_Stream);
}

//////////////////////////////////////////////////////////////////////////
size_t ZlibStream::Read(void* buffer, size_t size)
{
	dword initOut = m_Stream.total_out;

	m_Stream.avail_out = size;
	m_Stream.next_out = static_cast<byte*>(buffer);

	while (m_Stream.total_out - initOut < size && m_Stream.total_in < m_CompressedSize)
	{
		// needs to read more data?
		if (m_Stream.avail_in == 0)
		{
			m_Stream.avail_in = std::min<int>(COMPRESSED_BUFFER_SIZE, m_CompressedSize - m_Stream.total_in);

			m_FileStream->seekg(m_StreamStart + m_Stream.total_in, std::ios::beg);
			m_FileStream->read(reinterpret_cast<char*>(m_CompBuffer), m_Stream.avail_in);
			m_Stream.next_in = m_CompBuffer;
		}

		int res = inflate(&m_Stream, Z_SYNC_FLUSH);
		if (res != Z_OK && res != Z_STREAM_END)
		{
			Game::GetInstance()->Log(L"zlib error: " + StringUtil::ToString(res));
			return 0;
		}
	}
	return m_Stream.total_out - initOut;
}

#define STREAM_BUFFER_SIZE 4096
//////////////////////////////////////////////////////////////////////////
bool ZlibStream::SkipToPos(size_t pos)
{
	byte streamBuffer[STREAM_BUFFER_SIZE];
	if(m_InflateRunning) inflateEnd(&m_Stream);

	m_Stream.avail_in = 0;
	m_Stream.next_in = m_CompBuffer;
	m_Stream.avail_out = std::min<int>(STREAM_BUFFER_SIZE, pos);
	m_Stream.next_out = streamBuffer;

	inflateInit(&m_Stream);
	m_InflateRunning = true;

	while (m_Stream.total_out < pos && m_Stream.total_in < m_CompressedSize)
	{
		// needs to read more data?
		if (m_Stream.avail_in == 0)
		{
			m_Stream.avail_in = std::min<int>(COMPRESSED_BUFFER_SIZE, m_CompressedSize - m_Stream.total_in);
			m_FileStream->seekg(m_StreamStart + m_Stream.total_in, std::ios::beg);
			m_FileStream->read(reinterpret_cast<char*>(m_CompBuffer), m_Stream.avail_in);
			m_Stream.next_in = m_CompBuffer;
		}

		// needs more space?
		if (m_Stream.avail_out == 0)
		{
			m_Stream.next_out = streamBuffer;
			m_Stream.avail_out = std::min<int>(STREAM_BUFFER_SIZE, pos - m_Stream.total_out);
		}

		// stream on!
		int res = inflate(&m_Stream, Z_SYNC_FLUSH);
		if (res != Z_OK && res != Z_STREAM_END)
		{
			return false;
		}
	}
	return true;
}


} // namespace Wme

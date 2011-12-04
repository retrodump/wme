// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DiskFile.h"
#include "FileManager.h"
#include "Game.h"
#include "ZlibStream.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
DiskFile::DiskFile()
{
	m_IsCompressed = false;
	m_ZlibStream = NULL;
}

//////////////////////////////////////////////////////////////////////////
DiskFile::~DiskFile()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////
bool DiskFile::Open(const WideString& fileName)
{
	WideString fullPath = Game::GetInstance()->GetFileMgr()->FindDiskFile(fileName);
	if (fullPath.empty()) return false;

#ifdef WIN32
	m_Stream.open(fullPath.c_str(), std::ios::binary);
#else
	m_Stream.open(StringUtil::WideToAnsi(fullPath).c_str(), std::ios::binary);
#endif
	if (!m_Stream.is_open())
	{
		m_Stream.clear();
		return false;
	}

	m_FileName = fullPath;

	ReinitializeStream();

	return true;
}

//////////////////////////////////////////////////////////////////////////
void DiskFile::ReinitializeStream()
{
	size_t originalOffset = m_Stream.tellg();

	dword magic1, magic2;
	m_Stream.read(reinterpret_cast<char*>(&magic1), sizeof(dword));
	m_Stream.read(reinterpret_cast<char*>(&magic2), sizeof(dword));

	// compressed file layout:
	//   dword WME_COMPRESSED_MAGIC_1
	//   dword WME_COMPRESSED_MAGIC_2
	//   dword uncompressed size
	//   dword compressed size
	//   byte[compressed size] data

	if (magic1 == WME_COMPRESSED_MAGIC_1 && magic2 == WME_COMPRESSED_MAGIC_2)
	{
		m_IsCompressed = true;

		dword uncompressedSize, compressedSize;
		m_Stream.read(reinterpret_cast<char*>(&uncompressedSize), sizeof(dword));
		m_Stream.read(reinterpret_cast<char*>(&compressedSize), sizeof(dword));

		m_Size = uncompressedSize;

		SAFE_DELETE(m_ZlibStream);
		m_ZlibStream = new ZlibStream(&m_Stream, compressedSize, uncompressedSize, m_Stream.tellg());
	}
	else
	{
		m_IsCompressed = false;

		m_Stream.seekg(0, std::ios::end);
		m_Size = m_Stream.tellg();

		m_Stream.seekg(originalOffset, std::ios::beg);
	}

}

//////////////////////////////////////////////////////////////////////////
void DiskFile::Close()
{
	SAFE_DELETE(m_ZlibStream);
	if (m_Stream.is_open()) m_Stream.close();
}

//////////////////////////////////////////////////////////////////////////
bool DiskFile::Seek(size_t pos, SeekOrigin origin)
{
	if (!m_IsCompressed)
	{
		bool ret = false;
		m_Stream.clear();

		switch (origin)
		{
		case SEEK_FROM_BEGIN:
			m_Stream.seekg(pos, std::ios::beg);
			ret = !m_Stream.fail();
			break;
		case SEEK_FROM_END:
			m_Stream.seekg(pos, std::ios::end);
			ret = !m_Stream.fail();
			break;
		case SEEK_FROM_CURRENT:
			m_Stream.seekg(pos, std::ios::cur);
			ret = !m_Stream.fail();
			break;
		}
		if (ret) m_Pos = m_Stream.tellg();
		else m_Pos = m_Size;

		return ret;
	}
	else
	{
		size_t newPos = m_Pos;

		switch (origin)
		{
		case SEEK_FROM_BEGIN:
			newPos = pos;
			break;
		case SEEK_FROM_END:
			newPos = m_Size + pos;
			break;
		case SEEK_FROM_CURRENT:
			newPos = m_Pos + pos;
			break;
		}

		if (newPos == m_Pos) return true;
		else
		{
			bool ret = m_ZlibStream->SkipToPos(newPos);
			if (ret) m_Pos = newPos;
			else m_Pos = m_Size;

			return ret;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
size_t DiskFile::Read(void* buffer, size_t size)
{
	if (!m_IsCompressed)
	{
		m_Stream.read(static_cast<char*>(buffer), size);		

		size_t readSize = m_Stream.gcount();
		m_Pos += readSize;
		return readSize;
	}
	else
	{
		size_t readSize = m_ZlibStream->Read(buffer, size);
		m_Pos += readSize;
		return readSize;
	}
}



} // namespace Wme

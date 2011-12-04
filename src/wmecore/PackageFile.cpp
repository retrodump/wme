// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PackageFile.h"
#include "PackageEntry.h"
#include "ZlibStream.h"
#include "Game.h"
#include "FileManager.h"


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
PackageFile::PackageFile()
{
	m_Entry = NULL;
	m_ZlibStream = NULL;
}

//////////////////////////////////////////////////////////////////////////
PackageFile::~PackageFile()
{
	Close();
}

//////////////////////////////////////////////////////////////////////////
bool PackageFile::Open(const WideString& fileName)
{
	m_Entry = Game::GetInstance()->GetFileMgr()->FindPackageEntry(fileName);
	if (!m_Entry) return false;

	if (m_Entry->IsCompressed())
	{
		m_ZlibStream = new ZlibStream(&m_Entry->GetPackage()->GetStream(), m_Entry->GetCompressedSize(), m_Entry->GetSize(), m_Entry->GetOffset());
	}

	m_Size = m_Entry->GetSize();

	return true;
}

//////////////////////////////////////////////////////////////////////////
void PackageFile::Close()
{
	m_Entry = NULL;
	SAFE_DELETE(m_ZlibStream);
}

//////////////////////////////////////////////////////////////////////////
size_t PackageFile::Read(void* buffer, size_t size)
{
	if (!m_Entry) return 0;
	
	if (m_Entry->IsCompressed())
	{
		size_t readSize = m_ZlibStream->Read(buffer, size);
		m_Pos += readSize;
		return readSize;
	}
	else
	{
		std::ifstream& stream = m_Entry->GetPackage()->GetStream();

		stream.seekg(m_Entry->GetOffset() + m_Pos);
		stream.read(static_cast<char*>(buffer), size);		

		size_t readSize = stream.gcount();
		m_Pos += readSize;
		return readSize;
	}
}

//////////////////////////////////////////////////////////////////////////
bool PackageFile::Seek(size_t pos, SeekOrigin origin)
{
	if (!m_Entry) return false;


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

	if (m_Entry->IsCompressed())
	{
		if (newPos == m_Pos) return true;

		bool ret = m_ZlibStream->SkipToPos(newPos);
		if (ret) m_Pos = newPos;
		else m_Pos = m_Size;

		return ret;
	}
	else
	{
		bool ret = true;

		if (newPos > m_Size)
		{
			newPos = m_Size;
			ret = false;
		}
		if (newPos < 0)
		{
			newPos = 0;
			ret = false;
		}

		return ret;
	}
}

//////////////////////////////////////////////////////////////////////////
WideString PackageFile::GetFileName()
{
	if (!m_Entry) return L"";
	return m_Entry->GetFileName();
}


} // namespace Wme

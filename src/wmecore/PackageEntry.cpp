// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PackageEntry.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
PackageEntry::PackageEntry(Package* package, const WideString& fileName, dword offset, dword size, dword compressedSize, bool isCompressed, time_t time)
{
	m_Package = package;
	m_FileName = fileName;
	m_Size = size;
	m_CompressedSize = compressedSize;
	m_Offset = offset;
	m_IsCompressed = isCompressed;
	m_Time = time;
}

//////////////////////////////////////////////////////////////////////////
PackageEntry::~PackageEntry()
{
}


} // namespace Wme

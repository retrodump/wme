// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Package.h"
#include "FileManager.h"
#include "Game.h"
#include "StringUtil.h"
#include "PathUtil.h"
#include "DateTimeUtil.h"


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
Package::Package()
{
	m_Priority = 0;
}

//////////////////////////////////////////////////////////////////////////
Package::~Package(void)
{
	if (m_Stream.is_open()) m_Stream.close();
}

//////////////////////////////////////////////////////////////////////////
bool Package::Load(WideString fileName)
{
#ifdef WIN32
	m_Stream.open(fileName.c_str(), std::ios::binary);
#else
	m_Stream.open(StringUtil::WideToAnsi(fileName).c_str(), std::ios::binary);
#endif
	if (!m_Stream.is_open())
	{
		Game::GetInstance()->Log(L"Error opening package file '" + fileName + L"'.");
		return false;
	}

	PackageHeader header;
	m_Stream.read(reinterpret_cast<char*>(&header), sizeof(header));

	if (header.Magic1 != WME_PACKAGE_MAGIC_1 || header.Magic2 != WME_PACKAGE_MAGIC_2)
	{
		m_Stream.close();
		Game::GetInstance()->Log(L"Invalid package header: '" + fileName + L"'.");
		return false;
	}

	if (header.Version != WME_PACKAGE_VERSION)
	{
		m_Stream.close();
		Game::GetInstance()->Log(L"Wrong package version: '" + fileName + L"'.");
		return false;
	}

	// dir offset
	dword directoryOffset;
	m_Stream.read(reinterpret_cast<char*>(&directoryOffset), sizeof(dword));


	byte descriptionLength;
	m_Stream.read(reinterpret_cast<char*>(&descriptionLength), sizeof(byte));

	char* description = new char[descriptionLength + 1];
	m_Stream.read(description, descriptionLength);
	description[descriptionLength] = '\0';

	// fill package properties
	m_FileName = fileName;
	m_Priority = header.Priority;
	m_Description = StringUtil::Utf8ToWide(description);
	m_CreationTime = DateTimeUtil::BuildTime(header.CreationYear, header.CreationMonth, header.CreationDay, header.CreationHour, header.CreationMinute, header.CreationSecond);

	delete [] description;

	// jump to directory
	m_Stream.seekg(directoryOffset);

	dword numItems;
	m_Stream.read(reinterpret_cast<char*>(&numItems), sizeof(dword));


	for (dword i = 0; i < numItems; i++)
	{
		ReadDirectoryEntry();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Package::ReadDirectoryEntry()
{
	FileManager* fileMgr = Game::GetInstance()->GetFileMgr();


	FileEntryHeader header;
	m_Stream.read(reinterpret_cast<char*>(&header), sizeof(header));

	word fileNameLength;
	m_Stream.read(reinterpret_cast<char*>(&fileNameLength), sizeof(word));

	char* fileName = new char[fileNameLength + 1];
	m_Stream.read(fileName, fileNameLength);
	fileName[fileNameLength] = '\0';

	WideString fileNameWide = StringUtil::Utf8ToWide(fileName);
	delete [] fileName;

	fileNameWide = PathUtil::UnifySeparators(fileNameWide);

	time_t time = DateTimeUtil::BuildTime(header.ModYear, header.ModMonth, header.ModDay, header.ModHour, header.ModMinute, header.ModSecond);

	fileMgr->AddPackageEntry(this, fileNameWide, header.Offset, header.OriginalSize, header.CompressedSize, header.IsCompressed != 0, time);
}


} // namespace Wme

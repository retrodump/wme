// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PackageBuilder.h"
#include "Game.h"
#include "PackageBuilderFilter.h"
#include "PathUtil.h"
#include "StringUtil.h"
#include "DateTimeUtil.h"
#include "WmePackage.h"
#include "PackageEntry.h"


namespace Wme
{
	
//////////////////////////////////////////////////////////////////////////
PackageBuilder::PackageBuilder()
{
	m_Filters.push_back(new PackageBuilderFilterIgnore());
	m_Filters.push_back(new PackageBuilderFilterScript());
	m_Filters.push_back(new PackageBuilderFilterUncompressed());
	m_Filters.push_back(new PackageBuilderFilterDefault());

	FilterList::iterator it;
	for (it = m_Filters.begin(); it != m_Filters.end(); it++)
	{
		(*it)->Initialize();
	}

}

//////////////////////////////////////////////////////////////////////////
PackageBuilder::~PackageBuilder()
{
	FilterList::iterator it;
	for (it = m_Filters.begin(); it != m_Filters.end(); it++)
	{
		delete (*it);
	}

	DeleteEntries();
}

//////////////////////////////////////////////////////////////////////////
void PackageBuilder::DeleteEntries()
{
	PackageEntries::iterator it;
	for (it = m_PackageEntries.begin(); it != m_PackageEntries.end(); it++)
	{
		delete (*it);
	}
	m_PackageEntries.clear();
}

//////////////////////////////////////////////////////////////////////////
bool PackageBuilder::ProcessDirectory(const WideString& packageDir, const WideString& outputPath, int priority, const WideString& description)
{
	DeleteEntries();

	WideString inputDir = PathUtil::UnifySeparators(packageDir);
	if (!StringUtil::EndsWith(inputDir, L"/")) inputDir += L"/";

	WideStringListPtr list = StringUtil::Split(inputDir, L"/");
	if (list->size() == 0) return false;

	WideString packageFile = list->back() + L"." + WME_PACKAGE_EXTENSION;
	packageFile = PathUtil::Combine(outputPath, packageFile);

	std::ofstream stream;
#ifdef WIN32
	stream.open(packageFile.c_str(), std::ios::binary);
#else
	stream.open(StringUtil::WideToAnsi(packageFile).c_str(), std::ios::binary);
#endif
	if (!stream.is_open()) return false;


	// header
	PackageHeader header;
	memset(&header, 0, sizeof(PackageHeader));
	header.Magic1 = WME_PACKAGE_MAGIC_1;
	header.Magic2 = WME_PACKAGE_MAGIC_2;
	header.Version = WME_PACKAGE_VERSION;
	header.Priority = priority;
	header.Reserved1 = 0;
	header.Reserved2 = 0;

	time_t currTime;
	time(&currTime);
	DateTimeUtil::ParseTime(currTime, header.CreationYear, header.CreationMonth, header.CreationDay, header.CreationHour, header.CreationMinute, header.CreationSecond);

	stream.write(reinterpret_cast<const char*>(&header), sizeof(PackageHeader));

	// directory offset
	size_t directoryOffsetPos = stream.tellp();
	dword directoryOffset = 0;
	stream.write(reinterpret_cast<const char*>(&directoryOffset), sizeof(dword));


	// description
	Utf8String descUtf8 = StringUtil::WideToUtf8(description);
	byte descLength = descUtf8.size();
	stream.write(reinterpret_cast<const char*>(&descLength), sizeof(byte));
	stream.write(descUtf8.c_str(), descUtf8.size());


	// files
	ProcessFiles(wpath(inputDir), inputDir, stream);


	// remember directory position
	directoryOffset = stream.tellp();


	// write drectory
	dword numFiles = m_PackageEntries.size();
	stream.write(reinterpret_cast<const char*>(&numFiles), sizeof(dword));

	PackageEntries::const_iterator it;
	for (it = m_PackageEntries.begin(); it != m_PackageEntries.end(); it++)
	{
		// write file header
		FileEntryHeader header;
		memset(&header, 0, sizeof(FileEntryHeader));

		header.Offset = (*it)->GetOffset();
		header.OriginalSize = (*it)->GetSize();
		header.CompressedSize = (*it)->GetCompressedSize();
		header.IsCompressed = (*it)->IsCompressed();
		DateTimeUtil::ParseTime((*it)->GetTime(), header.ModYear, header.ModMonth, header.ModDay, header.ModHour, header.ModMinute, header.ModSecond);

		stream.write(reinterpret_cast<const char*>(&header), sizeof(FileEntryHeader));

		// write file name
		Utf8String fileName = StringUtil::WideToUtf8((*it)->GetFileName());
		word fileNameLength = fileName.length();
		stream.write(reinterpret_cast<const char*>(&fileNameLength), sizeof(word));
		stream.write(reinterpret_cast<const char*>(fileName.c_str()), fileNameLength);
	}


	// write directory position
	stream.seekp(directoryOffsetPos);
	stream.write(reinterpret_cast<const char*>(&directoryOffset), sizeof(dword));
	

	stream.close();
	return true;
}

//////////////////////////////////////////////////////////////////////////
void PackageBuilder::ProcessFiles(const wpath& currentPath, const WideString& basePath, std::ofstream& packageStream)
{
	wdirectory_iterator endIter;	// DEPRECATED - pouzit neco jinyho
	for (wdirectory_iterator it(currentPath); it != endIter; it++)
	{
		if (is_directory(it->status()))
		{
			ProcessFiles(it->path(), basePath, packageStream);
		}
		else
		{
			ProcessSingleFile(it->path(), basePath, packageStream);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void PackageBuilder::ProcessSingleFile(const wpath& currentPath, const WideString& basePath, std::ofstream& packageStream)
{
	WideString fullPath = currentPath.string();	// neexistujici operace prirazeni


	FilterList::const_iterator it;

	for (it = m_Filters.begin(); it != m_Filters.end(); it++)
	{
		if ((*it)->MatchesFile(fullPath))
		{
			PackageFilterInfo info;
			memset(&info, 0, sizeof(PackageFilterInfo));

			info.OrigFileName = fullPath;
			if (!(*it)->ProcessFile(info)) return;

			if (info.Processing == PackageFilterInfo::FILTER_SKIP) return;

			WideString relativeFileName = info.NewFileName;
			if (relativeFileName.empty()) relativeFileName = info.OrigFileName;
			relativeFileName = relativeFileName.substr(basePath.length());

			PackageEntry* entry = new PackageEntry(NULL, relativeFileName, packageStream.tellp(), info.OrigSize, info.BufferSize, info.IsCompressed, last_write_time(currentPath));
			m_PackageEntries.push_back(entry);

			packageStream.write(reinterpret_cast<const char*>(info.Buffer), info.BufferSize);
			delete [] info.Buffer;
		}
	}
}


} // namespace Wme

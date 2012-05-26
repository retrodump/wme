// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FileManager.h"
#include "StringUtil.h"
#include "PathUtil.h"
#include "DiskFile.h"
#include "Game.h"
#include "PackageEntry.h"
#include "File.h"
#include "DiskFile.h"
#include "PackageFile.h"
#include <boost/filesystem.hpp>
#include <zlib.h>

using namespace boost::filesystem;

namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
FileManager::FileManager()
{
}

//////////////////////////////////////////////////////////////////////////
FileManager::~FileManager()
{
	foreach (File* file, m_OpenFiles)
	{
		SAFE_DELETE(file);
	}

	PackageEntries::iterator eit;
	for (eit = m_PackageEntries.begin(); eit != m_PackageEntries.end(); eit++)
	{
		SAFE_DELETE((*eit).second);
	}

	foreach (Package* package, m_Packages)
	{
		SAFE_DELETE(package);
	}
}

//////////////////////////////////////////////////////////////////////////
void FileManager::Initialize()
{
	InitSearchPaths();
	ScanPackages();

}

//////////////////////////////////////////////////////////////////////////
void FileManager::AddSearchPaths(WideStringList& list, const WideString& toAdd, const WideString& delimiters)
{
	WideStringListPtr listToAdd = StringUtil::Split(toAdd, delimiters);
	
	WideStringList::const_iterator it;
	for (it = listToAdd->begin(); it != listToAdd->end(); it++)
	{
		WideString path = PathUtil::UnifySeparators(*it);
		if (!StringUtil::EndsWith(path, L"/")) path += L"/";

		WideStringList::const_iterator it2;
		bool found = false;
		for (it2 = list.begin(); it2 != list.end(); it2++)
		{			
			if (*it2 == path)
			{
				found = true;
				break;
			}
		}
		if (!found) list.push_back(path);
	}
}

//////////////////////////////////////////////////////////////////////////
WideString FileManager::FindDiskFile(const WideString& fileName)
{
	return FindFile(m_FileSearchPaths, fileName);
}

//////////////////////////////////////////////////////////////////////////
WideString FileManager::FindPackageFile(const WideString& fileName)
{
	return FindFile(m_PackageSearchPaths, fileName);
}

//////////////////////////////////////////////////////////////////////////
WideString FileManager::FindFile(WideStringList& list, const WideString& fileName)
{
	WideStringList::const_iterator it;
	for (it = list.begin(); it != list.end(); it++)
	{
		WideString fullPath = PathUtil::Combine(*it, fileName);
		if (PathUtil::FileExists(fullPath)) return fullPath;
	}

	if (PathUtil::FileExists(fileName)) return fileName;
	else return L"";
}


//////////////////////////////////////////////////////////////////////////
bool FileManager::SaveBufferToFile(const WideString& fileName, byte* buffer, size_t size, bool compressed)
{
	std::ofstream stream;
	
#ifdef WIN32
	stream.open(fileName.c_str(), std::ios::binary);
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str(), std::ios::binary);
#endif
	if (!stream.is_open())
	{
		Game::GetInstance()->Log(L"Error creating file '" + fileName + L"'.");
		return false;
	}


	if (compressed)
	{
		dword compSize = size + (size / 100) + 12; // 1% extra space
		byte* compBuffer = new byte[compSize];
		if(!compBuffer)
		{
			Game::GetInstance()->Log(L"Error allocating compression buffer while saving '" + fileName + L"'.");
		}
		else
		{
			if(compress(compBuffer, &compSize, buffer, size) == Z_OK)
			{
				dword magic, dwSize;
				
				magic = WME_COMPRESSED_MAGIC_1;
				stream.write(reinterpret_cast<char*>(&magic), sizeof(dword));

				magic = WME_COMPRESSED_MAGIC_2;
				stream.write(reinterpret_cast<char*>(&magic), sizeof(dword));
				
				dwSize = size;
				stream.write(reinterpret_cast<char*>(&dwSize), sizeof(dword));

				dwSize = compSize;
				stream.write(reinterpret_cast<char*>(&dwSize), sizeof(dword));

				stream.write(reinterpret_cast<char*>(compBuffer), compSize);

				delete [] compBuffer;
				stream.close();

				return true;
			}
			else
			{
				Game::GetInstance()->Log(L"Error compressing buffer while saving '" + fileName + L"'.");
				delete [] compBuffer;
			}
		}
	}

	stream.write(reinterpret_cast<char*>(buffer), size);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool FileManager::AddPackageEntry(Package* package, const WideString& fileName, dword offset, dword size, dword compressedSize, bool isCompressed, time_t time)
{
	WideString normFileName = PathUtil::NormalizeFileName(fileName);

	PackageEntries::iterator it;
	it = m_PackageEntries.find(normFileName);

	bool addEntry = false;

	if (it == m_PackageEntries.end())
	{
		addEntry = true;
	}
	else if((*it).second->GetPackage()->GetPriority() < package->GetPriority())
	{
		SAFE_DELETE((*it).second);
		addEntry = true;
	}

	if (addEntry)
	{
		PackageEntry* entry = new PackageEntry(package, fileName, offset, size, compressedSize, isCompressed, time);
		m_PackageEntries[normFileName] = entry;
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
PackageEntry* FileManager::FindPackageEntry(const WideString& fileName) const
{
	WideString normFileName = PathUtil::NormalizeFileName(fileName);

	PackageEntries::const_iterator it;
	it = m_PackageEntries.find(normFileName);
	if (it == m_PackageEntries.end()) return NULL;
	else return (*it).second;
}

//////////////////////////////////////////////////////////////////////////
File* FileManager::OpenFile(const WideString& fileName)
{
	DiskFile* diskFile = new DiskFile();
	if (diskFile->Open(fileName))
	{
		m_OpenFiles.insert(diskFile);
		return diskFile;
	}
	delete diskFile;

	PackageFile* pkgFile = new PackageFile();
	if (pkgFile->Open(fileName))
	{
		m_OpenFiles.insert(pkgFile);
		return pkgFile;
	}

	delete pkgFile;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool FileManager::CloseFile(File*& file)
{
	FileList::iterator it;

	it = m_OpenFiles.find(file);
	if (it == m_OpenFiles.end()) return false;

	delete (*it);
	m_OpenFiles.erase(it);

	file = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool FileManager::FileExists(const WideString& fileName)
{
	File* testFile = OpenFile(fileName);
	if (testFile)
	{
		CloseFile(testFile);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void FileManager::InitSearchPaths()
{
	// TODO load paths from config file
	AddPackageSearchPaths(L".");
	AddFileSearchPaths(L"../data");
#ifndef WIN32
	AddFileSearchPaths(L"/Users/nedoma/Documents/projects/wme2/bin/data/");
#endif
}

//////////////////////////////////////////////////////////////////////////
void FileManager::ScanPackages()
{
	WideStringList::const_iterator it;
	WideString packageMask = WideString(L"*.") + WideString(WME_PACKAGE_EXTENSION);

	Game::GetInstance()->Log(L"Scanning packages...");

	foreach (WideString pathName, m_PackageSearchPaths)
	{
		path searchPath(pathName);
		directory_iterator endIter;
		for (directory_iterator dit = directory_iterator(searchPath); dit != endIter; ++dit)
		{
			directory_entry dirEntry = *dit;
			
			if (!is_directory(dirEntry))
			{
				WideString fileName = dirEntry.path().filename().c_str();
				if (!PathUtil::MatchesMask(fileName, packageMask)) continue;

				Package* package = new Package();
				if (package->Load(dirEntry.path().c_str()))
				{
					m_Packages.push_back(package);
				}
				else delete package;
			}
		}
	}

	Game::GetInstance()->Log(L"  Registered " + StringUtil::ToString(m_PackageEntries.size()) + L" files in " + StringUtil::ToString(m_Packages.size()) + L" package(s)");
}


} // namespace Wme

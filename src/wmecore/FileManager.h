// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFileManager_H__
#define __WmeFileManager_H__

#include "Object.h"


namespace Wme
{
	class PackageEntry;
	class Package;
	class File;

	class WmeDllExport FileManager : public Object
	{
	public:
		FileManager();
		virtual ~FileManager();

		const WideStringList& GetFileSearchPaths()
		{
			return m_FileSearchPaths;
		}

		void AddFileSearchPaths(const WideString& pathList, const WideString& delimiters = L";")
		{
			AddSearchPaths(m_FileSearchPaths, pathList, delimiters);
		}

		const WideStringList& GetPackageSearchPaths()
		{
			return m_FileSearchPaths;
		}

		void AddPackageSearchPaths(const WideString& pathList, const WideString& delimiters = L";")
		{
			AddSearchPaths(m_PackageSearchPaths, pathList, delimiters);
		}

		WideString FindDiskFile(const WideString& fileName);
		WideString FindPackageFile(const WideString& fileName);

		bool SaveBufferToFile(const WideString& fileName, byte* buffer, size_t size, bool compressed);

		bool AddPackageEntry(Package* package, const WideString& fileName, dword offset, dword size, dword compressedSize, bool isCompressed, time_t time);
		PackageEntry* FindPackageEntry(const WideString& fileName) const;

		File* OpenFile(const WideString& fileName);
		bool CloseFile(File*& file);

		bool FileExists(const WideString& fileName);

		void Initialize();


	private:
		WideStringList m_FileSearchPaths;
		WideStringList m_PackageSearchPaths;

		void InitSearchPaths();
		void ScanPackages();

		void AddSearchPaths(WideStringList& list, const WideString& toAdd, const WideString& delimiters);
		WideString FindFile(WideStringList& list, const WideString& fileName);

		typedef std::map<WideString, PackageEntry*> PackageEntries;
		PackageEntries m_PackageEntries;

		typedef std::list<Package*> PackageList;
		PackageList m_Packages;

		typedef std::set<File*> FileList;
		FileList m_OpenFiles;
	};
}

#endif // __WmeFileManager_H__
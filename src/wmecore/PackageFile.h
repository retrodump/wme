// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePackageFile_H__
#define __WmePackageFile_H__

#include "File.h"

namespace Wme
{
	class PackageEntry;
	class ZlibStream;

	class WmeDllExport PackageFile :	public File
	{
	public:
		PackageFile();
		virtual ~PackageFile();

		virtual bool Seek(size_t pos, SeekOrigin origin);
		virtual bool Open(const WideString& fileName);
		virtual void Close();
		virtual size_t Read(void* buffer, size_t size);
		virtual WideString GetFileName();

	private:
		PackageEntry* m_Entry;
		ZlibStream* m_ZlibStream;
	};
}

#endif // __WmePackageFile_H__
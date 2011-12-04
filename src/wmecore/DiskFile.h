// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDiskFile_H__
#define __WmeDiskFile_H__

#include "File.h"

namespace Wme
{

#define WME_COMPRESSED_MAGIC_1	0x32454D57 // WME2
#define WME_COMPRESSED_MAGIC_2	0x42494C5A // ZLIB

	class ZlibStream;

	class WmeDllExport DiskFile : public File
	{
	public:
		DiskFile();
		virtual ~DiskFile();

		virtual bool Seek(size_t pos, SeekOrigin origin);
		virtual bool Open(const WideString& fileName);
		virtual void Close();
		virtual size_t Read(void* buffer, size_t size);

		void ReinitializeStream();

	private:
		bool m_IsCompressed;
		std::ifstream m_Stream;
		ZlibStream* m_ZlibStream;
	};
}


#endif // __WmeDiskFile_H__
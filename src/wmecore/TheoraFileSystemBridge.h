// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTheoraFileSystemBridge_H__
#define __WmeTheoraFileSystemBridge_H__


#include "TheoraDataSource.h"


namespace Wme
{
	class File;

	class TheoraFileSystemBridge : public TheoraDataSource
	{
	public:
		TheoraFileSystemBridge(const WideString& fileName);
		~TheoraFileSystemBridge();

		// TheoraDataSource
		int read(void* output, int nBytes);
		void seek(unsigned long byte_index);
		std::string repr();
		unsigned long size();
		unsigned long tell();

	private:
		File* m_File;
	};
}

#endif // __WmeTheoraFileSystemBridge_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFile_H__
#define __WmeFile_H__

#include "Object.h"

namespace Wme
{
	class WmeDllExport File : public Object
	{
	public:
		enum SeekOrigin
		{
			SEEK_FROM_BEGIN,
			SEEK_FROM_END,
			SEEK_FROM_CURRENT
		};

		File();
		virtual ~File();

		virtual size_t GetSize();
		virtual size_t GetPos();
		virtual bool IsEOF();
		virtual WideString GetFileName();
		virtual bool Seek(size_t pos, SeekOrigin origin) = 0;
		virtual bool Open(const WideString& fileName) = 0;
		virtual void Close() = 0;		
		virtual size_t Read(void* buffer, size_t size) = 0;

	protected:
		WideString m_FileName;
		size_t m_Size;
		size_t m_Pos;

	};
}


#endif // __WmeFile_H__

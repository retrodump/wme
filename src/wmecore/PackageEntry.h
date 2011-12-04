// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePackageEntry_H__
#define __WmePackageEntry_H__

#include "Object.h"
#include "Package.h"

namespace Wme
{
	class WmeDllExport PackageEntry : public Object
	{
	public:
		PackageEntry(Package* package, const WideString& fileName, dword offset, dword size, dword compressedSize, bool isCompressed, time_t time);
		virtual ~PackageEntry();

		Package* GetPackage() { return m_Package; }
		const WideString& GetFileName() const { return m_FileName; }
		dword GetSize() const { return m_Size; }
		dword GetCompressedSize() const { return m_CompressedSize; }
		dword GetOffset() const { return m_Offset; }
		bool IsCompressed() const { return m_IsCompressed; }
		time_t GetTime() const { return m_Time; }

	private:
		Package* m_Package;
		WideString m_FileName;
		dword m_Size;
		dword m_CompressedSize;
		dword m_Offset;
		bool m_IsCompressed;
		time_t m_Time;
	};
}


#endif // __WmePackageEntry_H__
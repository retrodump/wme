// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeWmePackage_H__
#define __WmeWmePackage_H__

namespace Wme
{

#define WME_PACKAGE_MAGIC_1	0x32454D57 // WME2
#define WME_PACKAGE_MAGIC_2	0x474b4350 // PCKG
#define WME_PACKAGE_VERSION 0x0001

#define WME_PACKAGE_EXTENSION L"wme"

#ifdef _MSC_VER
#	pragma pack (push, 1)
#else
#	pragma pack (1)
#endif

	typedef struct
	{
		dword Magic1;
		dword Magic2;
		dword Version;
		dword Priority;
		word CreationYear;
		byte CreationMonth;
		byte CreationDay;
		byte CreationHour;
		byte CreationMinute;
		byte CreationSecond;
		byte Padding1;
		dword Reserved1;
		dword Reserved2;
	} PackageHeader;

	typedef struct 
	{
		dword Offset;
		dword OriginalSize;
		dword CompressedSize;
		byte IsCompressed;
		dword Reserved1;
		dword Reserved2;
		word ModYear;
		byte ModMonth;
		byte ModDay;
		byte ModHour;
		byte ModMinute;
		byte ModSecond;
	} FileEntryHeader;

#ifdef _MSC_VER
#	pragma pack (pop)
#else
#	pragma pack ()
#endif

}

#endif // __WmeWmePackage_H__
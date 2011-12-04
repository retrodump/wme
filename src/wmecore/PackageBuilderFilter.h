// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePackageBuilderFilter_H__
#define __WmePackageBuilderFilter_H__

#include "Object.h"

namespace Wme
{
	typedef struct  
	{
		WideString OrigFileName;
		WideString NewFileName;
		byte* Buffer;
		size_t BufferSize;
		size_t OrigSize;
		bool IsCompressed;
		
		enum ProcessingType
		{
			FILTER_SKIP, FILTER_USE_BUFFER
		} Processing;
	} PackageFilterInfo;


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport PackageBuilderFilter : public Object
	{
	public:
		PackageBuilderFilter();
		virtual ~PackageBuilderFilter() {};

		virtual void Initialize();
		virtual bool ProcessFile(PackageFilterInfo& info) = 0;
		virtual WideString GetDefaultMasks() { return L""; }
		void SetFileMasks(const WideString& masks);
		bool MatchesFile(const WideString& fileName);

	protected:
		static bool CompressBuffer(PackageFilterInfo& info);
		static bool ReadFileToBuffer(const WideString& fileName, byte*& buffer, size_t& bufferSize);
		WideStringListPtr m_FileMasks;
	};

	//////////////////////////////////////////////////////////////////////////
	class PackageBuilderFilterUncompressed : public PackageBuilderFilter
	{
	public:
		PackageBuilderFilterUncompressed() {};
		virtual bool ProcessFile(PackageFilterInfo& info);
		virtual WideString GetDefaultMasks() { return L"*.png;*.jpg;*.ogg;*.ogv;*.oga"; }
	};

	//////////////////////////////////////////////////////////////////////////
	class PackageBuilderFilterIgnore : public PackageBuilderFilter
	{
	public:
		virtual bool ProcessFile(PackageFilterInfo& info);
		virtual WideString GetDefaultMasks() { return L"*.bak;*.inc;*.tmp;*/.svn/*"; }
	};

	//////////////////////////////////////////////////////////////////////////
	class PackageBuilderFilterScript : public PackageBuilderFilter
	{
	public:
		virtual bool ProcessFile(PackageFilterInfo& info);
		virtual WideString GetDefaultMasks() { return L"*.script"; }
	};

	//////////////////////////////////////////////////////////////////////////
	class PackageBuilderFilterDefault : public PackageBuilderFilter
	{
	public:
		virtual bool ProcessFile(PackageFilterInfo& info);
		virtual WideString GetDefaultMasks() { return L"*.*"; }
	};

}


#endif // __WmePackageBuilderFilter_H__
// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PackageBuilderFilter.h"
#include "Game.h"
#include "ScriptManager.h"
#include "DynamicBuffer.h"
#include "StringUtil.h"
#include "PathUtil.h"
#include <iostream>
#include <zlib.h>


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
PackageBuilderFilter::PackageBuilderFilter()
{
	m_FileMasks = WideStringListPtr(new WideStringList());
}

//////////////////////////////////////////////////////////////////////////
void PackageBuilderFilter::Initialize()
{
	SetFileMasks(GetDefaultMasks());
}

//////////////////////////////////////////////////////////////////////////
void PackageBuilderFilter::SetFileMasks(const WideString& masks)
{
	m_FileMasks = StringUtil::Split(masks, L";,");
}

//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilter::MatchesFile(const WideString& fileName)
{
	WideStringList::const_iterator it;

	for (it = m_FileMasks->begin(); it != m_FileMasks->end(); it++)
	{
		//WideString shortFileName = PathUtil::GetFileName(fileName);
		if (PathUtil::MatchesMask(fileName, *it)) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilter::ReadFileToBuffer(const WideString& fileName, byte*& buffer, size_t& bufferSize)
{
	std::ifstream stream;
#ifdef WIN32
	stream.open(fileName.c_str(), std::ios::binary);
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str(), std::ios::binary);
#endif
	if (!stream.is_open()) return false;

	stream.seekg(0, std::ios_base::end);
	bufferSize = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	if (bufferSize > 0)	buffer = new byte[bufferSize];
	else
	{
		buffer = NULL;
		stream.close();
		return true;
	}

	stream.read(reinterpret_cast<char*>(buffer), bufferSize);
	stream.close();
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilter::CompressBuffer(PackageFilterInfo& info)
{
	dword compSize = compressBound(info.BufferSize);
	byte* compBuffer = new byte[compSize];

	if (compress(compBuffer, &compSize, info.Buffer, info.BufferSize) != Z_OK || compSize > info.BufferSize)
	{
		delete [] compBuffer;
		return false;
	}

	delete [] info.Buffer;
	
	info.OrigSize = info.BufferSize;
	info.Buffer = compBuffer;
	info.BufferSize = compSize;
	info.IsCompressed = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilterUncompressed::ProcessFile(PackageFilterInfo& info)
{
	if (!ReadFileToBuffer(info.OrigFileName, info.Buffer, info.BufferSize)) return false;

	info.NewFileName = info.OrigFileName;
	info.OrigSize = info.BufferSize;
	info.IsCompressed = false;
	info.Processing = PackageFilterInfo::FILTER_USE_BUFFER;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilterIgnore::ProcessFile(PackageFilterInfo& info)
{
	info.Processing = PackageFilterInfo::FILTER_SKIP;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilterScript::ProcessFile(PackageFilterInfo& info)
{
	DynamicBuffer* code = Game::GetInstance()->GetScriptMgr()->GetCompiledScript(info.OrigFileName);
	if (!code) return false;

	info.NewFileName = info.OrigFileName;
	info.BufferSize = code->GetSize();
	info.Buffer = new byte[code->GetSize()];
	memcpy(info.Buffer, code->GetData(), code->GetSize());
	info.Processing = PackageFilterInfo::FILTER_USE_BUFFER;	
	delete code;

	CompressBuffer(info);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PackageBuilderFilterDefault::ProcessFile(PackageFilterInfo& info)
{
	if (!ReadFileToBuffer(info.OrigFileName, info.Buffer, info.BufferSize)) return false;

	info.NewFileName = info.OrigFileName;
	info.OrigSize = info.BufferSize;
	info.IsCompressed = false;
	info.Processing = PackageFilterInfo::FILTER_USE_BUFFER;
	
	CompressBuffer(info);
	return true;
}


} // namespace Wme

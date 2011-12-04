// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TheoraFileSystemBridge.h"
#include "File.h"
#include "FileManager.h"
#include "Game.h"
#include "StringUtil.h"
#include "Exception.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TheoraFileSystemBridge::TheoraFileSystemBridge(const WideString& fileName)
{
	m_File = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!m_File) throw Exception(L"Error opening video file '" + fileName + L"'");
}

//////////////////////////////////////////////////////////////////////////
TheoraFileSystemBridge::~TheoraFileSystemBridge()
{
	Game::GetInstance()->GetFileMgr()->CloseFile(m_File);
}

//////////////////////////////////////////////////////////////////////////
int TheoraFileSystemBridge::read(void* output, int nBytes)
{
	return m_File->Read(output, nBytes);
}

//////////////////////////////////////////////////////////////////////////
void TheoraFileSystemBridge::seek(unsigned long byte_index)
{
	m_File->Seek(byte_index, File::SEEK_FROM_BEGIN);
}

//////////////////////////////////////////////////////////////////////////
std::string TheoraFileSystemBridge::repr()
{
	return StringUtil::WideToUtf8(m_File->GetFileName());
}

//////////////////////////////////////////////////////////////////////////
unsigned long TheoraFileSystemBridge::size()
{
	return m_File->GetSize();
}

//////////////////////////////////////////////////////////////////////////
unsigned long TheoraFileSystemBridge::tell()
{
	return m_File->GetPos();
}


} // namespace Wme

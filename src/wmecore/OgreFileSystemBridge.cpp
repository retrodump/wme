// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "OgreFileSystemBridge.h"
#include "Game.h"
#include "FileManager.h"
#include "File.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
WmeArchive::WmeArchive(const Ogre::String& name, const Ogre::String& archType) : Ogre::Archive(name, archType)
{

}

//////////////////////////////////////////////////////////////////////////
WmeArchive::~WmeArchive()
{
	unload();
}

//////////////////////////////////////////////////////////////////////////
void WmeArchive::load()
{

}

//////////////////////////////////////////////////////////////////////////
void WmeArchive::unload()
{

}

//////////////////////////////////////////////////////////////////////////
Ogre::DataStreamPtr WmeArchive::open(const Ogre::String& filename, bool readOnly) const
{
	File* file = Game::GetInstance()->GetFileMgr()->OpenFile(StringUtil::Utf8ToWide(filename));
	if (!file) return Ogre::DataStreamPtr();
	else return Ogre::DataStreamPtr(new WmeDataStream(file));
}

//////////////////////////////////////////////////////////////////////////
Ogre::StringVectorPtr WmeArchive::list(bool recursive, bool dirs)
{
	return Ogre::StringVectorPtr(new Ogre::StringVector());
}

//////////////////////////////////////////////////////////////////////////
Ogre::FileInfoListPtr WmeArchive::listFileInfo(bool recursive, bool dirs)
{
	return Ogre::FileInfoListPtr(new Ogre::FileInfoList());
}

//////////////////////////////////////////////////////////////////////////
Ogre::StringVectorPtr WmeArchive::find(const Ogre::String& pattern, bool recursive, bool dirs)
{
	return Ogre::StringVectorPtr(new Ogre::StringVector());
}

//////////////////////////////////////////////////////////////////////////
Ogre::FileInfoListPtr WmeArchive::findFileInfo(const Ogre::String& pattern, bool recursive, bool dirs)
{
	return Ogre::FileInfoListPtr(new Ogre::FileInfoList());
}

//////////////////////////////////////////////////////////////////////////
bool WmeArchive::exists(const Ogre::String& filename)
{
	return Game::GetInstance()->GetFileMgr()->FileExists(StringUtil::Utf8ToWide(filename));
}

//////////////////////////////////////////////////////////////////////////
time_t WmeArchive::getModifiedTime(const Ogre::String& filename)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
WmeDataStream::WmeDataStream(File* file)
{
	m_File = file;
	mSize = file->GetSize();
}

//////////////////////////////////////////////////////////////////////////
WmeDataStream::~WmeDataStream()
{
	close();
}

//////////////////////////////////////////////////////////////////////////
size_t WmeDataStream::read(void* buf, size_t count)
{
	if (!m_File) return 0;
	
	return m_File->Read(buf, count);
}

//////////////////////////////////////////////////////////////////////////
void WmeDataStream::skip(long count)
{
	if (m_File)
	{
		dword pos = (dword)m_File->GetPos();
		pos += count;

		m_File->Seek(pos, File::SEEK_FROM_BEGIN);
	}
}

//////////////////////////////////////////////////////////////////////////
void WmeDataStream::seek(size_t pos)
{
	if (m_File) m_File->Seek(pos, File::SEEK_FROM_BEGIN);
}

//////////////////////////////////////////////////////////////////////////
size_t WmeDataStream::tell(void) const
{
	if (!m_File) return 0;
	return m_File->GetPos();
}

//////////////////////////////////////////////////////////////////////////
bool WmeDataStream::eof(void) const
{
	if (!m_File) return true;
	return m_File->IsEOF();
}

//////////////////////////////////////////////////////////////////////////
void WmeDataStream::close(void)
{
	Game::GetInstance()->GetFileMgr()->CloseFile(m_File);
}


} // namespace Wme

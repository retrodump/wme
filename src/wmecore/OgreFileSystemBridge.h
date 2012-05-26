// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeOgreFileSystem_H__
#define __WmeOgreFileSystem_H__

#include <OgreArchive.h>
#include <OgreArchiveFactory.h>
#include <OgreDataStream.h>

namespace Wme
{
	class File;


	//////////////////////////////////////////////////////////////////////////
	// Archive
	class WmeDllExport WmeArchive : public Ogre::Archive
	{
	public:
		WmeArchive(const Ogre::String& name, const Ogre::String& archType);
		~WmeArchive();

		bool isCaseSensitive(void) const { return false; }
		void load();
		void unload();
		Ogre::DataStreamPtr open(const Ogre::String& filename, bool readOnly = true) const;
		Ogre::StringVectorPtr list(bool recursive = true, bool dirs = false);
		Ogre::FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);
		Ogre::StringVectorPtr find(const Ogre::String& pattern, bool recursive = true, bool dirs = false);
		Ogre::FileInfoListPtr findFileInfo(const Ogre::String& pattern, bool recursive = true, bool dirs = false) const;
		bool exists(const Ogre::String& filename);
		time_t getModifiedTime(const Ogre::String& filename);
	};


	//////////////////////////////////////////////////////////////////////////
	// ArchiveFactory
	class WmeDllExport WmeArchiveFactory : public Ogre::ArchiveFactory
	{
	public:
		virtual ~WmeArchiveFactory() {}

		const Ogre::String& getType(void) const
		{
			static Ogre::String name = "Wme";
			return name;
		}

		Ogre::Archive* createInstance(const Ogre::String& name) 
		{
			return OGRE_NEW WmeArchive(name, "Wme");
		}

		void destroyInstance(Ogre::Archive* arch) { OGRE_DELETE arch; }
	};


	//////////////////////////////////////////////////////////////////////////
	// DataStream
	class WmeDllExport WmeDataStream : public Ogre::DataStream
	{
	public:
		WmeDataStream(File* file);
		~WmeDataStream();

		size_t read(void* buf, size_t count);
		void skip(long count);
		void seek(size_t pos);
		size_t tell(void) const;
		bool eof(void) const;
		void close(void);
	private:
		File* m_File;
	};


}


#endif
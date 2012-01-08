// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTypes_H__
#define __WmeTypes_H__

#include <string>
#include <list>
#include <set>
#include <boost/smart_ptr.hpp>


namespace Wme
{
	typedef std::string AnsiString;
	typedef std::string Utf8String;
	typedef std::wstring WideString;

	typedef unsigned char byte;
	typedef unsigned long dword;
	typedef unsigned short word;

	typedef std::list<WideString> WideStringList;
	typedef boost::shared_ptr<WideStringList> WideStringListPtr;

	typedef std::list<AnsiString> AnsiStringList;
	typedef boost::shared_ptr<AnsiStringList> AnsiStringListPtr;

	typedef std::set<WideString> WideStringSet;

	typedef std::vector<Ogre::Vector3> PointVector;

	typedef struct
	{
		Ogre::Vector2 pos;
		Ogre::ColourValue color;
	} Vertex2D;

	typedef struct
	{
		Ogre::Vector2 pos;
		Ogre::ColourValue color;
		Ogre::Vector2 texCoord;
	} Vertex2DTex;


	typedef struct
	{
		Ogre::Vector3 pos;
		Ogre::RGBA color;
		Ogre::Vector2 texCoord;
	} SpriteVertex;

}
#endif // __WmeTypes_H__

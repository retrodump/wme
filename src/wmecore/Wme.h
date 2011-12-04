// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __Wme_H__
#define __Wme_H__


#if defined(WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#include "Ogre.h"
#include "WmeTypes.h"
#include <boost/foreach.hpp>

#define TIXML_USE_STL
#include "tinyxml.h"

#include <ft2build.h>
#include FT_FREETYPE_H


// version
#define WME_VER_MAJOR 2
#define WME_VER_MINOR 0
#define WME_VER_PATCH 0
#define WME_VER_SUFFIX "wip"
#define WME_VER_BETA true


// dll export
#ifdef WME_DLL_BUILD
#	define WmeDllExport __declspec(dllexport)
#else
#	define WmeDllExport
#endif


// compilation switches
#ifdef _DEBUG
	// dump assembly of compiled scripts to file
#	define WME_SCRIPT_COMPILER_DEBUGGING
	// check write barriers
#	define WME_GC_WRITE_BARRIER_CHECKING

#endif




// WinAPI fixes
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif


// convenience macros
#define SAFE_DELETE(obj)  if (obj) { delete obj; obj = NULL; } else 0
#define SAFE_DELETE_ARRAY(obj)  if (obj) { delete [] obj; obj = NULL; } else 0

#define RTTI(name) \
	virtual const WideString GetClassID() const \
	{ return L ## #name; }


#ifdef foreach
#	undef foreach
#endif

#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH



#endif // __Wme_H__

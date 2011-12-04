// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMaterialUtil_H__
#define __WmeMaterialUtil_H__


namespace Wme
{
	class WmeDllExport MaterialUtil
	{
	public:
		static const Utf8String HIGHLIGHT_MAT_PREFIX;

		static bool IsHighlightedName(const Utf8String& matName);
		static Utf8String GetHighlightedName(const Utf8String& origName);
		static Utf8String GetUnhighlightedName(const Utf8String& origName);

		static Ogre::MaterialPtr GetHighlightMat(const Ogre::MaterialPtr origMat, bool forceRebuild = false);
		static Ogre::MaterialPtr GetEmptyMat();

	};
}

#endif // __WmeMaterialUtil_H__
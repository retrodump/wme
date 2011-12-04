// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFontManager_H__
#define __WmeFontManager_H__

#include "Object.h"

namespace Wme
{
	class Font;

	class WmeDllExport FontManager : public Object
	{
	public:
		FontManager();
		virtual ~FontManager();

		bool Initialize();

		Font* GetFont(const WideString& fontFileName, bool useFallback);
		Font* GetFallbackFont();
		void ReleaseFont(Font*& font);

		FT_Library GetFTLibrary() { return m_FTLibrary; }

	private:		
		FT_Library  m_FTLibrary;

		Font* m_FallbackFont;		

		typedef std::map<WideString, Font*> FontMap;
		FontMap m_Fonts;
	};
}


#endif // __WmeFontManager_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FontManager.h"
#include "Game.h"
#include "Font.h"
#include "PathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
FontManager::FontManager()
{
	m_FTLibrary = NULL;
	m_FallbackFont = NULL;
}

//////////////////////////////////////////////////////////////////////////
FontManager::~FontManager()
{
	foreach (FontMap::value_type val, m_Fonts)
	{
		SAFE_DELETE(val.second);
	}
	m_Fonts.clear();


	SAFE_DELETE(m_FallbackFont);
	if (m_FTLibrary) FT_Done_FreeType(m_FTLibrary);
}

//////////////////////////////////////////////////////////////////////////
bool FontManager::Initialize()
{
	FT_Error error = FT_Init_FreeType(&m_FTLibrary);
	if (error)
	{
		Game::GetInstance()->Log(L"Error initializing FreeType library.");
		return false;
	}
	else return true;
}

//////////////////////////////////////////////////////////////////////////
Font* FontManager::GetFont(const WideString& fontFileName, bool useFallback)
{
	WideString safeFileName = PathUtil::NormalizeFileName(fontFileName);

	FontMap::iterator it;
	it = m_Fonts.find(safeFileName);

	if (it != m_Fonts.end())
	{
		(*it).second->AddRef();
		return (*it).second;
	}
	else
	{
		Font* font = new Font();
		if (font->LoadFromFile(safeFileName))
		{
			m_Fonts[safeFileName] = font;
			font->AddRef();
			return font;
		}
		else
		{
			delete font;
			return useFallback ? GetFallbackFont() : NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void FontManager::ReleaseFont(Font*& font)
{
	if (!font) return;
	if (font == GetFallbackFont()) return;	

	FontMap::iterator it;
	it = m_Fonts.find(PathUtil::NormalizeFileName(font->GetFileName()));

	if (it != m_Fonts.end())
	{
		if (!it->second->ReleaseRef()) m_Fonts.erase(it);
	}

	font = NULL;
}

//////////////////////////////////////////////////////////////////////////
Font* FontManager::GetFallbackFont()
{
	if (!m_FallbackFont)
	{
		m_FallbackFont = new Font();
		if (!m_FallbackFont->LoadFace(L"Arial.ttf", 12))
		{
			SAFE_DELETE(m_FallbackFont);
		}
	}

	return m_FallbackFont;
}


} // namespace Wme

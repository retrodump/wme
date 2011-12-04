// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFontGlyphCache_H__
#define __WmeFontGlyphCache_H__

#include "Object.h"

namespace Wme
{
	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport GlyphInfo
	{
	public:
		GlyphInfo(int glyphIndex)
		{
			m_GlyphIndex = glyphIndex;
			m_AdvanceX = m_AdvanceY = 0;
			m_BearingX = m_BearingY = 0;
			m_PageIndex = 0;

			m_Width = m_Height = 0;
		}

		void SetGlyphInfo(float AdvanceX, float AdvanceY, int BearingX, int BearingY)
		{
			m_AdvanceX = AdvanceX;
			m_AdvanceY = AdvanceY;
			m_BearingX = BearingX;
			m_BearingY = BearingY;
		}

		void SetTexturePos(int left, int top, int right, int bottom, size_t textureWidth, int pageIndex)
		{
			m_Width = right - left;
			m_Height = bottom - top;

			m_TextureRect = Ogre::FloatRect((float)left / (float)textureWidth, (float)top / (float)textureWidth, (float)right / (float)textureWidth, (float)bottom / (float)textureWidth);
			m_PageIndex = pageIndex;
		}

		int GetGlyphIndex() { return m_GlyphIndex; }
		int GetPageIndex() { return m_PageIndex; }
		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
		float GetAdvanceX() { return m_AdvanceX; }
		float GetAdvanceY() { return m_AdvanceY; }
		int GetBearingX() { return m_BearingX; }
		int GetBearingY() { return m_BearingY; }

		Ogre::FloatRect GetTextureRect() { return m_TextureRect; }
	private:
		int m_GlyphIndex;
		Ogre::FloatRect m_TextureRect;
		int m_PageIndex;
		float m_AdvanceX;
		float m_AdvanceY;
		int m_BearingX;
		int m_BearingY;

		int m_Width;
		int m_Height;
	};



	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport FontGlyphCache :	public Object
	{
	public:
		FontGlyphCache();
		virtual ~FontGlyphCache();

		void Initialize(size_t gridWidth, size_t gridHeight);
		bool HasGlyph(wchar_t ch);
		GlyphInfo* GetGlyph(wchar_t ch);
		void AddGlyph(wchar_t ch, int glyphIndex, FT_GlyphSlot glyphSlot, size_t width, size_t height, byte* pixels, size_t stride = 0);
		Ogre::MaterialPtr& GetMaterial(int pageIndex);

		int GetNumPages() { return m_Pages.size(); }

		void RebuildTextures();
		void DebugSave();


	private:
		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport GlyphImage
		{
		public:
			GlyphImage(size_t width, size_t height);
			virtual ~GlyphImage();

			Ogre::PixelBox* GetPixelBox() { return m_PixelBox; }
			Ogre::TexturePtr GetTexture();
			Ogre::MaterialPtr& GetMaterial();
			void RebuildTexture();
			void InsertGlyph(size_t posX, size_t posY, size_t width, size_t height, byte* pixels, size_t stride);
			void SaveToFile(Ogre::String fileName);

		private:
			Ogre::PixelBox* m_PixelBox;
			Ogre::TexturePtr m_Texture;
			Ogre::MaterialPtr m_Material;
			dword* m_PixelData;
			bool m_IsDirty;
		};

		typedef std::vector<GlyphImage*> GlyphImageVector;
		GlyphImageVector m_Pages;

		typedef std::map<wchar_t, GlyphInfo*> GlyphInfoMap;
		GlyphInfoMap m_Glyphs;

		size_t m_GridWidth;
		size_t m_GridHeight;
		size_t m_TextureWidth;
		int m_CurrentPage;
		int m_CurrentPosX;
		int m_CurrentPosY;
		int m_CurrentRowHeight;


		static int s_TextureCounter;
	};
}

#endif // __WmeFontGlyphCache_H__
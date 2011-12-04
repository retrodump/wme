// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFont_H__
#define __WmeFont_H__

#include "Object.h"
#include "DocumentAwareObject.h"
#include "RefCountObject.h"

namespace Wme
{
	class FontGlyphCache;


	class WmeDllExport Font : public Object, public DocumentAwareObject, public RefCountObject
	{
	public:

		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport TextLine
		{
		public:
			TextLine(const WideString& text, int width)
			{
				m_Text = text;
				m_Width = width;
			}

			const WideString& GetText() const { return m_Text; }
			int GetWidth() const { return m_Width; }
		private:
			WideString m_Text;
			int m_Width;
		};
		typedef std::list<TextLine*> TextLineList;



		Font();
		virtual ~Font();

		bool LoadFace(const WideString& fileName, float pointSize, float vertDpi = 96.0, float horDpi = 96.0, bool antiAlias = true);
		void PrepareGlyphs(const WideString& text);

		float GetKerning(wchar_t leftChar, wchar_t rightChar);

		FontGlyphCache* GetGlyphCache() { return m_GlyphCache; }
		float GetLineHeight() { return m_LineHeight; }
		float GetAscender() { return m_Ascender; }
		float GetDescender() { return m_Descender; }

		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeFont"; }

		const WideString& GetFileName() { return m_FileName; }

		void WrapText(const WideString& text, int maxWidth, int maxHeight, TextLineList& lines);
		void MeasureText(const WideString& text, int maxWidth, int maxHeight, int& textWidth, int& textHeight);

	private:
		void Cleanup();
		void CacheGlyph(wchar_t ch);

		FontGlyphCache* m_GlyphCache;
		FT_Face m_FTFace;

		// see glyph metrics: http://www.freetype.org/freetype2/docs/tutorial/step2.html
		float m_Ascender;
		float m_Descender;
		float m_LineHeight;
		float m_PointSize;
		float m_VertDpi;
		float m_HorDpi;
		WideString m_FaceFileName;
		bool m_AntiAlias;
		FT_Stream m_FTStream;

		static WideString GetSystemFontPath();


		// I/O bridge
		static unsigned long FTReadSeekProc(FT_Stream stream, unsigned long offset,	unsigned char* buffer, unsigned long count);
		static void FTCloseProc(FT_Stream stream);

	};
}

#endif // __WmeFont_H__

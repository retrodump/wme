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

		float GetKerning(wchar_t leftChar, wchar_t rightChar) const;

		FontGlyphCache* GetGlyphCache() const { return m_GlyphCache; }
		float GetLineHeight() const { return m_LineHeight; }
		float GetAscender() const { return m_Ascender; }
		float GetDescender() const { return m_Descender; }

		float GetUnderlinePosition() const { return m_UnderlinePosition; }
		float GetUnderlineThickness() const { return m_UnderlineThickness; }
		float GetStrikethroughPosition() const { return GetDescender() * 2.0f; }

		bool GetUnderline() const { return m_Underline; }
		void SetUnderline(bool val) { m_Underline = val; }

		bool GetStrikethrough() const { return m_Strikethrough; }
		void SetStrikethrough(bool val) { m_Strikethrough = val; }

		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeFont"; }

		const WideString& GetFileName() { return m_FileName; }

		void WrapText(const WideString& text, int maxWidth, int maxHeight, int leadingSpace, TextLineList& lines);
		void MeasureText(const WideString& text, int maxWidth, int maxHeight, int leadingSpace, int& textWidth, int& textHeight);

	private:
		void Cleanup();
		void CacheGlyph(wchar_t ch);

		bool m_Underline;
		bool m_Strikethrough;

		FontGlyphCache* m_GlyphCache;
		FT_Face m_FTFace;

		// see glyph metrics: http://www.freetype.org/freetype2/docs/tutorial/step2.html
		float m_Ascender;
		float m_Descender;
		float m_LineHeight;
		float m_PointSize;
		float m_VertDpi;
		float m_HorDpi;
		float m_UnderlinePosition;
		float m_UnderlineThickness;
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

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Font.h"
#include "FontGlyphCache.h"
#include "Game.h"
#include "FontManager.h"
#include "StringUtil.h"
#include "MathUtil.h"
#include "PathUtil.h"
#include "FileManager.h"
#include "File.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Font::Font()
{
	m_GlyphCache = NULL;
	m_FTFace = NULL;
	m_FTStream = NULL;

	m_Underline = false;
	m_Strikethrough = false;

	m_Ascender = m_Descender = m_LineHeight = m_PointSize = 0;
	m_HorDpi = m_VertDpi = 0;
	m_UnderlinePosition = m_UnderlineThickness = 0;
	m_AntiAlias = true;
}

//////////////////////////////////////////////////////////////////////////
Font::~Font()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
void Font::Cleanup()
{
	SAFE_DELETE(m_GlyphCache);

	if (m_FTFace)
	{
		FT_Done_Face(m_FTFace);
		m_FTFace = NULL;
	}
	SAFE_DELETE_ARRAY(m_FTStream);
}

//////////////////////////////////////////////////////////////////////////
bool Font::LoadFace(const WideString& fileName, float pointSize, float vertDpi, float horDpi, bool antiAlias)
{
	Cleanup();

	FT_Error error;

	WideString fontFileName = fileName;

	File* file = Game::GetInstance()->GetFileMgr()->OpenFile(fontFileName);
	if (!file)
	{
		// the requested font file is not in wme file space; try loading a system font
		fontFileName = PathUtil::Combine(GetSystemFontPath(), PathUtil::GetFileName(fontFileName));
		file = Game::GetInstance()->GetFileMgr()->OpenFile(fontFileName);

		if (!file) return false;
	}


	// use FT stream interface so that we don't have to keep the whole font file in memory
	// (those Unicode fonts can be pretty big)
	m_FTStream = (FT_Stream)new byte[sizeof(*m_FTStream)];
	memset(m_FTStream, 0, sizeof(*m_FTStream));

	m_FTStream->read = Font::FTReadSeekProc;
	m_FTStream->close = Font::FTCloseProc;
	m_FTStream->descriptor.pointer = file;
	m_FTStream->size = file->GetSize();

	FT_Open_Args args;
	args.flags = FT_OPEN_STREAM;
	args.stream = m_FTStream;

	error = FT_Open_Face(Game::GetInstance()->GetFontMgr()->GetFTLibrary(), &args, 0, &m_FTFace);
	if (error)
	{
		SAFE_DELETE_ARRAY(m_FTStream);
		Game::GetInstance()->GetFileMgr()->CloseFile(file);
		return false;
	}

	error = FT_Set_Char_Size(m_FTFace, 0, (FT_F26Dot6)(pointSize * 64), (FT_UInt)horDpi, (FT_UInt)vertDpi);
	if (error)
	{
		FT_Done_Face(m_FTFace);
		m_FTFace = NULL;
		return false;
	}

	// http://en.wikipedia.org/wiki/Em_(typography)
	float pixelsPerEm = (pointSize / 72.f) * vertDpi; // Size in inches * dpi
	float EmsPerUnit = 1.0f / m_FTFace->units_per_EM;
	float pixelsPerUnit = pixelsPerEm * EmsPerUnit;

	// bounding box in pixels
	float xMin = m_FTFace->bbox.xMin * pixelsPerUnit;
	float xMax = m_FTFace->bbox.xMax * pixelsPerUnit;
	float yMin = m_FTFace->bbox.yMin * pixelsPerUnit;
	float yMax = m_FTFace->bbox.yMax * pixelsPerUnit;
	
	// metrics in pixels
	m_Ascender = m_FTFace->ascender * pixelsPerUnit;
	m_Descender = - m_FTFace->descender * pixelsPerUnit;
	m_LineHeight = m_FTFace->height * pixelsPerUnit;
	m_UnderlinePosition = m_FTFace->underline_position * pixelsPerUnit;
	m_UnderlineThickness = m_FTFace->underline_thickness * pixelsPerUnit;

	// max character size (used for texture grid)
	size_t maxCharWidth  = (size_t)MathUtil::RoundUp(xMax - xMin);
	size_t maxCharHeight = (size_t)MathUtil::RoundUp(yMax - yMin);

	m_GlyphCache = new FontGlyphCache();
	m_GlyphCache->Initialize(maxCharWidth, maxCharHeight);

	m_PointSize = pointSize;
	m_FaceFileName = fileName;
	m_VertDpi = vertDpi;
	m_HorDpi = horDpi;
	m_AntiAlias = antiAlias;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Font::PrepareGlyphs(const WideString& text)
{
	// make sure we have all the glyphs we need
	foreach (wchar_t ch, text)
	{
		if (!m_GlyphCache->HasGlyph(ch)) CacheGlyph(ch);
	}
	m_GlyphCache->RebuildTextures();
	//m_GlyphCache->DebugSave();
}

//////////////////////////////////////////////////////////////////////////
void Font::CacheGlyph(wchar_t ch)
{
	FT_UInt glyphIndex = FT_Get_Char_Index(m_FTFace, ch);
	if (!glyphIndex) return;

	FT_Error error = FT_Load_Glyph(m_FTFace, glyphIndex, FT_LOAD_DEFAULT);
	if (error) return;

	error = FT_Render_Glyph(m_FTFace->glyph, m_AntiAlias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO);
	if (error) return;

	byte* pixels = m_FTFace->glyph->bitmap.buffer;
	size_t stride = m_FTFace->glyph->bitmap.pitch;


	// convert from monochrome to grayscale if needed
	byte* tempBuffer = NULL;
	if (pixels != NULL && m_FTFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		tempBuffer = new byte[m_FTFace->glyph->bitmap.width * m_FTFace->glyph->bitmap.rows];
		for (int j = 0; j < m_FTFace->glyph->bitmap.rows; j++)
		{
			int rowOffset = stride * j;
			for (int i = 0; i < m_FTFace->glyph->bitmap.width; i++)
			{
				int byteOffset = i / 8;
				int bitOffset = 7 - (i % 8);
				byte bit = (pixels[rowOffset + byteOffset] & (1 << bitOffset)) >> bitOffset;
				tempBuffer[m_FTFace->glyph->bitmap.width * j + i] =	255 * bit;
			}
		}

		pixels = tempBuffer;
		stride = m_FTFace->glyph->bitmap.width;
	}

	// add glyph to cache
	m_GlyphCache->AddGlyph(ch, glyphIndex, m_FTFace->glyph, m_FTFace->glyph->bitmap.width, m_FTFace->glyph->bitmap.rows, pixels, stride);

	if (tempBuffer) delete [] tempBuffer;
}

//////////////////////////////////////////////////////////////////////////
float Font::GetKerning(wchar_t leftChar, wchar_t rightChar) const
{
	GlyphInfo* infoLeft = m_GlyphCache->GetGlyph(leftChar);
	GlyphInfo* infoRight = m_GlyphCache->GetGlyph(rightChar);

	if (!infoLeft || !infoRight) return 0;

	FT_Vector delta;
	FT_Error error = FT_Get_Kerning(m_FTFace, infoLeft->GetGlyphIndex(), infoRight->GetGlyphIndex(), ft_kerning_unfitted, &delta);
	if (error) return 0;

	return delta.x * (1.0f / 64.0f);
}

//////////////////////////////////////////////////////////////////////////
WideString Font::GetSystemFontPath()
{
#ifdef _WIN32
	// we're looking for something like "c:\windows\fonts\";
	// in theory this might stop working in future Win versions but MS usually at least provides
	// a junction when they move some directory...
	wchar_t winDir[MAX_PATH + 1];
	winDir[MAX_PATH] = L'\0';
	::GetWindowsDirectoryW(winDir, MAX_PATH);
	return PathUtil::Combine(WideString(winDir), L"fonts");
#else
	// !PORTME
	return L"/Library/Fonts/";
#endif
}

//////////////////////////////////////////////////////////////////////////
bool Font::LoadFromXml(TiXmlElement* rootNode)
{
	WideString faceName = L"";
	float pointSize = 10.0;
	float vertDPI = 96.0;
	float horDPI = 96.0;
	bool antiAlias = true;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Face")
		{
			faceName = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "PointSize")
		{
			pointSize = XmlUtil::TextToFloat(elem, pointSize);
		}
		else if (elem->ValueStr() == "DPI")
		{
			vertDPI = XmlUtil::AttrToFloat(elem, "Vertical", vertDPI);
			horDPI = XmlUtil::AttrToFloat(elem, "Horizontal", horDPI);
		}
		else if (elem->ValueStr() == "Antialias")
		{
			antiAlias = XmlUtil::TextToBool(elem, antiAlias);
		}
		else if (elem->ValueStr() == "Underline")
		{
			m_Underline = XmlUtil::TextToBool(elem);
		}
		else if (elem->ValueStr() == "Strikethrough")
		{
			m_Strikethrough = XmlUtil::TextToBool(elem);
		}
	}


	if (faceName.empty() || pointSize <= 0)
	{
		Game::GetInstance()->Log(L"Incomplete font definition.");
		return false;
	}

	return LoadFace(faceName, pointSize, vertDPI, horDPI, antiAlias);
}


//////////////////////////////////////////////////////////////////////////
bool Font::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Face", rootNode);
	XmlUtil::SetFileName(elem, m_FaceFileName);

	elem = XmlUtil::AddElem("PointSize", rootNode);
	XmlUtil::SetText(elem, m_PointSize);

	elem = XmlUtil::AddElem("DPI", rootNode);
	XmlUtil::SetAttr(elem, "Vertical", m_VertDpi);
	XmlUtil::SetAttr(elem, "Horizontal", m_HorDpi);

	elem = XmlUtil::AddElem("Antialias", rootNode);
	XmlUtil::SetText(elem, m_AntiAlias);

	elem = XmlUtil::AddElem("Underline", rootNode);
	XmlUtil::SetText(elem, m_Underline);

	elem = XmlUtil::AddElem("Strikethrough", rootNode);
	XmlUtil::SetText(elem, m_Strikethrough);

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Font::WrapText(const WideString& text, int maxWidth, int maxHeight, int leadingSpace, TextLineList& lines)
{
	int currWidth = leadingSpace;
	wchar_t prevChar = L'\0';
	int prevSpaceIndex = -1;
	int prevSpaceWidth = 0;
	int lineStartIndex = 0;

	PrepareGlyphs(text);

	for (size_t i = 0; i < text.length(); i++)
	{
		wchar_t ch = text[i];

		if (ch == L' ')
		{
			prevSpaceIndex = i;
			prevSpaceWidth = currWidth;
		}

		int charWidth = 0;

		if (ch != '\n')
		{
			GlyphInfo* glyphInfo = GetGlyphCache()->GetGlyph(ch);
			if (!glyphInfo) continue;

			float kerning = 0;
			if (prevChar != L'\0') kerning = GetKerning(prevChar, ch);
			prevChar = ch;

			charWidth = (int)(glyphInfo->GetAdvanceX() + kerning);
		}

		bool lineTooLong = maxWidth >= 0 && currWidth + charWidth > maxWidth;
		bool breakOnSpace = false;

		// we can't fit even a single character
		if (lineTooLong && currWidth == 0) break;


		if (ch == L'\n' || i == text.length() - 1 || lineTooLong)
		{
			int breakPoint, breakWidth;

			if (prevSpaceIndex >= 0 && lineTooLong)
			{
				breakPoint = prevSpaceIndex;
				breakWidth = prevSpaceWidth;
				breakOnSpace = true;
			}
			else
			{
				breakPoint = i;
				breakWidth = currWidth;

				breakOnSpace = (ch == L'\n');

				// we're at the end
				if (i == text.length() - 1)
				{
					breakPoint++;
					breakWidth += charWidth;
				}
			}

			if (maxHeight >= 0 && (lines.size() + 1) * GetLineHeight() > maxHeight) break;

			WideString line = text.substr(lineStartIndex, breakPoint - lineStartIndex);
			lines.push_back(new TextLine(line, breakWidth));

			currWidth = 0;
			prevChar = L'\0';
			prevSpaceIndex = -1;

			if (breakOnSpace) breakPoint++;

			lineStartIndex = breakPoint;
			i = breakPoint - 1;

			continue;
		}

		//if (ch == L' ' && currLine.empty()) continue;
		currWidth += charWidth;
	}
}

//////////////////////////////////////////////////////////////////////////
void Font::MeasureText(const WideString& text, int maxWidth, int maxHeight, int leadingSpace, int& textWidth, int& textHeight)
{
	TextLineList lines;
	WrapText(text, maxWidth, maxHeight, leadingSpace, lines);

	textHeight = (int)(lines.size() * m_LineHeight);
	textWidth = 0;

	foreach (Font::TextLine* line, lines)
	{
		textWidth = std::max(textWidth, line->GetWidth());
		SAFE_DELETE(line);
	}
}


//////////////////////////////////////////////////////////////////////////
// I/O bridge between FreeType and WME file system
//////////////////////////////////////////////////////////////////////////
unsigned long Font::FTReadSeekProc(FT_Stream stream, unsigned long offset,	unsigned char* buffer, unsigned long count)
{
	File* f = static_cast<File*>(stream->descriptor.pointer);
	if (!f) return 0;

	f->Seek(offset, File::SEEK_FROM_BEGIN);
	if (count) return f->Read(buffer, count);
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
void Font::FTCloseProc(FT_Stream stream)
{
	File* f = static_cast<File*>(stream->descriptor.pointer);
	if (!f) return;

	Game::GetInstance()->GetFileMgr()->CloseFile(f);
	stream->descriptor.pointer = NULL;
}


} // namespace Wme

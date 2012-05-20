// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TextElement2D.h"
#include "SceneNode2D.h"
#include "Font.h"
#include "FontGlyphCache.h"
#include "MaterialUtil.h"


namespace Wme
{

// TODO: Improve layer batching (all decoration layers can be rendered in a single batch)
//////////////////////////////////////////////////////////////////////////
TextElement2D::TextElement2D()
{
	m_Font = NULL;
	m_Color = Ogre::ColourValue::White;
	m_DecorationColor = Ogre::ColourValue::Black;

	m_Underline = m_Strikethrough = false;

	m_DecorationType = DECORATION_NONE;
	m_DecorationThickness = 1.0f;

	m_StrokeMaterial = MaterialUtil::GetGeometry2DMat();
}

//////////////////////////////////////////////////////////////////////////
TextElement2D::~TextElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetFont(Font* font)
{
	if (font != m_Font)
	{
		m_Font = font;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetText(const WideString& text)
{
	if (text != m_Text)
	{
		m_Text = text;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetColor(const Ogre::ColourValue& color)
{
	if (color != m_Color)
	{
		m_Color = color;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetDecorationColor(const Ogre::ColourValue& color)
{
	if (color != m_DecorationColor)
	{
		m_DecorationColor = color;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetAlignment(const Alignment& alignment)
{
	if (alignment != m_Alignment)
	{
		m_Alignment = alignment;
		SetDirty();
	}	
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetWidth(int width)
{
	if (width != m_Width)
	{
		m_Width = width;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetHeight(int height)
{
	if (height != m_Height)
	{
		m_Height = height;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetLeadingSpace(int leadingSpace)
{
	if (leadingSpace != m_LeadingSpace)
	{
		m_LeadingSpace = leadingSpace;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetDecorationType(DecorationType type)
{
	if (type != m_DecorationType)
	{
		m_DecorationType = type;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetDecorationThickness(float thickness)
{
	if (thickness != m_DecorationThickness)
	{
		m_DecorationThickness = thickness;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetUnderline(bool underline)
{
	if (underline != m_Underline)
	{
		m_Underline = underline;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::SetStrikethrough(bool strikethrough)
{
	if (strikethrough != m_Strikethrough)
	{
		m_Strikethrough = strikethrough;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::AddGeometry()
{
	if (!m_Font || m_Text.length() == 0) return;

	m_Font->PrepareGlyphs(m_Text);

	LayerList layers;
	GenerateLayers(layers);
	

	foreach (Layer& layer, layers)
	{
		// generate vertices for each character
		RenderBatchMap renderBatches;
		StrokeList strokes;
		InitializeRenderBatches(renderBatches);
		GenerateRenderBatches(renderBatches, strokes, layer);


		// add geometry to scene node
		foreach (RenderBatchMap::value_type batch, renderBatches)
		{
			m_ParentNode->AddGeometry(batch.second->GetVertices(), batch.second->GetNumCharacters() * 6, m_Font->GetGlyphCache()->GetMaterial(batch.first), Ogre::RenderOperation::OT_TRIANGLE_LIST);
		}
		RenderStrokes(strokes, layer);


		// cleanup
		foreach (RenderBatchMap::value_type batch, renderBatches)
		{
			delete batch.second;
		}
		renderBatches.clear();

		foreach (Stroke* stroke, strokes)
		{
			delete stroke;
		}
		strokes.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::InitializeRenderBatches(RenderBatchMap& renderBatches)
{
	// count number of characters used from each page to needed allocate vertices
	foreach (wchar_t ch, m_Text)
	{
		GlyphInfo* glyphInfo = m_Font->GetGlyphCache()->GetGlyph(ch);
		if (glyphInfo && glyphInfo->GetPageIndex() >= 0 && glyphInfo->GetWidth() > 0 && glyphInfo->GetHeight() > 0)
		{
			int page = glyphInfo->GetPageIndex();
			if (renderBatches.find(page) == renderBatches.end()) renderBatches[page] = new RenderBatch();
			renderBatches[page]->IncrementCapacity();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::GenerateRenderBatches(RenderBatchMap& renderBatches, StrokeList& strokes, const Layer& layer)
{
	Font::TextLineList lines;
	m_Font->WrapText(m_Text, m_Width, m_Height, m_LeadingSpace, lines);

	int posX = 0;
	int posY = (int)m_Font->GetLineHeight() - (int)m_Font->GetDescender();

	int maxPosX = posX;
	int maxPosY = posY;


	// vertical alignment	
	switch (m_Alignment.GetVertical())
	{
	case Alignment::VA_CENTER:
		posY += (int)(m_Height - (lines.size() * m_Font->GetLineHeight())) / 2;
		posY = std::max(posY, maxPosY);
		break;

	case Alignment::VA_BOTTOM:
		posY += (int)(m_Height - (lines.size() * m_Font->GetLineHeight()));
		posY = std::max(posY, maxPosY);
		break;
	}


	// render text
	bool firstLine = true;
	foreach (Font::TextLine* line, lines)
	{
		posX = 0;

		// horizontal alignment of a line
		switch (m_Alignment.GetHorizontal())
		{
		case Alignment::HA_CENTER:
			posX += (m_Width - line->GetWidth()) / 2;
			posX = std::max(posX, maxPosX);
			break;

		case Alignment::HA_RIGHT:
			posX += m_Width - line->GetWidth();
			posX = std::max(posX, maxPosX);
			break;
		}

		if (firstLine)
		{
			posX += m_LeadingSpace;
			firstLine = false;
		}

		int lineStart = posX;

		wchar_t prevChar = L'\0';
		foreach (wchar_t ch, line->GetText())
		{
			GlyphInfo* glyphInfo = m_Font->GetGlyphCache()->GetGlyph(ch);
			if (!glyphInfo) continue;

			float kerning = 0;
			if (prevChar != L'\0') kerning = m_Font->GetKerning(prevChar, ch);
			posX += (int)kerning;

			AddCharacter(ch, posX, posY, glyphInfo, renderBatches, layer);
			posX += (int)(glyphInfo->GetAdvanceX());
			posY += (int)(glyphInfo->GetAdvanceY());

			prevChar = ch;
		}

		// handle underline / strikethrough
		if (GetUnderline()) strokes.push_back(new Stroke((float)lineStart, (float)posX, (float)posY - m_Font->GetUnderlinePosition(), m_Font->GetUnderlineThickness()));
		if (GetStrikethrough()) strokes.push_back(new Stroke((float)lineStart, (float)posX, (float)posY - m_Font->GetStrikethroughPosition(), m_Font->GetUnderlineThickness()));

		posY += (int)m_Font->GetLineHeight();		
	}

	foreach (Font::TextLine* line, lines)
	{
		SAFE_DELETE(line);
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::AddCharacter(wchar_t ch, int x, int y, GlyphInfo* glyphInfo, RenderBatchMap& renderBatches, const Layer& layer)
{
	// no glyph graphics?
	if (glyphInfo->GetWidth() == 0 || glyphInfo->GetHeight() == 0) return;

	int page = glyphInfo->GetPageIndex();
	if (renderBatches.find(page) == renderBatches.end()) return;


	x += glyphInfo->GetBearingX();
	y -= glyphInfo->GetBearingY();

	// character bounds
	float posLeft, posRight, posBottom, posTop;
	posLeft = (float)x + layer.OffsetX;
	posRight = (float)(x + glyphInfo->GetWidth()) + layer.OffsetX;
	posTop = (float)y + layer.OffsetY;
	posBottom = (float)(y + glyphInfo->GetHeight()) + layer.OffsetY;

	// texture coordinates
	Ogre::FloatRect textureRect = glyphInfo->GetTextureRect();

	// add character to the render batch
	RenderBatch* renderBatch = renderBatches[page];
	renderBatch->AddCharacter(Ogre::Vector2(posLeft, posTop), Ogre::Vector2(posRight, posBottom), textureRect, layer.Decoration ? m_DecorationColor : m_Color);
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::RenderStrokes(StrokeList& strokes, const Layer& layer)
{
	if (strokes.size() == 0) return;

	Vertex2D* vertBuffer = new Vertex2D[strokes.size() * 6];
	int vertexOffset = 0;

	foreach (Stroke* stroke, strokes)
	{
		RenderStroke(stroke, vertBuffer, vertexOffset, layer);
	}
	m_ParentNode->AddGeometry(vertBuffer, strokes.size() * 6, m_StrokeMaterial, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	delete [] vertBuffer;
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::RenderStroke(const Stroke* stroke, Vertex2D* vertBuffer, int& vertexOffset, const Layer& layer)
{
	Vertex2D* verts = &vertBuffer[vertexOffset];

	Ogre::ColourValue color = layer.Decoration ? m_DecorationColor : m_Color;

	// top left
	verts[0].color = color;
	verts[0].pos = Ogre::Vector2(stroke->GetStartX() + layer.OffsetX, stroke->GetPosY() + layer.OffsetY);

	// bottom left
	verts[1].color = color;
	verts[1].pos = Ogre::Vector2(stroke->GetStartX() + layer.OffsetX, stroke->GetPosY() + stroke->GetThickness() + layer.OffsetY);

	// top right
	verts[2].color = color;
	verts[2].pos = Ogre::Vector2(stroke->GetEndX() + layer.OffsetX, stroke->GetPosY() + layer.OffsetY);

	// bottom right
	verts[5].color = color;
	verts[5].pos = Ogre::Vector2(stroke->GetEndX() + layer.OffsetX, stroke->GetPosY() + stroke->GetThickness() + layer.OffsetY);

	// the other triangle
	verts[3] = verts[2];
	verts[4] = verts[1];


	vertexOffset += 6;
}

//////////////////////////////////////////////////////////////////////////
// RenderBatch
//////////////////////////////////////////////////////////////////////////
TextElement2D::RenderBatch::RenderBatch()
{
	m_Capacity = 0;
	m_NumCharacters = 0;
	m_Vertices = NULL;
}

//////////////////////////////////////////////////////////////////////////
TextElement2D::RenderBatch::~RenderBatch()
{
	delete [] m_Vertices;
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::RenderBatch::IncrementCapacity()
{
	m_Capacity++;
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::RenderBatch::AllocateVertices()
{
	m_Vertices = new Vertex2DTex[m_Capacity * 6];
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::RenderBatch::AddCharacter(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight, const Ogre::FloatRect& texCoords, const Ogre::ColourValue& color)
{
	if (m_NumCharacters >= m_Capacity) return;
	if (!m_Vertices) AllocateVertices();

	Vertex2DTex* verts = &m_Vertices[m_NumCharacters * 6];

	// top left
	verts[0].color = color;
	verts[0].pos = Ogre::Vector2(topLeft.x, topLeft.y);
	verts[0].texCoord = Ogre::Vector2(texCoords.left, texCoords.top);

	// bottom left
	verts[1].color = color;
	verts[1].pos = Ogre::Vector2(topLeft.x, bottomRight.y);
	verts[1].texCoord = Ogre::Vector2(texCoords.left, texCoords.bottom);

	// top right
	verts[2].color = color;
	verts[2].pos = Ogre::Vector2(bottomRight.x, topLeft.y);
	verts[2].texCoord = Ogre::Vector2(texCoords.right, texCoords.top);

	// bottom right
	verts[5].color = color;
	verts[5].pos = Ogre::Vector2(bottomRight.x, bottomRight.y);
	verts[5].texCoord = Ogre::Vector2(texCoords.right, texCoords.bottom);


	// the other triangle
	verts[3] = verts[2];
	verts[4] = verts[1];


	m_NumCharacters++;
}

//////////////////////////////////////////////////////////////////////////
void TextElement2D::GenerateLayers(LayerList& layers)
{
	if (m_DecorationType == DECORATION_SHADOW)
	{
		layers.push_back(Layer(m_DecorationThickness, m_DecorationThickness, true));
	}
	else if (m_DecorationType == DECORATION_OUTLINE)
	{
		for (float row = -1.0f; row <= 1.0f; row++)
		{
			for (float col = -1.0f; col <= 1.0f; col++)
			{
				if (row == 0.0f && col == 0.0f) continue;
				
				layers.push_back(Layer(row * m_DecorationThickness, col * m_DecorationThickness, true));
			}
		}
	}

	layers.push_back(Layer(0.0f, 0.0f, false));
}


} // namespace Wme

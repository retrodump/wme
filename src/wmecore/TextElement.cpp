// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TextElement.h"
#include "Game.h"
#include "Viewport.h"
#include "Font.h"
#include "FontGlyphCache.h"
#include "StringUtil.h"
#include "Sprite.h"
#include "ElementCollection.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TextElement::TextElement()
{
	m_HWBufferInitialized = false;
	m_NumCharacters = 0;

	m_Color = Ogre::ColourValue::White;

	m_Font = NULL;

	m_PosX = m_PosY = 0;
	m_MaxWidth = m_MaxHeight = -1;

	m_IsDirty = false;
	m_UpdateId = 0;
}

//////////////////////////////////////////////////////////////////////////
TextElement::~TextElement()
{
	DestroyHWBuffer();
	ClearRenderBatches();
}

//////////////////////////////////////////////////////////////////////////
void TextElement::SetFont(Font* font)
{
	if (font != m_Font)
	{
		m_Font = font;
		m_IsDirty = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement::SetRect(const Rect& rect)
{
	SetPosX((int)rect.x);
	SetPosY((int)rect.y);
	SetMaxWidth((int)rect.width);
	SetMaxHeight((int)rect.height);
}

//////////////////////////////////////////////////////////////////////////
void TextElement::SetAlignment(const Alignment& alignment)
{
	if (alignment != m_Alignment)
	{
		m_IsDirty = true;
		m_Alignment = alignment;
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement::Display(ElementCollection* elementCol, const SpriteDrawingParams& params)
{
	UpdateGeometry(params.Viewport, params.AbsolutePos);
	elementCol->AddElement(this);
}

//////////////////////////////////////////////////////////////////////////
void TextElement::UpdateGeometry(Viewport* viewport, bool absolutePos)
{
	if (!m_Font) return;
	if (!m_IsDirty && !viewport->IsDirty(m_UpdateId)) return;

	m_UpdateId = viewport->GetUpdateId();

	m_Font->PrepareGlyphs(m_Text);

	ResizeHWBuffer(m_Text.length());
	UpdateRenderBatches();

	Font::TextLineList lines;
	m_Font->WrapText(m_Text, m_MaxWidth, m_MaxHeight, 0, lines);


	SpriteVertex* vertexBuf = reinterpret_cast<SpriteVertex*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	int posX = m_PosX;
	int posY = m_PosY + (int)m_Font->GetLineHeight() - (int)m_Font->GetDescender();

	int maxPosX = posX;
	int maxPosY = posY;
	

	// vertical alignment	
	switch (m_Alignment.GetVertical())
	{
	case Alignment::VA_CENTER:
		if (m_MaxHeight > 0)
		{
			posY += (int)(m_MaxHeight - (lines.size() * m_Font->GetLineHeight())) / 2;
			posY = std::max(posY, maxPosY);
		}
		else
		{
			posY -= (int)((lines.size() * m_Font->GetLineHeight()) / 2);
		}		
		break;

	case Alignment::VA_BOTTOM:
		if (m_MaxHeight > 0)
		{
			posY += (int)(m_MaxHeight - (lines.size() * m_Font->GetLineHeight()));
			posY = std::max(posY, maxPosY);
		}
		else
		{
			posY -= (int)(lines.size() * m_Font->GetLineHeight());
		}
		break;
	}
	

	// render text
	foreach (Font::TextLine* line, lines)
	{
		posX = m_PosX;

		// horizontal alignment of a line
		switch (m_Alignment.GetHorizontal())
		{
		case Alignment::HA_CENTER:
			if (m_MaxWidth > 0)
			{
				posX += (m_MaxWidth - line->GetWidth()) / 2;
				posX = std::max(posX, maxPosX);
			}
			else
			{
				posX -= line->GetWidth() / 2;
			}
			break;

		case Alignment::HA_RIGHT:
			if (m_MaxWidth > 0)
			{
				posX += m_MaxWidth - line->GetWidth();
				posX = std::max(posX, maxPosX);
			}
			else
			{
				posX -= line->GetWidth();
			}
			break;
		}



		wchar_t prevChar = L'\0';
		foreach (wchar_t ch, line->GetText())
		{
			GlyphInfo* glyphInfo = m_Font->GetGlyphCache()->GetGlyph(ch);
			if (!glyphInfo) continue;

			float kerning = 0;
			if (prevChar != L'\0') kerning = m_Font->GetKerning(prevChar, ch);
			posX += (int)kerning;

			AddCharacter(ch, posX, posY, glyphInfo, vertexBuf, viewport, absolutePos);
			posX += (int)(glyphInfo->GetAdvanceX());
			posY += (int)(glyphInfo->GetAdvanceY());

			prevChar = ch;
		}		
		posY += (int)m_Font->GetLineHeight();
	}

	m_HardwareBuffer->unlock();

	foreach (Font::TextLine* line, lines)
	{
		SAFE_DELETE(line);
	}

	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void TextElement::ClearRenderBatches()
{
	foreach (TextRenderBatch* batch, m_RenderBatches)
	{
		SAFE_DELETE(batch);
	}
	m_RenderBatches.clear();
}

//////////////////////////////////////////////////////////////////////////
void TextElement::UpdateRenderBatches()
{
	int numPages = m_Font->GetGlyphCache()->GetNumPages();
	if (m_RenderBatches.size() != numPages)
	{
		ClearRenderBatches();
		for (int i = 0; i < numPages; i++)
			m_RenderBatches.push_back(new TextRenderBatch());
	}


	if (numPages == 1)
	{
		m_RenderBatches[0]->m_BatchIndex = 0;
		m_RenderBatches[0]->m_StartIndex = 0;
		m_RenderBatches[0]->m_CurrentIndex = 0;
		m_RenderBatches[0]->m_NumChars = m_Text.length();
		m_RenderBatches[0]->m_TextElement = this;
	}
	else
	{
		// clear the values
		int batchIndex = 0;
		foreach (TextRenderBatch* batch, m_RenderBatches)
		{
			batch->m_BatchIndex = batchIndex;
			batchIndex++;

			batch->m_StartIndex = 0;
			batch->m_CurrentIndex = 0;
			batch->m_NumChars = 0;
			batch->m_TextElement = this;
		}

		// count number of characters used from each page
		foreach (wchar_t ch, m_Text)
		{
			GlyphInfo* glyphInfo = m_Font->GetGlyphCache()->GetGlyph(ch);
			if (glyphInfo && glyphInfo->GetPageIndex() >= 0 && glyphInfo->GetPageIndex() < numPages && glyphInfo->GetWidth() > 0 && glyphInfo->GetHeight() > 0)
			{
				m_RenderBatches[glyphInfo->GetPageIndex()]->m_NumChars++;
			}
		}
	}


	// update indices
	int startIndex = 0;
	foreach (TextRenderBatch* batch, m_RenderBatches)
	{
		batch->m_StartIndex = startIndex;
		startIndex += batch->m_NumChars;

		batch->m_NumChars = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement::AddCharacter(wchar_t ch, int X, int Y, GlyphInfo* glyphInfo, SpriteVertex* vertexBuf, Viewport* viewport, bool absolutePos)
{
	// no glyph graphics?
	if (glyphInfo->GetWidth() == 0 || glyphInfo->GetHeight() == 0) return;


	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
	TextRenderBatch* batch = m_RenderBatches[glyphInfo->GetPageIndex()];

	X += glyphInfo->GetBearingX();
	Y -= glyphInfo->GetBearingY();

	SpriteVertex verts[4];
	
	float posLeft, posRight, posBottom, posTop;
	posLeft = (float)X;
	posRight = (float)(X + glyphInfo->GetWidth());
	posTop = (float)Y;
	posBottom = (float)(Y + glyphInfo->GetHeight());

	verts[0].pos = Ogre::Vector3(posLeft,  posBottom, -1.0f); // lower left
	verts[1].pos = Ogre::Vector3(posLeft,  posTop,    -1.0f); // top left
	verts[2].pos = Ogre::Vector3(posRight, posBottom, -1.0f); // bottom right
	verts[3].pos = Ogre::Vector3(posRight, posTop,    -1.0f); // top right

	viewport->TransformVertices(verts, 4, absolutePos);


	// color
	Ogre::uint32 color;
	rs->convertColourValue(m_Color, &color); // convert to platform native format
	verts[0].color = verts[1].color = verts[2].color = verts[3].color = color;


	// texture coordinates
	Ogre::FloatRect rect = glyphInfo->GetTextureRect();

	verts[0].texCoord = Ogre::Vector2(rect.left,  rect.bottom); // lower left
	verts[1].texCoord = Ogre::Vector2(rect.left,  rect.top);    // top left
	verts[2].texCoord = Ogre::Vector2(rect.right, rect.bottom); // bottom right
	verts[3].texCoord = Ogre::Vector2(rect.right, rect.top); // top right

	
	// create two triangles in HW buffer
	SpriteVertex* finalVerts = &vertexBuf[(batch->m_StartIndex + batch->m_CurrentIndex) * 6];

	finalVerts[0] = verts[1];
	finalVerts[1] = verts[0];
	finalVerts[2] = verts[3];

	finalVerts[3] = verts[3];
	finalVerts[4] = verts[0];
	finalVerts[5] = verts[2];


	batch->m_CurrentIndex++;
	batch->m_NumChars++;
}

//////////////////////////////////////////////////////////////////////////
void TextElement::ResizeHWBuffer(size_t numCharacters)
{
	if (numCharacters == 0) numCharacters = 1;

	if (!m_HWBufferInitialized)
	{
		m_RenderOperation.useIndexes = false;
		m_RenderOperation.vertexData = new Ogre::VertexData();
		Ogre::VertexDeclaration* decl = m_RenderOperation.vertexData->vertexDeclaration;

		size_t offset = 0;

		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

		decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

		m_HWBufferInitialized = true;
	}

	if (m_NumCharacters < numCharacters)
	{
		m_HardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			m_RenderOperation.vertexData->vertexDeclaration->getVertexSize(0),
			6 * numCharacters,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


		m_RenderOperation.vertexData->vertexBufferBinding->setBinding(0, m_HardwareBuffer);

		m_RenderOperation.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

		m_NumCharacters = numCharacters;
	}
}

//////////////////////////////////////////////////////////////////////////
void TextElement::DestroyHWBuffer()
{
	SAFE_DELETE(m_RenderOperation.vertexData);
	if (!m_HardwareBuffer.isNull()) m_HardwareBuffer.setNull();
}

//////////////////////////////////////////////////////////////////////////
// IRenderableProvider
//////////////////////////////////////////////////////////////////////////
void TextElement::GetRenderables(IRenderableProvider::RenderableList& renderableList)
{
	foreach (TextRenderBatch* batch, m_RenderBatches)
	{
		if (!batch->m_NumChars) continue;
		renderableList.push_back(batch);
	}
}


//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable implementation for TextRenderBatch
//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& TextElement::TextRenderBatch::getMaterial(void) const
{
	return m_TextElement->GetFont()->GetGlyphCache()->GetMaterial(m_BatchIndex);
}

//////////////////////////////////////////////////////////////////////////
void TextElement::TextRenderBatch::getRenderOperation(Ogre::RenderOperation& op)
{
	op = m_TextElement->GetRenderOp();
	op.vertexData->vertexStart = m_StartIndex * 6;
	op.vertexData->vertexCount = m_NumChars * 6;
}

//////////////////////////////////////////////////////////////////////////
void TextElement::TextRenderBatch::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& TextElement::TextRenderBatch::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


} // namespace Wme

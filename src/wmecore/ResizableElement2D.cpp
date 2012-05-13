// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ResizableElement2D.h"
#include "SceneNode2D.h"
#include "SpriteTexture.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ResizableElement2D::ResizableElement2D()
{
	m_Width = m_Height = 0.0f;
	m_Texture = NULL;
	m_Color = Ogre::ColourValue::White;

	m_FrameTopHeight = m_FrameBottomHeight = m_FrameLeftWidth = m_FrameRightWidth = 0.0f;

	m_StretchHorizontal = m_StretchVertical = true;
	m_PaintMiddlePart = true;
}

//////////////////////////////////////////////////////////////////////////
ResizableElement2D::~ResizableElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::AddGeometry()
{
	if (m_Width == 0.0f || m_Height == 0.0f || m_Texture == NULL) return;

	int numTiles = 0;
	int tilesVertical = 0;
	int tilesHorizontal = 0;
	GetNumTiles(numTiles, tilesHorizontal, tilesVertical);
	
	Vertex2DTex* verts = new Vertex2DTex[numTiles * 6];
	int vertexOffset = 0;

	float middleTileWidth = m_Texture->GetWidth() - m_FrameLeftWidth - m_FrameRightWidth;
	float middleTileHeight = m_Texture->GetHeight() - m_FrameTopHeight - m_FrameBottomHeight;

	float middlePartWidth = m_Width - m_FrameLeftWidth - m_FrameRightWidth;
	float middlePartHeight = m_Height - m_FrameTopHeight - m_FrameBottomHeight;
	

	float col = 0.0f;
	float row = 0.0f;

	// top row
	if (m_FrameTopHeight > 0.0f)
	{
		if (m_FrameLeftWidth)
		{
			AddTile(row, col, TILE_TOP_LEFT, m_FrameLeftWidth, m_FrameTopHeight, verts, vertexOffset);
			col += m_FrameLeftWidth;
		}
		if (tilesHorizontal > 0)
		{
			if (m_StretchHorizontal)
			{
				AddTile(row, col, TILE_TOP_MIDDLE, middlePartWidth, m_FrameTopHeight, verts, vertexOffset);
				col += middlePartWidth;
			}
			else
			{
				for (int i = 0; i < tilesHorizontal; i++)
				{
					AddTile(row, col, TILE_TOP_MIDDLE, middleTileWidth, m_FrameTopHeight, verts, vertexOffset);
					col += middleTileWidth;
				}
			}
		}
		if (m_FrameRightWidth)
		{
			AddTile(row, col, TILE_TOP_RIGHT, m_FrameRightWidth, m_FrameTopHeight, verts, vertexOffset);
			col += m_FrameRightWidth;
		}

		row += m_FrameTopHeight;
		col = 0.0f;
	}

	// middle part
	if (tilesVertical > 0)
	{
		float middleWidth = m_StretchHorizontal ? middlePartWidth : middleTileWidth;
		float middleHeight = m_StretchVertical ? middlePartHeight : middleTileHeight;

		for (int y = 0; y < tilesVertical; y++)
		{
			if (m_FrameLeftWidth)
			{
				AddTile(row, col, TILE_MIDDLE_LEFT, m_FrameLeftWidth, middleHeight, verts, vertexOffset);
				col += m_FrameLeftWidth;
			}

			for (int x = 0; x < tilesHorizontal; x++)
			{
				if (m_PaintMiddlePart) AddTile(row, col, TILE_MIDDLE_MIDDLE, middleWidth, middleHeight, verts, vertexOffset);
				col += middleWidth;
			}

			if (m_FrameRightWidth)
			{
				AddTile(row, col, TILE_MIDDLE_RIGHT, m_FrameRightWidth, middleHeight, verts, vertexOffset);
				col += m_FrameLeftWidth;
			}
			row += middleHeight;
			col = 0.0f;
		}
	}

	// bottom row
	if (m_FrameBottomHeight > 0.0f)
	{
		if (m_FrameLeftWidth)
		{
			AddTile(row, col, TILE_BOTTOM_LEFT, m_FrameLeftWidth, m_FrameBottomHeight, verts, vertexOffset);
			col += m_FrameLeftWidth;
		}
		if (tilesHorizontal > 0)
		{
			if (m_StretchHorizontal)
			{
				AddTile(row, col, TILE_BOTTOM_MIDDLE, middlePartWidth, m_FrameBottomHeight, verts, vertexOffset);
				col += middlePartWidth;
			}
			else
			{
				for (int i = 0; i < tilesHorizontal; i++)
				{
					AddTile(row, col, TILE_BOTTOM_MIDDLE, middleTileWidth, m_FrameBottomHeight, verts, vertexOffset);
					col += middleTileWidth;
				}
			}
		}
		if (m_FrameRightWidth)
		{
			AddTile(row, col, TILE_BOTTOM_RIGHT, m_FrameRightWidth, m_FrameBottomHeight, verts, vertexOffset);
			col += m_FrameRightWidth;
		}

		row += m_FrameBottomHeight;
		col = 0.0f;
	}

	m_ParentNode->AddGeometry(verts, numTiles * 6, m_Texture->GetMaterial(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
	delete [] verts;
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::GetNumTiles(int& numTiles, int& tilesHorizontal, int& tilesVertical)
{
	numTiles = tilesHorizontal = tilesVertical = 0;

	if (m_StretchHorizontal) tilesHorizontal = 1;
	else tilesHorizontal = (int)floor((m_Width - m_FrameLeftWidth - m_FrameRightWidth) / ((float)m_Texture->GetWidth() - m_FrameLeftWidth - m_FrameRightWidth));

	if (m_StretchVertical) tilesVertical = 1;
	else tilesVertical = (int)floor((m_Height - m_FrameTopHeight - m_FrameBottomHeight) / ((float)m_Texture->GetHeight() - m_FrameTopHeight - m_FrameBottomHeight));


	float f = m_Width / ((float)m_Texture->GetWidth() - m_FrameLeftWidth - m_FrameRightWidth);
	f = floor(f);

	// top row
	if (m_FrameTopHeight > 0.0f)
	{
		if (m_FrameLeftWidth > 0.0f) numTiles++;
		numTiles += tilesHorizontal;
		if (m_FrameRightWidth > 0.0f) numTiles++;
	}

	// middle part
	if (m_FrameLeftWidth > 0.0f) numTiles += tilesVertical;
	if (m_PaintMiddlePart) numTiles += tilesVertical * tilesHorizontal;
	if (m_FrameRightWidth > 0.0f) numTiles += tilesVertical;

	// bottom row
	if (m_FrameBottomHeight > 0.0f)
	{
		if (m_FrameLeftWidth > 0.0f) numTiles++;
		numTiles += tilesHorizontal;
		if (m_FrameRightWidth > 0.0f) numTiles++;
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::AddTile(float row, float col, TileType tileType, float width, float height, Vertex2DTex* vertBuffer, int& vertexOffset)
{
	Vertex2DTex* verts = &vertBuffer[vertexOffset];
	
	Ogre::FloatRect rect;
	GetTextureCoordinates(tileType, rect);

	// top left
	verts[0].pos = Ogre::Vector2(col, row);
	verts[0].texCoord = Ogre::Vector2(rect.left,  rect.top);

	// bottom left
	verts[1].pos = Ogre::Vector2(col, row + height);
	verts[1].texCoord = Ogre::Vector2(rect.left,  rect.bottom);

	// top right
	verts[2].pos = Ogre::Vector2(col + width, row);
	verts[2].texCoord = Ogre::Vector2(rect.right, rect.top);

	// bottom right
	verts[5].pos = Ogre::Vector2(col + width, row + height);
	verts[5].texCoord = Ogre::Vector2(rect.right, rect.bottom);

	// the other triangle
	verts[3] = verts[2];
	verts[4] = verts[1];

	// color
	for (int i = 0; i < 6; i++)
	{
		verts[i].color = m_Color;
	}

	vertexOffset += 6;
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::GetTextureCoordinates(TileType type, Ogre::FloatRect& rect)
{
	float leftColLeft = 0;
	float leftColRight = m_FrameLeftWidth;

	float rightColLeft = (float)m_Texture->GetWidth() - m_FrameRightWidth;
	float rightColRight = (float)m_Texture->GetWidth();

	float topRowTop = 0;
	float topRowBottom = (float)m_FrameTopHeight;

	float bottomRowTop = (float)m_Texture->GetHeight() - m_FrameBottomHeight;
	float bottomRowBottom = (float)m_Texture->GetHeight();


	switch (type)
	{
	case TILE_TOP_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case TILE_TOP_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case TILE_TOP_RIGHT:
		rect.left = rightColLeft;
		rect.right = rightColRight;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case TILE_MIDDLE_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case TILE_MIDDLE_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case TILE_MIDDLE_RIGHT:
		rect.left = rightColLeft;
		rect.right = rightColRight;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case TILE_BOTTOM_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = bottomRowTop;		
		rect.bottom = bottomRowBottom;
		break;

	case TILE_BOTTOM_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = bottomRowTop;		
		rect.bottom = bottomRowBottom;
		break;

	case TILE_BOTTOM_RIGHT:
		rect.left = rightColLeft;
		rect.right = rightColRight;
		rect.top = bottomRowTop;		
		rect.bottom = bottomRowBottom;
		break;
	}

	// relative coordinates
	rect.left /= (float)m_Texture->GetWidth();
	rect.right /= (float)m_Texture->GetWidth();
	rect.top /= (float)m_Texture->GetHeight();
	rect.bottom /= (float)m_Texture->GetHeight();

	// compensate for non-pow2 dimensions
	rect.left *= m_Texture->GetTexCoordU();
	rect.right *= m_Texture->GetTexCoordU();
	rect.top *= m_Texture->GetTexCoordV();
	rect.bottom *= m_Texture->GetTexCoordV();
}


//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetWidth(float val)
{
	if (val != m_Width)
	{
		m_Width = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetHeight(float val)
{
	if (val != m_Height)
	{
		m_Height = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetTexture(SpriteTexture* val)
{
	if (val != m_Texture)
	{
		m_Texture = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetColor(const Ogre::ColourValue& val)
{
	if (val != m_Color)
	{
		m_Color = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetFrameTopHeight(float val)
{
	if (val != m_FrameTopHeight)
	{
		m_FrameTopHeight = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetFrameBottomHeight(float val)
{
	if (val != m_FrameBottomHeight)
	{
		m_FrameBottomHeight = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetFrameLeftWidth(float val)
{
	if (val != m_FrameLeftWidth)
	{
		m_FrameLeftWidth = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetFrameRightWidth(float val)
{
	if (val != m_FrameRightWidth)
	{
		m_FrameRightWidth = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetStretchVertical(bool val)
{
	if (val != m_StretchVertical)
	{
		m_StretchVertical = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetStretchHorizontal(bool val)
{
	if (val != m_StretchHorizontal)
	{
		m_StretchHorizontal = val;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetPaintMiddlePart(bool val)
{
	if (val != m_PaintMiddlePart)
	{
		m_PaintMiddlePart = val;
		SetDirty();
	}
}


} // namespace Wme

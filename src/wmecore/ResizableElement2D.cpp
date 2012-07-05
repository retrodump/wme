// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ResizableElement2D.h"
#include "SceneNode2D.h"
#include "ResizableImage.h"
#include "SpriteTexture.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ResizableElement2D::ResizableElement2D()
{
	m_Width = m_Height = 0.0f;
	m_Image = NULL;
	m_Color = Ogre::ColourValue::White;
}

//////////////////////////////////////////////////////////////////////////
ResizableElement2D::~ResizableElement2D()
{
	if (m_Image) m_Image->RemoveListener(this);
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::AddGeometry()
{
	if (m_Width == 0.0f || m_Height == 0.0f || m_Image == NULL || m_Image->GetTexture() == NULL) return;

	int numTiles = 0;
	int tilesVertical = 0;
	int tilesHorizontal = 0;
	GetNumTiles(numTiles, tilesHorizontal, tilesVertical);
	
	Vertex2DTex* verts = new Vertex2DTex[numTiles * 6];
	int vertexOffset = 0;

	float middleTileWidth = m_Image->GetTexture()->GetWidth() - m_Image->GetFrameLeftWidth() - m_Image->GetFrameRightWidth();
	float middleTileHeight = m_Image->GetTexture()->GetHeight() - m_Image->GetFrameTopHeight() - m_Image->GetFrameBottomHeight();

	float middlePartWidth = m_Width - m_Image->GetFrameLeftWidth() - m_Image->GetFrameRightWidth();
	float middlePartHeight = m_Height - m_Image->GetFrameTopHeight() - m_Image->GetFrameBottomHeight();
	

	float col = 0.0f;
	float row = 0.0f;

	// top row
	if (m_Image->GetFrameTopHeight() > 0.0f)
	{
		if (m_Image->GetFrameLeftWidth())
		{
			AddTile(row, col, TILE_TOP_LEFT, m_Image->GetFrameLeftWidth(), m_Image->GetFrameTopHeight(), verts, vertexOffset);
			col += m_Image->GetFrameLeftWidth();
		}
		if (tilesHorizontal > 0)
		{
			if (m_Image->GetStretchHorizontal())
			{
				AddTile(row, col, TILE_TOP_MIDDLE, middlePartWidth, m_Image->GetFrameTopHeight(), verts, vertexOffset);
				col += middlePartWidth;
			}
			else
			{
				for (int i = 0; i < tilesHorizontal; i++)
				{
					AddTile(row, col, TILE_TOP_MIDDLE, middleTileWidth, m_Image->GetFrameTopHeight(), verts, vertexOffset);
					col += middleTileWidth;
				}
			}
		}
		if (m_Image->GetFrameRightWidth())
		{
			AddTile(row, col, TILE_TOP_RIGHT, m_Image->GetFrameRightWidth(), m_Image->GetFrameTopHeight(), verts, vertexOffset);
			col += m_Image->GetFrameRightWidth();
		}

		row += m_Image->GetFrameTopHeight();
		col = 0.0f;
	}

	// middle part
	if (tilesVertical > 0)
	{
		float middleWidth = m_Image->GetStretchHorizontal() ? middlePartWidth : middleTileWidth;
		float middleHeight = m_Image->GetStretchVertical() ? middlePartHeight : middleTileHeight;

		for (int y = 0; y < tilesVertical; y++)
		{
			if (m_Image->GetFrameLeftWidth())
			{
				AddTile(row, col, TILE_MIDDLE_LEFT, m_Image->GetFrameLeftWidth(), middleHeight, verts, vertexOffset);
				col += m_Image->GetFrameLeftWidth();
			}

			for (int x = 0; x < tilesHorizontal; x++)
			{
				if (m_Image->GetPaintMiddlePart()) AddTile(row, col, TILE_MIDDLE_MIDDLE, middleWidth, middleHeight, verts, vertexOffset);
				col += middleWidth;
			}

			if (m_Image->GetFrameRightWidth())
			{
				AddTile(row, col, TILE_MIDDLE_RIGHT, m_Image->GetFrameRightWidth(), middleHeight, verts, vertexOffset);
				col += m_Image->GetFrameLeftWidth();
			}
			row += middleHeight;
			col = 0.0f;
		}
	}

	// bottom row
	if (m_Image->GetFrameBottomHeight() > 0.0f)
	{
		if (m_Image->GetFrameLeftWidth())
		{
			AddTile(row, col, TILE_BOTTOM_LEFT, m_Image->GetFrameLeftWidth(), m_Image->GetFrameBottomHeight(), verts, vertexOffset);
			col += m_Image->GetFrameLeftWidth();
		}
		if (tilesHorizontal > 0)
		{
			if (m_Image->GetStretchHorizontal())
			{
				AddTile(row, col, TILE_BOTTOM_MIDDLE, middlePartWidth, m_Image->GetFrameBottomHeight(), verts, vertexOffset);
				col += middlePartWidth;
			}
			else
			{
				for (int i = 0; i < tilesHorizontal; i++)
				{
					AddTile(row, col, TILE_BOTTOM_MIDDLE, middleTileWidth, m_Image->GetFrameBottomHeight(), verts, vertexOffset);
					col += middleTileWidth;
				}
			}
		}
		if (m_Image->GetFrameRightWidth())
		{
			AddTile(row, col, TILE_BOTTOM_RIGHT, m_Image->GetFrameRightWidth(), m_Image->GetFrameBottomHeight(), verts, vertexOffset);
			col += m_Image->GetFrameRightWidth();
		}

		row += m_Image->GetFrameBottomHeight();
		col = 0.0f;
	}

	m_ParentNode->AddGeometry(verts, numTiles * 6, m_Image->GetTexture()->GetMaterial(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
	delete [] verts;
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::GetNumTiles(int& numTiles, int& tilesHorizontal, int& tilesVertical)
{
	numTiles = tilesHorizontal = tilesVertical = 0;

	if (m_Image->GetStretchHorizontal()) tilesHorizontal = 1;
	else tilesHorizontal = (int)floor((m_Width - m_Image->GetFrameLeftWidth() - m_Image->GetFrameRightWidth()) / ((float)m_Image->GetTexture()->GetWidth() - m_Image->GetFrameLeftWidth() - m_Image->GetFrameRightWidth()));

	if (m_Image->GetStretchVertical()) tilesVertical = 1;
	else tilesVertical = (int)floor((m_Height - m_Image->GetFrameTopHeight() - m_Image->GetFrameBottomHeight()) / ((float)m_Image->GetTexture()->GetHeight() - m_Image->GetFrameTopHeight() - m_Image->GetFrameBottomHeight()));


	float f = m_Width / ((float)m_Image->GetTexture()->GetWidth() - m_Image->GetFrameLeftWidth() - m_Image->GetFrameRightWidth());
	f = floor(f);

	// top row
	if (m_Image->GetFrameTopHeight() > 0.0f)
	{
		if (m_Image->GetFrameLeftWidth() > 0.0f) numTiles++;
		numTiles += tilesHorizontal;
		if (m_Image->GetFrameRightWidth() > 0.0f) numTiles++;
	}

	// middle part
	if (m_Image->GetFrameLeftWidth() > 0.0f) numTiles += tilesVertical;
	if (m_Image->GetPaintMiddlePart()) numTiles += tilesVertical * tilesHorizontal;
	if (m_Image->GetFrameRightWidth() > 0.0f) numTiles += tilesVertical;

	// bottom row
	if (m_Image->GetFrameBottomHeight() > 0.0f)
	{
		if (m_Image->GetFrameLeftWidth() > 0.0f) numTiles++;
		numTiles += tilesHorizontal;
		if (m_Image->GetFrameRightWidth() > 0.0f) numTiles++;
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
	float leftColRight = m_Image->GetFrameLeftWidth();

	float rightColLeft = (float)m_Image->GetTexture()->GetWidth() - m_Image->GetFrameRightWidth();
	float rightColRight = (float)m_Image->GetTexture()->GetWidth();

	float topRowTop = 0;
	float topRowBottom = (float)m_Image->GetFrameTopHeight();

	float bottomRowTop = (float)m_Image->GetTexture()->GetHeight() - m_Image->GetFrameBottomHeight();
	float bottomRowBottom = (float)m_Image->GetTexture()->GetHeight();


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
	rect.left /= (float)m_Image->GetTexture()->GetWidth();
	rect.right /= (float)m_Image->GetTexture()->GetWidth();
	rect.top /= (float)m_Image->GetTexture()->GetHeight();
	rect.bottom /= (float)m_Image->GetTexture()->GetHeight();

	// compensate for non-pow2 dimensions
	rect.left *= m_Image->GetTexture()->GetTexCoordU();
	rect.right *= m_Image->GetTexture()->GetTexCoordU();
	rect.top *= m_Image->GetTexture()->GetTexCoordV();
	rect.bottom *= m_Image->GetTexture()->GetTexCoordV();
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
void ResizableElement2D::SetSize(float width, float height)
{
	if (width != m_Width || height != m_Height)
	{
		m_Width = width;
		m_Height = height;
		SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::SetImage(ResizableImage* val)
{
	if (val != m_Image)
	{
		if (m_Image) m_Image->RemoveListener(this);

		m_Image = val;
		if (m_Image) m_Image->AddListener(this);

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
// ResizableImage::Listener
//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::OnChanged()
{
	SetDirty();
}

//////////////////////////////////////////////////////////////////////////
void ResizableElement2D::OnDestroy()
{
	m_Image = NULL;
	SetDirty();
}


} // namespace Wme

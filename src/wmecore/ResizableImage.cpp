// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ResizableImage.h"
#include "SpriteTexture.h"
#include "Viewport.h"
#include "XmlUtil.h"
#include "Sprite.h"
#include "ElementCollection.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ResizableImage::ResizableImage()
{
	m_IsDirty = true;
	m_UpdateId = 0;

	m_Texture = NULL;
	m_Type = IMAGE_STRETCHED;

	m_Color = Ogre::ColourValue::White;

	m_PosX = m_PosY = 0;
	m_Width = m_Height = 0;
	m_CorrectedWidth = m_CorrectedHeight = 0;

	m_FrameTopHeight = m_FrameBottomHeight = m_FrameLeftWidth = m_FrameRightWidth = 0;

	m_MaxTiles = 0;
	m_HWBufferInitialized = false;

	// Ogre::Renderable
	mUseIdentityProjection = true;
	mUseIdentityView = true;
	mPolygonModeOverrideable = false;
}

//////////////////////////////////////////////////////////////////////////
ResizableImage::~ResizableImage()
{
	SAFE_DELETE(m_RenderOperation.vertexData);
	if (!m_HardwareBuffer.isNull()) m_HardwareBuffer.setNull();

	SAFE_DELETE(m_Texture);
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::Display(ElementCollection* elementCol, const SpriteDrawingParams& params)
{
	if (!m_Texture) return;

	UpdateGeometry(params.Viewport, params.AbsolutePos);
	elementCol->AddElement(this);
}

//////////////////////////////////////////////////////////////////////////
bool ResizableImage::SetTexture(const WideString& fileName, Ogre::ColourValue colorKey)
{
	SAFE_DELETE(m_Texture);

	m_Texture = new SpriteTexture();
	if (m_Texture->SetTexture(fileName, colorKey)) return true;
	else
	{
		SAFE_DELETE(m_Texture);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetPos(int posX, int posY)
{
	if (posX != m_PosX || posY != m_PosY) m_IsDirty = true;

	m_PosX = posX;
	m_PosY = posY;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::SetSize(int width, int height)
{
	if (width != m_Width || height != m_Height) m_IsDirty = true;

	m_Width = width;
	m_Height = height;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::UpdateGeometry(Viewport* viewport, bool absolutePos)
{
	if (!m_Texture) return;
	if (!m_IsDirty && !viewport->IsDirty(m_UpdateId)) return;

	m_UpdateId = viewport->GetUpdateId();


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

	BuildTiles();

	if (m_Tiles.size() > m_MaxTiles)
	{
		m_HardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			m_RenderOperation.vertexData->vertexDeclaration->getVertexSize(0),
			m_Tiles.size() * 6,
			Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


		m_RenderOperation.vertexData->vertexBufferBinding->setBinding(0, m_HardwareBuffer);
		m_RenderOperation.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

		m_MaxTiles = m_Tiles.size();
	}

	FillTiles(viewport, absolutePos);
	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
int ResizableImage::GetCorrectedWidth() const
{
	int middleWidth, middleHeight, middleTileWidth, middleTileHeight;
	GetCorrectedSize(middleWidth, middleHeight, middleTileWidth, middleTileHeight);

	return m_CorrectedWidth;
}

//////////////////////////////////////////////////////////////////////////
int ResizableImage::GetCorrectedHeight() const
{
	int middleWidth, middleHeight, middleTileWidth, middleTileHeight;
	GetCorrectedSize(middleWidth, middleHeight, middleTileWidth, middleTileHeight);

	return m_CorrectedHeight;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::GetCorrectedSize(int& middleWidth, int& middleHeight, int& middleTileWidth, int& middleTileHeight) const
{
	m_CorrectedWidth = m_Width;
	m_CorrectedHeight = m_Height;

	if (!m_Texture) return;


	if (m_Type == IMAGE_STRETCHED)
	{
		middleWidth = middleTileWidth = m_Width - m_FrameLeftWidth - m_FrameRightWidth;
		middleHeight = middleTileHeight = m_Height - m_FrameTopHeight - m_FrameBottomHeight;
		return;
	}

	m_CorrectedWidth = std::max(m_CorrectedWidth, (int)(m_FrameLeftWidth + m_FrameRightWidth));
	m_CorrectedHeight = std::max(m_CorrectedHeight, (int)(m_FrameTopHeight + m_FrameBottomHeight));


	middleTileWidth = m_Texture->GetWidth() - m_FrameLeftWidth - m_FrameRightWidth;
	middleTileHeight = m_Texture->GetHeight() - m_FrameTopHeight - m_FrameBottomHeight;

	middleWidth = m_CorrectedWidth - m_FrameLeftWidth - m_FrameRightWidth;
	if (middleWidth % middleTileWidth > 0) middleWidth += (middleTileWidth - (middleWidth % middleTileWidth));

	middleHeight = m_CorrectedHeight - m_FrameTopHeight - m_FrameBottomHeight;
	if (middleHeight % middleTileHeight > 0) middleHeight += (middleTileHeight - (middleHeight % middleTileHeight));

	m_CorrectedWidth = m_FrameLeftWidth + middleWidth + m_FrameRightWidth;
	m_CorrectedHeight = m_FrameTopHeight + middleHeight + m_FrameBottomHeight;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::BuildTiles()
{
	m_Tiles.clear();

	int middleWidth, middleHeight, middleTileWidth, middleTileHeight;
	GetCorrectedSize(middleWidth, middleHeight, middleTileWidth, middleTileHeight);

	if (m_Type == IMAGE_STRETCHED)
	{
		m_Tiles.push_back(ImageTile(m_PosX, m_PosY, m_Width, m_Height, ImageTile::TILE_ALL));
		return;
	}

	int numMiddleCols = middleWidth / middleTileWidth;
	int numMiddleRows = middleHeight / middleTileHeight;


	// top row
	if (m_FrameTopHeight > 0)
	{
		int rowY = m_PosY;

		if (m_FrameLeftWidth > 0)
		{
			m_Tiles.push_back(ImageTile(m_PosX, rowY, m_FrameLeftWidth, m_FrameTopHeight, ImageTile::TILE_TOP_LEFT));
		}

		if (m_FrameRightWidth > 0)
		{
			m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + middleWidth, rowY, m_FrameRightWidth, m_FrameTopHeight, ImageTile::TILE_TOP_RIGHT));
		}

		if (middleTileWidth > 0)
		{
			for (int col = 0; col < numMiddleCols; col++)
			{
				m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + col * middleTileWidth, rowY, middleTileWidth, m_FrameTopHeight, ImageTile::TILE_TOP_MIDDLE));
			}
		}
	}

	// bottom row
	if (m_FrameBottomHeight > 0)
	{
		int rowY = m_PosY + m_FrameTopHeight + middleHeight;

		if (m_FrameLeftWidth > 0)
		{
			m_Tiles.push_back(ImageTile(m_PosX, rowY, m_FrameLeftWidth, m_FrameBottomHeight, ImageTile::TILE_BOTTOM_LEFT));
		}

		if (m_FrameRightWidth > 0)
		{
			m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + middleWidth, rowY, m_FrameRightWidth, m_FrameBottomHeight, ImageTile::TILE_BOTTOM_RIGHT));
		}

		if (middleTileWidth > 0)
		{
			for (int col = 0; col < numMiddleCols; col++)
			{
				m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + col * middleTileWidth, rowY, middleTileWidth, m_FrameBottomHeight, ImageTile::TILE_BOTTOM_MIDDLE));
			}
		}
	}

	// middle rows
	if (middleTileHeight > 0)
	{
		for (int row = 0; row < numMiddleRows; row++)
		{
			int rowY = m_PosY + m_FrameTopHeight + row * middleTileHeight;

			if (m_FrameLeftWidth > 0)
			{
				m_Tiles.push_back(ImageTile(m_PosX, rowY, m_FrameLeftWidth, middleTileHeight, ImageTile::TILE_MIDDLE_LEFT));
			}

			if (m_FrameRightWidth > 0)
			{
				m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + middleWidth, rowY, m_FrameRightWidth, middleTileHeight, ImageTile::TILE_MIDDLE_RIGHT));
			}

			if (m_Type == IMAGE_TILED && middleTileWidth > 0)
			{
				for (int col = 0; col < numMiddleCols; col++)
				{
					m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth + col * middleTileWidth, rowY, middleTileWidth, middleTileHeight, ImageTile::TILE_MIDDLE_MIDDLE));
				}
			}
		}
	}

	// non-tiled middle part
	if (m_Type == IMAGE_TILED_FRAME)
	{
		m_Tiles.push_back(ImageTile(m_PosX + m_FrameLeftWidth, m_PosY + m_FrameTopHeight, middleWidth, middleHeight, ImageTile::TILE_MIDDLE_MIDDLE));
	}
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::FillTiles(Viewport* viewport, bool absolutePos)
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	Ogre::uint32 color;
	rs->convertColourValue(m_Color, &color); // convert to platform native format

	SpriteVertex* verts = reinterpret_cast<SpriteVertex*>(m_HardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	int vertNum = 0;
	foreach (ImageTile tile, m_Tiles)
	{
		// position
		verts[6 * vertNum + 1].pos = Ogre::Vector3(tile.m_Rect.left,  tile.m_Rect.bottom, -1.0f); // lower left
		verts[6 * vertNum + 0].pos = Ogre::Vector3(tile.m_Rect.left,  tile.m_Rect.top,    -1.0f); // top left
		verts[6 * vertNum + 5].pos = Ogre::Vector3(tile.m_Rect.right, tile.m_Rect.bottom, -1.0f); // bottom right
		verts[6 * vertNum + 2].pos = Ogre::Vector3(tile.m_Rect.right, tile.m_Rect.top,    -1.0f); // top right

		// texture coordinates
		Ogre::FloatRect rect;
		GetTextureCoordinates(tile.m_Type, rect);

		verts[6 * vertNum + 1].texCoord.x = rect.left;	verts[6 * vertNum + 1].texCoord.y = rect.bottom; // lower left
		verts[6 * vertNum + 0].texCoord.x = rect.left;	verts[6 * vertNum + 0].texCoord.y = rect.top;    // top left
		verts[6 * vertNum + 5].texCoord.x = rect.right;	verts[6 * vertNum + 5].texCoord.y = rect.bottom; // bottom right
		verts[6 * vertNum + 2].texCoord.x = rect.right;	verts[6 * vertNum + 2].texCoord.y = rect.top;    // top right

		// color
		for (int i = 0; i < 6; i++)
		{
			verts[6 * vertNum + i].color = color;
		}


		// copy vertices for the second triangle
		verts[6 * vertNum + 3].pos = verts[6 * vertNum + 2].pos;
		verts[6 * vertNum + 4].pos = verts[6 * vertNum + 1].pos;

		verts[6 * vertNum + 3].texCoord = verts[6 * vertNum + 2].texCoord;
		verts[6 * vertNum + 4].texCoord = verts[6 * vertNum + 1].texCoord;

		vertNum++;
	}

	// transform to ogre screen space
	viewport->TransformVertices(verts, m_Tiles.size() * 6, absolutePos);

	m_HardwareBuffer->unlock();
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::GetTextureCoordinates(ImageTile::TileType type, Ogre::FloatRect& rect)
{
	float leftColLeft = 0;
	float leftColRight = (float)m_FrameLeftWidth;
	
	float rightColLeft = (float)(m_Texture->GetWidth() - m_FrameRightWidth);
	float rightColRight = (float)m_Texture->GetWidth();

	float topRowTop = 0;
	float topRowBottom = (float)m_FrameTopHeight;

	float bottomRowTop = (float)(m_Texture->GetHeight() - m_FrameBottomHeight);
	float bottomRowBottom = (float)m_Texture->GetHeight();



	switch (type)
	{
	case ImageTile::TILE_ALL:
		rect.left = 0;
		rect.top = 0;
		rect.right = (float)m_Texture->GetWidth();
		rect.bottom = (float)m_Texture->GetHeight();
		break;

	case ImageTile::TILE_TOP_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case ImageTile::TILE_TOP_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case ImageTile::TILE_TOP_RIGHT:
		rect.left = rightColLeft;
		rect.right = rightColRight;
		rect.top = topRowTop;		
		rect.bottom = topRowBottom;
		break;

	case ImageTile::TILE_MIDDLE_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case ImageTile::TILE_MIDDLE_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case ImageTile::TILE_MIDDLE_RIGHT:
		rect.left = rightColLeft;
		rect.right = rightColRight;
		rect.top = topRowBottom;		
		rect.bottom = bottomRowTop;
		break;

	case ImageTile::TILE_BOTTOM_LEFT:
		rect.left = leftColLeft;
		rect.right = leftColRight;
		rect.top = bottomRowTop;		
		rect.bottom = bottomRowBottom;
		break;

	case ImageTile::TILE_BOTTOM_MIDDLE:
		rect.left = leftColRight;
		rect.right = rightColLeft;
		rect.top = bottomRowTop;		
		rect.bottom = bottomRowBottom;
		break;

	case ImageTile::TILE_BOTTOM_RIGHT:
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
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool ResizableImage::LoadFromXml(TiXmlElement* rootNode)
{
	WideString imageName = L"";
	Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Image")
		{
			imageName = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "ColorKey")
		{
			colorKey = XmlUtil::GetColor(elem);
		}
		else if (elem->ValueStr() == "FrameSize")
		{
			m_FrameLeftWidth = XmlUtil::AttrToSize(elem, "Left", 0);
			m_FrameRightWidth = XmlUtil::AttrToSize(elem, "Right", 0);
			m_FrameTopHeight = XmlUtil::AttrToSize(elem, "Top", 0);
			m_FrameBottomHeight = XmlUtil::AttrToSize(elem, "Bottom", 0);
		}
		else if (elem->ValueStr() == "Type")
		{
			Utf8String strType = XmlUtil::TextToUtf8(elem);
			if (strType == "Tiled") m_Type = IMAGE_TILED;
			else if (strType == "TiledFrame") m_Type = IMAGE_TILED_FRAME;
			else m_Type = IMAGE_STRETCHED;
		}
	}
	
	if (imageName.empty() || !SetTexture(imageName, colorKey)) return false;

	m_IsDirty = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ResizableImage::SaveToXml(TiXmlElement* rootNode)
{
	if (!m_Texture) return false;

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Image", rootNode);
	XmlUtil::SetFileName(elem, m_Texture->GetFileName());

	if (m_Texture->GetColorKey() != Ogre::ColourValue::ZERO)
	{
		elem = XmlUtil::AddElem("ColorKey", rootNode);
		XmlUtil::SetColor(elem, m_Texture->GetColorKey());
	}

	elem = XmlUtil::AddElem("FrameSize", rootNode);
	XmlUtil::SetAttr(elem, "Left", m_FrameLeftWidth);
	XmlUtil::SetAttr(elem, "Right", m_FrameRightWidth);
	XmlUtil::SetAttr(elem, "Top", m_FrameTopHeight);
	XmlUtil::SetAttr(elem, "Bottom", m_FrameBottomHeight);


	elem = XmlUtil::AddElem("Type", rootNode);
	switch (m_Type)
	{
	case IMAGE_STRETCHED:
		XmlUtil::SetText(elem, "Stretched");
		break;
	case IMAGE_TILED:
		XmlUtil::SetText(elem, "Tiled");
		break;
	case IMAGE_TILED_FRAME:
		XmlUtil::SetText(elem, "TiledFrame");
		break;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// IRenderableProvider
//////////////////////////////////////////////////////////////////////////
void ResizableImage::GetRenderables(IRenderableProvider::RenderableList& renderableList)
{
	if (m_Texture) renderableList.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Renderable implementation
//////////////////////////////////////////////////////////////////////////
const Ogre::MaterialPtr& ResizableImage::getMaterial(void) const
{
	return m_Texture->GetMaterial();
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::getRenderOperation(Ogre::RenderOperation& op)
{
	op = m_RenderOperation;
	op.vertexData->vertexStart = 0;
	op.vertexData->vertexCount = m_Tiles.size() * 6;
}

//////////////////////////////////////////////////////////////////////////
void ResizableImage::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::LightList& ResizableImage::getLights(void) const
{
	// N/A
	static Ogre::LightList ll;
	return ll;
}


} // namespace Wme

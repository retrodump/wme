// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TextureElement2D.h"
#include "SceneNode2D.h"
#include "SpriteSubFrame.h"
#include "SpriteTexture.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TextureElement2D::TextureElement2D()
{
	m_SubFrame = NULL;
	m_Color = Ogre::ColourValue::White;
}

//////////////////////////////////////////////////////////////////////////
TextureElement2D::~TextureElement2D()
{
}

//////////////////////////////////////////////////////////////////////////
void TextureElement2D::AddGeometry()
{
	if (!m_SubFrame) return;

	const SpriteTexture* texture = m_SubFrame->GetTexture();
	if (!texture) return;

	Ogre::Vector2 basePoint = Ogre::Vector2(m_SubFrame->GetBasePoint().x * texture->GetWidth(), m_SubFrame->GetBasePoint().y * texture->GetHeight());

	
	// texture coordinates
	float texStartX, texStartY, texEndX, texEndY;

	if (m_SubFrame->GetMirrorHorizontal())
	{
		texStartX = 1.0;
		texEndX = 0.0;
	}
	else
	{
		texStartX = 0.0;
		texEndX = 1.0;
	}

	if (m_SubFrame->GetMirrorVertical())
	{
		texStartY = 1.0;
		texEndY = 0.0;
	}
	else
	{
		texStartY = 0.0;
		texEndY = 1.0;
	}


	// fill vertices
	Vertex2DTex verts[4];

	// top left
	verts[1].pos = Ogre::Vector2(- basePoint.x, - basePoint.y);
	verts[1].texCoord = Ogre::Vector2(texStartX, texStartY);

	// top right
	verts[3].pos = Ogre::Vector2(- basePoint.x + texture->GetWidth(), - basePoint.y);
	verts[3].texCoord = Ogre::Vector2(texEndX, texStartY);

	// bottom left
	verts[0].pos = Ogre::Vector2(- basePoint.x, - basePoint.y + texture->GetHeight());
	verts[0].texCoord = Ogre::Vector2(texStartX, texEndY);

	// bottom right
	verts[2].pos = Ogre::Vector2(- basePoint.x + texture->GetWidth(), - basePoint.y + texture->GetHeight());
	verts[2].texCoord = Ogre::Vector2(texEndX, texEndY);

	// color
	for (int i = 0; i < 4; i++)
	{
		verts[i].color = m_Color;
	}

	m_ParentNode->AddGeometry(verts, 4, texture->GetMaterial(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
}

//////////////////////////////////////////////////////////////////////////
bool TextureElement2D::IsTransparentAt(float x, float y)
{
	if (!m_SubFrame) return true;

	const SpriteTexture* texture = m_SubFrame->GetTexture();
	if (!texture) return true;

	Ogre::Vector2 basePoint = Ogre::Vector2(m_SubFrame->GetBasePoint().x * texture->GetWidth(), m_SubFrame->GetBasePoint().y * texture->GetHeight());
	return m_SubFrame->IsTransparentAt(static_cast<int>(x + basePoint.x), static_cast<int>(y + basePoint.y));
}


} // namespace Wme

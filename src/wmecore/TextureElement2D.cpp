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

	Vertex2DTex verts[4];

	// top left
	verts[1].pos = Ogre::Vector2(- basePoint.x, - basePoint.y);
	verts[1].texCoord = Ogre::Vector2(0, 0);

	// top right
	verts[3].pos = Ogre::Vector2(- basePoint.x + texture->GetWidth(), - basePoint.y);
	verts[3].texCoord = Ogre::Vector2(1, 0);

	// bottom left
	verts[0].pos = Ogre::Vector2(- basePoint.x, - basePoint.y + texture->GetHeight());
	verts[0].texCoord = Ogre::Vector2(0, 1);

	// bottom right
	verts[2].pos = Ogre::Vector2(- basePoint.x + texture->GetWidth(), - basePoint.y + texture->GetHeight());
	verts[2].texCoord = Ogre::Vector2(1, 1);

	m_ParentNode->AddGeometry(verts, 4, texture->GetMaterial(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
}

//////////////////////////////////////////////////////////////////////////
bool TextureElement2D::IsTransparentAt(float x, float y)
{
	if (!m_SubFrame) return true;
	else return m_SubFrame->IsTransparentAt(static_cast<int>(x), static_cast<int>(y));
}


} // namespace Wme

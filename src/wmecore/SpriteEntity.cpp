// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteEntity.h"
#include "Sprite.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SpriteEntity::SpriteEntity()
{
	m_CurrentSprite = NULL;
}

//////////////////////////////////////////////////////////////////////////
SpriteEntity::~SpriteEntity()
{
}

//////////////////////////////////////////////////////////////////////////
void SpriteEntity::SetCurrentSprite(Sprite* currentSprite)
{
	if (currentSprite == m_CurrentSprite) return;

	if (m_CurrentSprite) m_CurrentSprite->RemoveListener(this);
	
	m_CurrentSprite = currentSprite;
	if (m_CurrentSprite) m_CurrentSprite->AddListener(this);
}

//////////////////////////////////////////////////////////////////////////
void SpriteEntity::OnSpriteChanged()
{

}


} // namespace Wme

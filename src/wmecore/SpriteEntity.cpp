// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteEntity.h"
#include "Sprite.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SpriteEntity::SpriteEntity()
{
	m_CurrentSprite = NULL;
	m_SpriteNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
SpriteEntity::~SpriteEntity()
{
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void SpriteEntity::PutToStage(Scene2DBase* stage, Entity2DBase* parentEntity)
{
	Entity2DBase::PutToStage(stage, parentEntity);
	m_SpriteNode = m_SceneNode->CreateChildNode();
}

//////////////////////////////////////////////////////////////////////////
void SpriteEntity::RemoveFromStage()
{
	if (m_SpriteNode)
	{
		m_SceneNode->RemoveAndDestroyChild(m_SpriteNode, true);
		m_SpriteNode = NULL;
	}
	Entity2DBase::RemoveFromStage();
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
	if (m_CurrentSprite && IsInStage()) m_CurrentSprite->AttachToSceneNode(m_SpriteNode);
}

//////////////////////////////////////////////////////////////////////////
void SpriteEntity::Update()
{
	/*
	static unsigned long prevTime = 0;
	if (Game::GetInstance()->GetDefaultTier()->GetCurrentTime() - prevTime > 1000)
	{
		prevTime = Game::GetInstance()->GetDefaultTier()->GetCurrentTime();
		//SetRotation(GetRotation().valueDegrees() + 10);
		SetPosition(GetPosition().x + 10, GetPosition().y);
	}
	*/

	if (m_CurrentSprite) m_CurrentSprite->Update();
}


} // namespace Wme

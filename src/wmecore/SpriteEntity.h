// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteEntity_H__
#define __WmeSpriteEntity_H__


#include "Entity2DBase.h"
#include "Sprite.h"

namespace Wme
{
	class WmeDllExport SpriteEntity : public Entity2DBase, public Sprite::Listener
	{
	public:
		SpriteEntity();
		virtual ~SpriteEntity();

		virtual void PutToStage(Scene2DBase* stage, Entity2DBase* parentEntity = NULL);
		virtual void RemoveFromStage();

		Sprite* GetCurrentSprite() const { return m_CurrentSprite; }
		void SetCurrentSprite(Sprite* currentSprite);

		virtual void Update();

		// Sprite::Listener
		void OnSpriteChanged();

		// temp - remove later, this class should be abstract
		RTTI(SpriteEntity);

	protected:
		Sprite* m_CurrentSprite;
		SceneNode2D* m_SpriteNode;
	};
}

#endif // __WmeSpriteEntity_H__

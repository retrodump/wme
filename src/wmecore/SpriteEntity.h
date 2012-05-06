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

		Sprite* GetCurrentSprite() const { return m_CurrentSprite; }
		void SetCurrentSprite(Sprite* currentSprite);

		// Sprite::Listener
		void OnSpriteChanged();

	protected:
		Sprite* m_CurrentSprite;
	};
}

#endif // __WmeSpriteEntity_H__

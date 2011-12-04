// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteManager_H__
#define __WmeSpriteManager_H__

#include "Object.h"
#include "Sprite.h"

namespace Wme
{
	class SpriteTexture;

	class WmeDllExport SpriteManager : public Object
	{
	public:
		SpriteManager();
		virtual ~SpriteManager();
		
		void xxDisplaySprite(int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params);

		Ogre::RenderOperation* xxRenderSprite(int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params);

		void PrepareSpriteVertices(SpriteVertex* verts, int targetX, int targetY, const SpriteSubFrame* subFrame, const SpriteDrawingParams& params);
		Ogre::RenderOperation* RenderSubFrame(const SpriteSubFrame* subFrame);

	private:
		void xxSetupRenderSystem();
		bool Initialize();
		void CreateSpriteHWBuffer();
		void DestroySpriteHWBuffer();
		Ogre::HardwareVertexBufferSharedPtr m_HardwareBuffer;
		Ogre::RenderOperation m_RenderOperation;
		bool m_Initialized;
	};
}

#endif // __WmeSpriteManager_H__
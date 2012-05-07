// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTextureElement2D_H__
#define __WmeTextureElement2D_H__


#include "Element2D.h"


namespace Wme
{
	class SpriteSubFrame;

	class WmeDllExport TextureElement2D : public Element2D
	{
	public:
		TextureElement2D();
		virtual ~TextureElement2D();

		void AddGeometry();
		bool IsTransparentAt(float x, float y);

		void SetSubFrame(SpriteSubFrame* subFrame) { m_SubFrame = subFrame; }
		SpriteSubFrame* GetSubFrame() const { return m_SubFrame; }

		void SetColor(const Ogre::ColourValue& color) { m_Color = color; }
		Ogre::ColourValue GetColor() const { return m_Color; }

	private:
		SpriteSubFrame* m_SubFrame;
		Ogre::ColourValue m_Color;
	};
}

#endif // __WmeTextureElement2D_H__
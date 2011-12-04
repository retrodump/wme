// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteTexture_H__
#define __WmeSpriteTexture_H__

#include "Object.h"

namespace Wme
{
	class WmeDllExport SpriteTexture : public Object
	{
	public:
		SpriteTexture();
		virtual ~SpriteTexture();

		const Ogre::TexturePtr GetTexture() const { return m_Texture; }
		size_t GetWidth() const { return m_Width; }
		size_t GetHeight() const { return m_Height; }
		Ogre::Real GetTexCoordU() const;
		Ogre::Real GetTexCoordV() const;

		const WideString& GetFileName() const { return m_FileName; }
		const Ogre::ColourValue& GetColorKey() const { return m_ColorKey; }

		bool SetTexture(const WideString& fileName, Ogre::ColourValue colorKey);
		const Ogre::MaterialPtr& GetMaterial() const { return m_Material; }

		bool IsTransparentAt(int posX, int posY);

	private:
		void DestroyMaterial();

		Ogre::ColourValue m_ColorKey;
		size_t m_Width;
		size_t m_Height;
		size_t m_RealWidth;
		size_t m_RealHeight;
		Ogre::TexturePtr m_Texture;
		WideString m_FileName;

		Ogre::MaterialPtr m_Material;
	};
}


#endif // __WmeSpriteTexture_H__
// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeVideoClip_H__
#define __WmeVideoClip_H__


#include "Object.h"


class TheoraVideoClip;

namespace Wme
{
	class WmeDllExport VideoClip : public Object
	{
	public:
		VideoClip();
		~VideoClip();

		bool Initialize(const WideString& fileName, bool looping);
		void Update();

		WideString GetFileName() const { return m_FileName; }
		Ogre::TexturePtr GetTexture() const { return m_Texture; }
		const Ogre::MaterialPtr& GetMaterial() const { return m_Material; }

		void AttachToMaterial(const WideString& matName, word techniqueLevel = 0, word passLevel = 0, word stateLevel = 0);
		void AttachToMaterial(Ogre::MaterialPtr& material, word techniqueLevel = 0, word passLevel = 0, word stateLevel = 0);

		bool GetLooping() const { return m_Looping; }
		void SetLooping(bool val);

	private:
		void Cleanup();

		bool m_Looping;
		WideString m_FileName;
		Ogre::TexturePtr m_Texture;
		Ogre::MaterialPtr m_Material;
		TheoraVideoClip* m_TheoraClip;
	};
}

#endif // __WmeVideoClip_H__

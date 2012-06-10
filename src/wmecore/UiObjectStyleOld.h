// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectStyleOld_H__
#define __WmeUiObjectStyleOld_H__


#include "Object.h"


namespace Wme
{
	class Sprite;
	class ResizableImageOld;
	class Font;
	class UiObjectOld;

	class UiObjectStyleOld : public Object
	{
	public:
		UiObjectStyleOld(UiObjectOld* owner);
		~UiObjectStyleOld();

		Sprite* GetSprite() const { return m_Sprite; }
		void SetSprite(Sprite* sprite);

		ResizableImageOld* GetImage() const { return m_Image; }
		void SetImage(ResizableImageOld* image);

		Font* GetFont() const { return m_Font; }
		void SetFont(Font* font);

		Ogre::ColourValue GetTextColor() const { return m_TextColor; }
		void SetTextColor(const Ogre::ColourValue& val) { m_TextColor = val; }


		bool LoadFromXml(TiXmlElement* node);
		bool SaveToXml(TiXmlElement* node);

	private:
		bool LoadSprite(const WideString& fileName);
		bool LoadFont(const WideString& fileName);

		UiObjectOld* m_Owner;

		Sprite* m_Sprite;
		ResizableImageOld* m_Image;
		Font* m_Font;
		Ogre::ColourValue m_TextColor;
	};
}

#endif // __WmeUiObjectStyleOld_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectStyle_H__
#define __WmeUiObjectStyle_H__


#include "Object.h"


namespace Wme
{
	class Sprite;
	class ResizableImage;
	class Font;
	class UiObject;

	class UiObjectStyle : public Object
	{
	public:
		UiObjectStyle(UiObject* owner);
		~UiObjectStyle();

		Sprite* GetSprite() const { return m_Sprite; }
		void SetSprite(Sprite* sprite);

		ResizableImage* GetImage() const { return m_Image; }
		void SetImage(ResizableImage* image);

		Font* GetFont() const { return m_Font; }
		void SetFont(Font* font);

		Ogre::ColourValue GetTextColor() const { return m_TextColor; }
		void SetTextColor(const Ogre::ColourValue& val) { m_TextColor = val; }


		bool LoadFromXml(TiXmlElement* node);
		bool SaveToXml(TiXmlElement* node);

	private:
		bool LoadSprite(const WideString& fileName);
		bool LoadFont(const WideString& fileName);

		UiObject* m_Owner;

		Sprite* m_Sprite;
		ResizableImage* m_Image;
		Font* m_Font;
		Ogre::ColourValue m_TextColor;
	};
}

#endif // __WmeUiObjectStyle_H__

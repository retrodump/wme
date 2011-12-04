// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePropertyColor_H__
#define __WmePropertyColor_H__


#include "GenericProperty.h"
#include "XmlUtil.h"


namespace Wme
{
	class PropertyColor;

	//////////////////////////////////////////////////////////////////////////
	class PropertyValueColor : public GenericPropertyValue
	{
	public:
		PropertyValueColor(PropertyColor* parentProp);
		~PropertyValueColor();

		Ogre::ColourValue GetColor() const { return m_Color; }
		void SetColor(const Ogre::ColourValue& color) { m_Color = color; }

		bool operator== (const GenericPropertyValue& val) const;
		GenericPropertyValue& operator= (const GenericPropertyValue& val);

		WideString ToString() const;

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		PropertyColor* GetParentProp() const { return m_ParentProp; }

	private:
		PropertyColor* m_ParentProp;

		Ogre::ColourValue m_Color;
	};

	//////////////////////////////////////////////////////////////////////////
	class PropertyColor : public GenericProperty
	{
	public:
		PropertyColor()
		{
			m_Type = L"color";
			m_IncludeAlpha = true;
		}
		~PropertyColor() {};

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		bool GetIncludeAlpha() const { return m_IncludeAlpha; }
		void SetIncludeAlpha(bool includeAlpha) { m_IncludeAlpha = includeAlpha; }

		GenericPropertyValue* CreateValue() { return new PropertyValueColor(this); }

	private:
		bool m_IncludeAlpha;
	};
}


#endif // __WmePropertyColor_H__
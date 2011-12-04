// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePropertyTexture_H__
#define __WmePropertyTexture_H__


#include "GenericProperty.h"
#include "XmlUtil.h"


namespace Wme
{
	class PropertyTexture;

	//////////////////////////////////////////////////////////////////////////
	class PropertyValueTexture : public GenericPropertyValue
	{
	public:
		PropertyValueTexture(PropertyTexture* parentProp);
		~PropertyValueTexture();

		WideString GetFileName() const { return m_FileName; }
		void SetFileName(const WideString& fileName) { m_FileName = fileName; }

		bool operator== (const GenericPropertyValue& val) const;
		GenericPropertyValue& operator= (const GenericPropertyValue& val);

		WideString ToString() const;

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		PropertyTexture* GetParentProp() const { return m_ParentProp; }

	private:
		PropertyTexture* m_ParentProp;

		WideString m_FileName;
	};

	//////////////////////////////////////////////////////////////////////////
	class PropertyTexture : public GenericProperty
	{
	public:
		PropertyTexture()
		{
			m_Type = L"texture";
		}
		~PropertyTexture() {};

		GenericPropertyValue* CreateValue() { return new PropertyValueTexture(this); }

	private:
	};
}


#endif // __WmePropertyTexture_H__

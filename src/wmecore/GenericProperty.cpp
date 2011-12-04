// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GenericProperty.h"
#include "XmlUtil.h"
#include "PropertyColor.h"
#include "PropertyFloat.h"
#include "PropertyTexture.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
GenericProperty::GenericProperty()
{
	m_DefaultValue = NULL;
}

//////////////////////////////////////////////////////////////////////////
GenericProperty::~GenericProperty()
{
	SAFE_DELETE(m_DefaultValue);
}

//////////////////////////////////////////////////////////////////////////
void GenericProperty::SetDefaultValue(GenericPropertyValue* value)
{
	SAFE_DELETE(m_DefaultValue);
	m_DefaultValue = value;
}

//////////////////////////////////////////////////////////////////////////
bool GenericProperty::IsDefaultValue(const GenericPropertyValue* value)
{
	if (!value || !m_DefaultValue) return false;
	return (*value == *m_DefaultValue);
}

//////////////////////////////////////////////////////////////////////////
bool GenericProperty::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Name")
		{
			m_Name = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "Label")
		{
			m_Label = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "DefaultValue")
		{
			SAFE_DELETE(m_DefaultValue); 
			m_DefaultValue = CreateValue();
			m_DefaultValue->LoadFromXml(elem);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool GenericProperty::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Name", rootNode);
	XmlUtil::SetText(elem, m_Name);

	if (!m_Label.empty())
	{
		elem = XmlUtil::AddElem("Label", rootNode);
		XmlUtil::SetText(elem, m_Label);
	}

	if (m_DefaultValue)
	{
		elem = XmlUtil::AddElem("DefaultValue", rootNode);
		m_DefaultValue->SaveToXml(elem);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool GenericProperty::LoadPropertiesFromXml(TiXmlElement* propsNode, GenericPropertyList& properties)
{
	for (TiXmlElement* elem = propsNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Property")
		{
			WideString propType = XmlUtil::AttrToString(elem, "Type");
			GenericProperty* property = GenericProperty::CreateProperty(propType);
			if (property)
			{
				if (property->LoadFromXml(elem)) properties.push_back(property);
				else delete property;
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool GenericProperty::SavePropertiesToXml(TiXmlElement* propsNode, const GenericPropertyList& properties)
{
	foreach (GenericProperty* property, properties)
	{
		TiXmlElement* elem;

		elem = XmlUtil::AddElem("Property", propsNode);
		XmlUtil::SetAttr(elem, "Type", property->GetType());

		property->SaveToXml(elem);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
GenericProperty* GenericProperty::CreateProperty(const WideString& type)
{
	if (type == L"color") return new PropertyColor;
	else if (type == L"float") return new PropertyFloat;
	else if (type == L"texture") return new PropertyTexture;
	else return NULL;
}


} // namespace Wme

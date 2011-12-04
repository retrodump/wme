// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Material.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Material::Material()
{
}

//////////////////////////////////////////////////////////////////////////
Material::~Material()
{
	foreach (GenericProperty* prop, m_Properties)
	{
		SAFE_DELETE(prop);
	}
	m_Properties.clear();
}

//////////////////////////////////////////////////////////////////////////
bool Material::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Name")
		{
			m_Name = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "MaterialScript")
		{
			m_MaterialScriptFile = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "BaseMaterialName")
		{
			m_BaseMaterialName = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "Properties")
		{
			GenericProperty::LoadPropertiesFromXml(elem, m_Properties);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Material::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Name", rootNode);
	XmlUtil::SetText(elem, m_Name);

	elem = XmlUtil::AddElem("MaterialScript", rootNode);
	XmlUtil::SetFileName(elem, m_MaterialScriptFile);

	elem = XmlUtil::AddElem("BaseMaterialName", rootNode);
	XmlUtil::SetText(elem, m_BaseMaterialName);

	elem = XmlUtil::AddElem("Properties", rootNode);
	GenericProperty::SavePropertiesToXml(elem, m_Properties);


	return true;
}

//////////////////////////////////////////////////////////////////////////
void Material::GeneratePropValues(GenericPropertyValues& values)
{
	foreach (GenericPropertyValues::value_type val, values)
	{
		SAFE_DELETE(val.second);
	}
	values.clear();

	foreach (GenericProperty* prop, m_Properties)
	{
		GenericPropertyValue* val = prop->CreateValue();
		if (prop->GetDefaultValue()) *val = *prop->GetDefaultValue();

		values[prop->GetName()] = val;
	}
}


} // namespace Wme

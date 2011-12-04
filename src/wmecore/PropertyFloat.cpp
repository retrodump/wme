// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PropertyFloat.h"
#include "XmlUtil.h"
#include <boost/lexical_cast.hpp>


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
bool PropertyFloat::LoadFromXml(TiXmlElement* rootNode)
{
	GenericProperty::LoadFromXml(rootNode);


	bool hasMin = false;
	bool hasMax = false;

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "MinValue")
		{
			m_MinValue = XmlUtil::TextToFloat(elem);
			hasMin = true;
		}
		else if (elem->ValueStr() == "MaxValue")
		{
			m_MaxValue = XmlUtil::TextToFloat(elem);
			hasMax = true;
		}
		else if (elem->ValueStr() == "Decimals")
		{
			m_Decimals = (size_t)XmlUtil::TextToInt(elem);
		}
	}

	m_CheckRange = hasMin && hasMax;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyFloat::SaveToXml(TiXmlElement* rootNode)
{
	GenericProperty::SaveToXml(rootNode);

	TiXmlElement* elem;

	if (m_CheckRange)
	{
		elem = XmlUtil::AddElem("MinValue", rootNode);
		XmlUtil::SetText(elem, m_MinValue);

		elem = XmlUtil::AddElem("MaxValue", rootNode);
		XmlUtil::SetText(elem, m_MaxValue);

		elem = XmlUtil::AddElem("Decimals", rootNode);
		XmlUtil::SetText(elem, (int)m_Decimals);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PropertyValueFloat::PropertyValueFloat(PropertyFloat* parentProp)
{
	m_ParentProp = parentProp;

	m_Value = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
PropertyValueFloat::~PropertyValueFloat()
{

}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueFloat::operator== (const GenericPropertyValue& val) const
{
	const PropertyValueFloat& floatVal = static_cast<const PropertyValueFloat&>(val);

	if (floatVal.GetValue() != this->GetValue()) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
GenericPropertyValue& PropertyValueFloat::operator= (const GenericPropertyValue& val)
{
	const PropertyValueFloat& floatVal = static_cast<const PropertyValueFloat&>(val);
	SetValue(floatVal.GetValue());

	return *this;
}

//////////////////////////////////////////////////////////////////////////
WideString PropertyValueFloat::ToString() const
{
	return boost::lexical_cast<WideString>(m_Value);
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueFloat::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Value")
		{
			m_Value = XmlUtil::TextToFloat(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueFloat::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem = XmlUtil::AddElem("Value", rootNode);
	XmlUtil::SetText(elem, m_Value);

	return true;
}


} // namespace Wme
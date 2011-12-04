// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PropertyColor.h"
#include "XmlUtil.h"
#include <boost/lexical_cast.hpp>


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
bool PropertyColor::LoadFromXml(TiXmlElement* rootNode)
{
	GenericProperty::LoadFromXml(rootNode);
	
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "IncludeAlpha")
		{
			m_IncludeAlpha = XmlUtil::TextToBool(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyColor::SaveToXml(TiXmlElement* rootNode)
{
	GenericProperty::SaveToXml(rootNode);

	TiXmlElement* elem;
	elem = XmlUtil::AddElem("IncludeAlpha", rootNode);
	XmlUtil::SetText(elem, m_IncludeAlpha);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PropertyValueColor::PropertyValueColor(PropertyColor* parentProp)
{
	m_ParentProp = parentProp;

	m_Color = Ogre::ColourValue::White;
}

//////////////////////////////////////////////////////////////////////////
PropertyValueColor::~PropertyValueColor()
{

}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueColor::operator== (const GenericPropertyValue& val) const
{
	const PropertyValueColor& colorVal = static_cast<const PropertyValueColor&>(val);
	
	if (colorVal.GetColor() != this->GetColor()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
GenericPropertyValue& PropertyValueColor::operator= (const GenericPropertyValue& val)
{
	const PropertyValueColor& colorVal = static_cast<const PropertyValueColor&>(val);
	SetColor(colorVal.GetColor());

	return *this;
}

//////////////////////////////////////////////////////////////////////////
WideString PropertyValueColor::ToString() const
{
	WideString ret;

	ret = L"";
	ret += boost::lexical_cast<WideString>(m_Color.r) + L" " +
	ret += boost::lexical_cast<WideString>(m_Color.g) + L" " +
	ret += boost::lexical_cast<WideString>(m_Color.b);
	
	if (m_ParentProp->GetIncludeAlpha())
		ret += L" " + boost::lexical_cast<WideString>(m_Color.a);
	else
		ret += L" " + boost::lexical_cast<WideString>(1.0f);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueColor::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Color")
		{
			m_Color = XmlUtil::GetColor(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueColor::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem = XmlUtil::AddElem("Color", rootNode);
	XmlUtil::SetColor(elem, m_Color);

	return true;
}


} // namespace Wme

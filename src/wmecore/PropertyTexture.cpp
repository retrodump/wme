// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PropertyTexture.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PropertyValueTexture::PropertyValueTexture(PropertyTexture* parentProp)
{
	m_ParentProp = parentProp;
}

//////////////////////////////////////////////////////////////////////////
PropertyValueTexture::~PropertyValueTexture()
{

}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueTexture::operator== (const GenericPropertyValue& val) const
{
	const PropertyValueTexture& textureVal = static_cast<const PropertyValueTexture&>(val);
	
	if (textureVal.GetFileName() != this->GetFileName()) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
GenericPropertyValue& PropertyValueTexture::operator= (const GenericPropertyValue& val)
{
	const PropertyValueTexture& textureVal = static_cast<const PropertyValueTexture&>(val);
	SetFileName(textureVal.GetFileName());

	return *this;
}

//////////////////////////////////////////////////////////////////////////
WideString PropertyValueTexture::ToString() const
{
	return m_FileName;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueTexture::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Texture")
		{
			m_FileName = XmlUtil::TextToString(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PropertyValueTexture::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem = XmlUtil::AddElem("Texture", rootNode);
	XmlUtil::SetFileName(elem, m_FileName);

	return true;
}


} // namespace Wme
// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DocumentAwareObject.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
DocumentAwareObject::DocumentAwareObject()
{
}

//////////////////////////////////////////////////////////////////////////
DocumentAwareObject::~DocumentAwareObject()
{
}

//////////////////////////////////////////////////////////////////////////
bool DocumentAwareObject::LoadFromFile(const WideString& fileName)
{
	TiXmlDocument* doc = XmlUtil::OpenXmlFile(fileName, GetDocRootName());
	if (!doc) return false;

	bool ret = LoadFromXml(doc->RootElement());

	if (ret) m_FileName = fileName;

	delete doc;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool DocumentAwareObject::LoadFromString(const Utf8String& xmlString)
{
	TiXmlDocument* doc = XmlUtil::OpenXmlString(xmlString, GetDocRootName());
	if (!doc) return false;

	bool ret = LoadFromXml(doc->RootElement());

	if (ret) m_FileName = L"(memory)";

	delete doc;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool DocumentAwareObject::SaveToFile(const WideString& fileName)
{
	TiXmlDocument* doc;
	TiXmlElement* root;
	XmlUtil::CreateXmlDocument(GetDocRootName(), doc, root);

	bool ret = SaveToXml(root);
	if (ret) ret = XmlUtil::SaveXmlFile(fileName, doc);

	delete doc;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool DocumentAwareObject::SaveToString(Utf8String& xmlString)
{
	TiXmlDocument* doc;
	TiXmlElement* root;
	XmlUtil::CreateXmlDocument(GetDocRootName(), doc, root);

	bool ret = SaveToXml(root);
	if (ret) ret = XmlUtil::SaveXmlString(xmlString, doc);

	delete doc;
	return ret;
}


} // namespace Wme

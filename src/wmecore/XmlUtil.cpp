// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "XmlUtil.h"
#include "Game.h"
#include "FileManager.h"
#include "File.h"
#include "StringUtil.h"
#include <boost/lexical_cast.hpp>


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		pragma comment (lib, "tinyxmld_STL.lib")
#	else
#		pragma comment (lib, "tinyxml_STL.lib")
#	endif
#endif


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
void XmlUtil::CreateXmlDocument(const AnsiString& rootNodeName, TiXmlDocument*& doc, TiXmlElement*& root)
{
	doc = new TiXmlDocument();
	doc->LinkEndChild(new TiXmlDeclaration( "1.0", "utf-8", ""));

	root = new TiXmlElement(rootNodeName);
	doc->LinkEndChild(root);

	WideString verString;
	verString = StringUtil::ToString(WME_VER_MAJOR) + L"." + StringUtil::ToString(WME_VER_MINOR) + L"." + StringUtil::ToString(WME_VER_PATCH);
	SetAttr(root, "GeneratorVersion", verString);
}

//////////////////////////////////////////////////////////////////////////
TiXmlDocument* XmlUtil::OpenXmlFile(const WideString& fileName, const Utf8String& checkRootNode)
{
	File* file = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!file)
	{
		Game::GetInstance()->Log(L"Error: File not found", fileName);
		return NULL;
	}

	char* buffer = new char[file->GetSize() + 1];
	file->Read(buffer, file->GetSize());
	buffer[file->GetSize()] = '\0';

	Game::GetInstance()->GetFileMgr()->CloseFile(file);

	TiXmlDocument* doc = new TiXmlDocument;
	if (!doc->Parse(buffer))
	{
		Game::GetInstance()->Log(L"Error parsing XML document", fileName);
		delete [] buffer;
		delete doc;
		return NULL;
	}

	delete [] buffer;

	if (!checkRootNode.empty())
	{
		TiXmlElement* rootElem = doc->RootElement();
		if (!rootElem || rootElem->ValueStr() != checkRootNode)
		{
			Game::GetInstance()->Log(L"Unexpected root node in XML document (expecting '" + StringUtil::Utf8ToWide(checkRootNode) + L"', found '" + (rootElem ? StringUtil::Utf8ToWide(rootElem->ValueStr()) : L"<none>") + L"')", fileName);
			delete doc;
			return NULL;
		}
	}

	return doc;
}

//////////////////////////////////////////////////////////////////////////
TiXmlDocument* XmlUtil::OpenXmlString(const Utf8String& xmlString, const Utf8String& checkRootNode)
{
	TiXmlDocument* doc = new TiXmlDocument;
	if (!doc->Parse(xmlString.c_str()))
	{
		Game::GetInstance()->Log(L"Error parsing XML string");
		delete doc;
		return NULL;
	}

	if (!checkRootNode.empty())
	{
		TiXmlElement* rootElem = doc->RootElement();
		if (!rootElem || rootElem->ValueStr() != checkRootNode)
		{
			Game::GetInstance()->Log(L"Unexpected root node in XML document (expecting '" + StringUtil::Utf8ToWide(checkRootNode) + L"', found '" + (rootElem ? StringUtil::Utf8ToWide(rootElem->ValueStr()) : L"<none>") + L"')");
			delete doc;
			return NULL;
		}
	}

	return doc;
}

//////////////////////////////////////////////////////////////////////////
bool XmlUtil::SaveXmlFile(const WideString& fileName, TiXmlDocument* doc)
{
	TiXmlPrinter printer;
	doc->Accept(&printer);

	std::ofstream stream;
#ifdef WIN32
	stream.open(fileName.c_str());
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str());
#endif
	if (!stream.is_open()) return false;
	else
	{
		stream << printer.Str();
		stream.close();

		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
bool XmlUtil::SaveXmlString(Utf8String& xmlString, TiXmlDocument* doc)
{
	TiXmlPrinter printer;
	doc->Accept(&printer);

	xmlString = printer.Str();
	return true;
}

//////////////////////////////////////////////////////////////////////////
TiXmlElement* XmlUtil::AddElem(const AnsiString& tagName, TiXmlElement* parentElem)
{
	TiXmlElement* elem = new TiXmlElement(tagName);
	if (parentElem != NULL) parentElem->LinkEndChild(elem);

	return elem;
}

//////////////////////////////////////////////////////////////////////////
WideString XmlUtil::TextToString(TiXmlElement* element)
{
	const char* str = element->GetText();
	if (str) return StringUtil::Utf8ToWide(str);
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
Utf8String XmlUtil::TextToUtf8(TiXmlElement* element)
{
	const char* str = element->GetText();
	if (str) return Utf8String(str);
	else return "";
}

//////////////////////////////////////////////////////////////////////////
float XmlUtil::TextToFloat(TiXmlElement* element, float initVal)
{
	try
	{
		return boost::lexical_cast<float>(element->GetText());
	}
	catch (boost::bad_lexical_cast&)
	{
		return initVal;
	}
}

//////////////////////////////////////////////////////////////////////////
int XmlUtil::TextToInt(TiXmlElement* element, int initVal)
{
	try
	{
		return boost::lexical_cast<int>(element->GetText());
	}
	catch (boost::bad_lexical_cast&)
	{
		return initVal;
	}
}

//////////////////////////////////////////////////////////////////////////
bool XmlUtil::TextToBool(TiXmlElement* element, bool initVal)
{
	WideString val = StringUtil::Utf8ToWide(element->GetText());
	if (val == L"true") return true;
	else if (val == L"false") return false;
	else return initVal;
}


//////////////////////////////////////////////////////////////////////////
WideString XmlUtil::AttrToString(TiXmlElement* element, const AnsiString& attrName, const WideString& initVal)
{
	const char* val = element->Attribute(attrName.c_str());
	if (val) return StringUtil::Utf8ToWide(val);
	else return initVal;
}

//////////////////////////////////////////////////////////////////////////
float XmlUtil::AttrToFloat(TiXmlElement* element, const AnsiString& attrName, float initVal)
{
	const char* val = element->Attribute(attrName.c_str());
	if (val)
	{
		try
		{
			return boost::lexical_cast<float>(val);
		}
		catch (boost::bad_lexical_cast&)
		{
			return initVal;
		}
	}
	else return initVal;
}

//////////////////////////////////////////////////////////////////////////
int XmlUtil::AttrToInt(TiXmlElement* element, const AnsiString& attrName, int initVal)
{
	const char* val = element->Attribute(attrName.c_str());
	if (val)
	{
		try
		{
			return boost::lexical_cast<int>(val);
		}
		catch (boost::bad_lexical_cast&)
		{
			return initVal;
		}
	}
	else return initVal;
}

//////////////////////////////////////////////////////////////////////////
size_t XmlUtil::AttrToSize(TiXmlElement* element, const AnsiString& attrName, size_t initVal)
{
	const char* val = element->Attribute(attrName.c_str());
	if (val)
	{
		try
		{
			return boost::lexical_cast<size_t>(val);
		}
		catch (boost::bad_lexical_cast&)
		{
			return initVal;
		}
	}
	else return initVal;
}

//////////////////////////////////////////////////////////////////////////
bool XmlUtil::AttrToBool(TiXmlElement* element, const AnsiString& attrName, bool initVal)
{
	const char* val = element->Attribute(attrName.c_str());
	if (val)
	{
		WideString valStr = StringUtil::Utf8ToWide(val);
		if (valStr == L"true") return true;
		else if (valStr == L"false") return false;
		else return initVal;
	}
	else return initVal;
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetText(TiXmlElement* element, const WideString& val, bool localizable)
{
	element->LinkEndChild(new TiXmlText(StringUtil::WideToUtf8(val)));
	if (localizable) SetAttr(element, "Localizable", true);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetText(TiXmlElement* element, const char* val)
{
	element->LinkEndChild(new TiXmlText(val));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetText(TiXmlElement* element, float val)
{
	element->LinkEndChild(new TiXmlText(boost::lexical_cast<AnsiString>(val)));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetText(TiXmlElement* element, int val)
{
	element->LinkEndChild(new TiXmlText(boost::lexical_cast<AnsiString>(val)));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetText(TiXmlElement* element, bool val)
{
	element->LinkEndChild(new TiXmlText(val ? "true" : "false"));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetFileName(TiXmlElement* element, const WideString& fileName)
{
	SetText(element, fileName);
	SetAttr(element, "Filename", true);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetAttr(TiXmlElement* element, const AnsiString& attrName, const WideString& val)
{
	element->SetAttribute(attrName, StringUtil::WideToUtf8(val));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetAttr(TiXmlElement* element, const AnsiString& attrName, float val)
{
	element->SetAttribute(attrName, boost::lexical_cast<AnsiString>(val));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetAttr(TiXmlElement* element, const AnsiString& attrName, int val)
{
	element->SetAttribute(attrName, boost::lexical_cast<AnsiString>(val));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetAttr(TiXmlElement* element, const AnsiString& attrName, size_t val)
{
	element->SetAttribute(attrName, boost::lexical_cast<AnsiString>(val));
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetAttr(TiXmlElement* element, const AnsiString& attrName, bool val)
{
	element->SetAttribute(attrName, val ? "true" : "false");
}


//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 XmlUtil::GetVector2(TiXmlElement* element)
{
	return Ogre::Vector2(
		AttrToFloat(element, "X"),
		AttrToFloat(element, "Y")
		);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 XmlUtil::GetVector3(TiXmlElement* element)
{
	return Ogre::Vector3(
		AttrToFloat(element, "X"),
		AttrToFloat(element, "Y"),
		AttrToFloat(element, "Z")
		);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Quaternion XmlUtil::GetQuaternion(TiXmlElement* element)
{
	return Ogre::Quaternion(
		AttrToFloat(element, "W"),
		AttrToFloat(element, "X"),
		AttrToFloat(element, "Y"),
		AttrToFloat(element, "Z")
		);
}


//////////////////////////////////////////////////////////////////////////
Ogre::ColourValue XmlUtil::GetColor(TiXmlElement* element)
{
	return Ogre::ColourValue(
		AttrToFloat(element, "R"),
		AttrToFloat(element, "G"),
		AttrToFloat(element, "B"),
		AttrToFloat(element, "A", 1.0)
		);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetVector2(TiXmlElement* element, const Ogre::Vector2& val)
{
	SetAttr(element, "X", val.x);
	SetAttr(element, "Y", val.y);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetVector3(TiXmlElement* element, const Ogre::Vector3& val)
{
	SetAttr(element, "X", val.x);
	SetAttr(element, "Y", val.y);
	SetAttr(element, "Z", val.z);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetQuaternion(TiXmlElement* element, const Ogre::Quaternion& val)
{
	SetAttr(element, "W", val.w);
	SetAttr(element, "X", val.x);
	SetAttr(element, "Y", val.y);
	SetAttr(element, "Z", val.z);
}

//////////////////////////////////////////////////////////////////////////
void XmlUtil::SetColor(TiXmlElement* element, const Ogre::ColourValue& val)
{
	SetAttr(element, "R", val.r);
	SetAttr(element, "G", val.g);
	SetAttr(element, "B", val.b);
	SetAttr(element, "A", val.a);
}


} // namespace Wme

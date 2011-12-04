// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeXmlUtil_H__
#define __WmeXmlUtil_H__

namespace Wme
{
	class WmeDllExport XmlUtil
	{
	public:
		static void CreateXmlDocument(const AnsiString& rootNodeName, TiXmlDocument*& doc, TiXmlElement*& root);

		static TiXmlDocument* OpenXmlFile(const WideString& fileName, const Utf8String& checkRootNode = "");
		static TiXmlDocument* OpenXmlString(const Utf8String& xmlString, const Utf8String& checkRootNode = "");
		static bool SaveXmlFile(const WideString& fileName, TiXmlDocument* doc);
		static bool SaveXmlString(Utf8String& xmlString, TiXmlDocument* doc);

		static TiXmlElement* AddElem(const AnsiString& tagName, TiXmlElement* parentElem = NULL);

		static WideString TextToString(TiXmlElement* element);
		static Utf8String TextToUtf8(TiXmlElement* element);
		static float TextToFloat(TiXmlElement* element, float initVal = 0.0);
		static int TextToInt(TiXmlElement* element, int initVal = 0);
		static bool TextToBool(TiXmlElement* element, bool initVal = false);

		static WideString AttrToString(TiXmlElement* element, const AnsiString& attrName, const WideString& initVal = L"");
		static float AttrToFloat(TiXmlElement* element, const AnsiString& attrName, float initVal = 0.0);
		static int AttrToInt(TiXmlElement* element, const AnsiString& attrName, int initVal = 0);
		static size_t AttrToSize(TiXmlElement* element, const AnsiString& attrName, size_t initVal = 0);
		static bool AttrToBool(TiXmlElement* element, const AnsiString& attrName, bool initVal = false);

		static void SetText(TiXmlElement* element, const WideString& val, bool localizable = false);
		static void SetText(TiXmlElement* element, const char* val);
		static void SetText(TiXmlElement* element, float val);
		static void SetText(TiXmlElement* element, int val);
		static void SetText(TiXmlElement* element, bool val);
		static void SetFileName(TiXmlElement* element, const WideString& fileName);

		static void SetAttr(TiXmlElement* element, const AnsiString& attrName, const WideString& val);
		static void SetAttr(TiXmlElement* element, const AnsiString& attrName, float val);
		static void SetAttr(TiXmlElement* element, const AnsiString& attrName, int val);
		static void SetAttr(TiXmlElement* element, const AnsiString& attrName, size_t val);
		static void SetAttr(TiXmlElement* element, const AnsiString& attrName, bool val);


		static Ogre::Vector2 GetVector2(TiXmlElement* element);
		static Ogre::Vector3 GetVector3(TiXmlElement* element);
		static Ogre::Quaternion GetQuaternion(TiXmlElement* element);
		static Ogre::ColourValue GetColor(TiXmlElement* element);

		static void SetVector2(TiXmlElement* element, const Ogre::Vector2& val);
		static void SetVector3(TiXmlElement* element, const Ogre::Vector3& val);
		static void SetQuaternion(TiXmlElement* element, const Ogre::Quaternion& val);
		static void SetColor(TiXmlElement* element, const Ogre::ColourValue& val);
	};
}


#endif // __WmeXmlUtil_H__
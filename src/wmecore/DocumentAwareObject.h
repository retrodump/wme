// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDocumentAwareObject_H__
#define __WmeDocumentAwareObject_H__


namespace Wme
{
	class WmeDllExport DocumentAwareObject
	{
	public:
		DocumentAwareObject();
		virtual ~DocumentAwareObject();
		
		WideString GetFileName() { return m_FileName; }

		virtual bool LoadFromFile(const WideString& fileName);
		virtual bool LoadFromString(const Utf8String& xmlString);
		virtual bool SaveToFile(const WideString& fileName);
		virtual bool SaveToString(Utf8String& xmlString);

		virtual bool LoadFromXml(TiXmlElement* rootNode) { return false; }
		virtual bool SaveToXml(TiXmlElement* rootNode) { return false; }
		virtual Utf8String GetDocRootName() { return ""; }

	protected:
		WideString m_FileName;
	};
}

#endif // __WmeDocumentAwareObject_H__
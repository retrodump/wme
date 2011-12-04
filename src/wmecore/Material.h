// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMaterial_H__
#define __WmeMaterial_H__


#include "DocumentAwareObject.h"
#include "RefCountObject.h"
#include "GenericProperty.h"


namespace Wme
{
	class WmeDllExport Material : public Object, public DocumentAwareObject, public RefCountObject
	{
	public:
		Material();
		~Material();

		WideString GetMaterialScriptFile() const { return m_MaterialScriptFile; }
		WideString GetBaseMaterialName() const { return m_BaseMaterialName; }
		WideString GetName() const { return m_Name; }
		const GenericPropertyList& GetProperties() const { return m_Properties; }

		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeMaterialDef"; }

		void GeneratePropValues(GenericPropertyValues& values);

	private:
		WideString m_Name;
		WideString m_MaterialScriptFile;
		WideString m_BaseMaterialName;

		GenericPropertyList m_Properties;
	};
}


#endif // __WmeMaterial_H__

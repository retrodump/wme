// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMaterialInstance_H__
#define __WmeMaterialInstance_H__


#include "DocumentAwareObject.h"
#include "RefCountObject.h"
#include "GenericProperty.h"
#include "OgreScriptCompiler.h"


namespace Wme
{
	class Material;

	class WmeDllExport MaterialInstance : public Object, public DocumentAwareObject, public RefCountObject, public Ogre::ScriptCompilerListener
	{
	public:
		MaterialInstance();
		~MaterialInstance();

		Ogre::MaterialPtr GetOgreMaterial();

		WideString GetName() const { return m_Name; }		
		void SetName(const WideString name) { m_Name = name; }

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty(bool dirty) { m_IsDirty = dirty; }

		Material* GetMaterialDef() const { return m_Material; }
		void SetMaterialDef(const WideString& fileName);

		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeMaterial"; }

		GenericPropertyValue* GetPropertyValue(const WideString& propName) const;

		// Ogre::ScriptCompilerListener
		virtual bool handleEvent(Ogre::ScriptCompiler* compiler, Ogre::ScriptCompilerEvent* evt, void* retval);

	private:
		void ClearProperies();
		bool LoadPropertyValuesFromXml(TiXmlElement* rootNode);
		bool SavePropertyValuesToXml(TiXmlElement* rootNode);

		void GenerateMaterial();

		WideString m_Name;
		Material* m_Material;
		GenericPropertyValues m_Properties;
		bool m_IsDirty;
	};
}

#endif // __WmeMaterialInstance_H__
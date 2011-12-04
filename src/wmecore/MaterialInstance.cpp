// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialManager.h"
#include "Game.h"
#include "Renderer.h"
#include "StringUtil.h"
#include <OgreScriptCompiler.h>
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MaterialInstance::MaterialInstance()
{
	m_Material = NULL;
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
MaterialInstance::~MaterialInstance()
{
	ClearProperies();
	
	if (m_Material) Game::GetInstance()->GetMaterialMgr()->ReleaseMaterialDef(m_Material);
}

//////////////////////////////////////////////////////////////////////////
void MaterialInstance::SetMaterialDef(const WideString& fileName)
{
	TiXmlElement* props = NULL;

	if (m_Material)
	{
		// store original property values so that we can repopulate them
		props = XmlUtil::AddElem("Properties");
		SavePropertyValuesToXml(props);

		Game::GetInstance()->GetMaterialMgr()->ReleaseMaterialDef(m_Material);
	}
	m_Material = Game::GetInstance()->GetMaterialMgr()->GetMaterialDef(fileName);
	if (m_Material) m_Material->GeneratePropValues(m_Properties);

	if (props)
	{		
		LoadPropertyValuesFromXml(props);
		SAFE_DELETE(props);
	}
	m_IsDirty = true;
}

//////////////////////////////////////////////////////////////////////////
Ogre::MaterialPtr MaterialInstance::GetOgreMaterial()
{
	if (IsDirty()) GenerateMaterial();

	WideString matName = GetFileName();
	return Ogre::MaterialManager::getSingleton().getByName(StringUtil::WideToUtf8(matName));
}

//////////////////////////////////////////////////////////////////////////
void MaterialInstance::GenerateMaterial()
{
	WideString matName = GetFileName();

	WideString str = L"";

	str += L"import " + m_Material->GetBaseMaterialName() + L" from \"" + m_Material->GetMaterialScriptFile() + L"\"\n";
	str += L"material " + matName + L" : " + m_Material->GetBaseMaterialName() + L"\n";
	str += L"{\n";

	foreach (GenericPropertyValues::value_type prop, m_Properties)
	{
		str += L"\tset $" + prop.first + L" \"" + prop.second->ToString() + L"\"\n";
	}

	str += L"}\n";

	try
	{
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(StringUtil::WideToUtf8(matName));
		if (!mat.isNull())
		{
			// if the material already exists, we're hitting an ogre bug
			// see http://www.ogre3d.org/mantis/view.php?id=130
			Game::GetInstance()->GetRenderer()->ScheduleClearSceneManagerPassMaps();
		}

		Ogre::ScriptCompiler compiler;
		compiler.setListener(this);
		compiler.compile(StringUtil::WideToUtf8(str), "", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);	
	}
	catch (Ogre::Exception e)
	{
		Game::GetInstance()->Log(L"Error generating material script for '" + GetFileName() + L"'.");
		Game::GetInstance()->Log(e.getDescription());
	}
	SetDirty(false);
}

//////////////////////////////////////////////////////////////////////////
bool MaterialInstance::handleEvent(Ogre::ScriptCompiler* compiler, Ogre::ScriptCompilerEvent* evt, void* retval)
{
	if (evt->mType == "createMaterial")
	{
		Ogre::CreateMaterialScriptCompilerEvent* matEvt = static_cast<Ogre::CreateMaterialScriptCompilerEvent*>(evt);
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matEvt->mName, matEvt->mResourceGroup);
		if (!mat.isNull())
		{
			Ogre::Material** matPtr = (Ogre::Material**)retval;
			*matPtr = mat.getPointer();
			return true;
		}

	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
GenericPropertyValue* MaterialInstance::GetPropertyValue(const WideString& propName) const
{
	GenericPropertyValues::const_iterator it = m_Properties.find(propName);
	if (it != m_Properties.end()) return it->second;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool MaterialInstance::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Name")
		{
			m_Name = XmlUtil::TextToString(elem);
		}
		else if (elem->ValueStr() == "MaterialDef")
		{
			if (m_Material) Game::GetInstance()->GetMaterialMgr()->ReleaseMaterialDef(m_Material);
			m_Material = Game::GetInstance()->GetMaterialMgr()->GetMaterialDef(XmlUtil::TextToString(elem));
		}
		else if (elem->ValueStr() == "Properties")
		{
			if (m_Material) m_Material->GeneratePropValues(m_Properties);
			LoadPropertyValuesFromXml(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MaterialInstance::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Name", rootNode);
	XmlUtil::SetText(elem, m_Name);

	if (m_Material)
	{
		elem = XmlUtil::AddElem("MaterialDef", rootNode);
		XmlUtil::SetFileName(elem, m_Material->GetFileName());
	}

	if (!m_Properties.empty())
	{
		elem = XmlUtil::AddElem("Properties", rootNode);
		SavePropertyValuesToXml(elem);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MaterialInstance::LoadPropertyValuesFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Property")
		{
			WideString propName = XmlUtil::AttrToString(elem, "Name");
			GenericPropertyValues::iterator it = m_Properties.find(propName);
			if (it != m_Properties.end()) (it->second)->LoadFromXml(elem);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MaterialInstance::SavePropertyValuesToXml(TiXmlElement* rootNode)
{
	foreach (GenericPropertyValues::value_type val, m_Properties)
	{
		TiXmlElement* elem = XmlUtil::AddElem("Property", rootNode);
		XmlUtil::SetAttr(elem, "Name", val.first);
		val.second->SaveToXml(elem);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void MaterialInstance::ClearProperies()
{
	GenericPropertyValues::iterator it;

	foreach (GenericPropertyValues::value_type val, m_Properties)
	{
		SAFE_DELETE(val.second);
	}
	m_Properties.clear();
}


} // namespace Wme

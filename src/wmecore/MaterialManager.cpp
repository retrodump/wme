// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MaterialManager.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "PathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MaterialManager::MaterialManager()
{
}

//////////////////////////////////////////////////////////////////////////
MaterialManager::~MaterialManager()
{
	foreach (MaterialMap::value_type val, m_Materials)
	{
		SAFE_DELETE(val.second);
	}
	m_Materials.clear();

	foreach (MaterialDefMap::value_type val, m_MaterialDefs)
	{
		SAFE_DELETE(val.second);
	}
	m_MaterialDefs.clear();
}

//////////////////////////////////////////////////////////////////////////
bool MaterialManager::Initialize()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
MaterialInstance* MaterialManager::GetMaterial(const WideString& fileName)
{
	WideString safeFileName = PathUtil::NormalizeFileName(fileName);

	MaterialMap::iterator it;
	it = m_Materials.find(safeFileName);

	if (it != m_Materials.end())
	{
		(*it).second->AddRef();
		return (*it).second;
	}
	else
	{
		MaterialInstance* material = new MaterialInstance();
		if (material->LoadFromFile(safeFileName))
		{
			m_Materials[safeFileName] = material;
			material->AddRef();
			return material;
		}
		else
		{
			delete material;
			return NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void MaterialManager::ReleaseMaterial(MaterialInstance*& material)
{
	if (!material) return;

	MaterialMap::iterator it;
	it = m_Materials.find(PathUtil::NormalizeFileName(material->GetFileName()));

	if (it != m_Materials.end())
	{
		if (!it->second->ReleaseRef()) m_Materials.erase(it);
	}
	material = NULL;
}

//////////////////////////////////////////////////////////////////////////
Material* MaterialManager::GetMaterialDef(const WideString& fileName)
{
	WideString safeFileName = PathUtil::NormalizeFileName(fileName);

	MaterialDefMap::iterator it;
	it = m_MaterialDefs.find(safeFileName);

	if (it != m_MaterialDefs.end())
	{
		(*it).second->AddRef();
		return (*it).second;
	}
	else
	{
		Material* material = new Material();
		if (material->LoadFromFile(safeFileName))
		{
			m_MaterialDefs[safeFileName] = material;
			material->AddRef();
			return material;
		}
		else
		{
			delete material;
			return NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void MaterialManager::ReleaseMaterialDef(Material*& material)
{
	if (!material) return;

	MaterialDefMap::iterator it;
	it = m_MaterialDefs.find(PathUtil::NormalizeFileName(material->GetFileName()));

	if (it != m_MaterialDefs.end())
	{
		if (!it->second->ReleaseRef()) m_MaterialDefs.erase(it);
	}
	material = NULL;
}


} // namespace Wme

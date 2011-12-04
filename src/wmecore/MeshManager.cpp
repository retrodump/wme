// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "MeshEntity.h"
#include "PathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MeshManager::MeshManager()
{
}

//////////////////////////////////////////////////////////////////////////
MeshManager::~MeshManager()
{
	foreach (MeshMap::value_type val, m_Meshes)
	{
		SAFE_DELETE(val.second);
	}
	m_Meshes.clear();
}

//////////////////////////////////////////////////////////////////////////
bool MeshManager::Initialize()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
Mesh* MeshManager::GetMesh(const WideString& fileName, MeshEntity* owner)
{
	WideString safeFileName = PathUtil::NormalizeFileName(fileName);

	MeshMap::iterator it;
	it = m_Meshes.find(safeFileName);

	if (it != m_Meshes.end())
	{
		(*it).second->AddRef();
		(*it).second->AddOwner(owner);
		return (*it).second;
	}
	else
	{
		Mesh* mesh = new Mesh();
		if (mesh->LoadFromFile(safeFileName))
		{
			m_Meshes[safeFileName] = mesh;
			mesh->AddRef();
			mesh->AddOwner(owner);
			return mesh;
		}
		else
		{
			delete mesh;
			return NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void MeshManager::ReleaseMesh(Mesh*& mesh, MeshEntity* owner)
{
	if (!mesh) return;

	MeshMap::iterator it;
	it = m_Meshes.find(PathUtil::NormalizeFileName(mesh->GetFileName()));

	if (it != m_Meshes.end())
	{
		it->second->RemoveOwner(owner);
		if (!it->second->ReleaseRef()) m_Meshes.erase(it);
	}
	mesh = NULL;
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "SkeletonManager.h"
#include "MeshEntity.h"
#include "Game.h"
#include "Renderer.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
{
	m_Skeleton = NULL;
}

//////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{
	m_OgreMesh.setNull();
	Game::GetInstance()->GetRenderer()->ScheduleMeshCleanup();

	if (m_Skeleton) Game::GetInstance()->GetSkeletonMgr()->ReleaseSkeleton(m_Skeleton);
}

//////////////////////////////////////////////////////////////////////////
void Mesh::AddOwner(MeshEntity* entity)
{
	if (!IsOwnedBy(entity)) m_Owners.insert(entity);
}

//////////////////////////////////////////////////////////////////////////
void Mesh::RemoveOwner(MeshEntity* entity)
{
	if (IsOwnedBy(entity)) m_Owners.erase(entity);
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::IsOwnedBy(MeshEntity* entity) const
{
	return m_Owners.find(entity) != m_Owners.end();
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::LoadFromFile(const WideString& fileName)
{
	if (fileName.empty()) return false;

	Utf8String meshName = StringUtil::WideToUtf8(fileName);
	Ogre::MeshSerializerListener* oldListener = NULL;

	try
	{
		oldListener = Ogre::MeshManager::getSingleton().getListener();
		Ogre::MeshManager::getSingleton().setListener(this);
		m_OgreMesh = Ogre::MeshManager::getSingleton().load(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		m_FileName = fileName;
	}
	catch (Ogre::Exception& e)
	{
		m_OgreMesh.setNull();
		Game::GetInstance()->Log(L"Error loading mesh '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());
	}
	Ogre::MeshManager::getSingleton().setListener(oldListener);
	
	return !m_OgreMesh.isNull();
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::SupportsSkeletalAnimation() const
{
	Ogre::MeshPtr ogreMesh = GetOgreMesh();
	if (ogreMesh.isNull()) return false;

	for (size_t i = 0; i < ogreMesh->getNumSubMeshes(); i++)
	{
		size_t s = ogreMesh->getSubMesh(i)->getBoneAssignments().size();
		if (s > 0) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::SetSkeleton(const WideString& skeletonFile)
{
	RemoveSkeleton();
	return AddSkeleton(skeletonFile);
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::AddSkeleton(const WideString& skeletonFile, const WideString& animsToMerge)
{	
	if (skeletonFile.empty()) return false;

	if (!m_Skeleton)
	{		
		if (SupportsSkeletalAnimation())
		{
			m_Skeleton = Game::GetInstance()->GetSkeletonMgr()->GetSkeleton(skeletonFile);
			if (m_Skeleton)
			{
				GetOgreMesh()->setSkeletonName(StringUtil::WideToUtf8(m_Skeleton->GetFileName()));
				foreach (MeshEntity* owner, m_Owners)
				{
					owner->OnSkeletonAdded();
				}
			}
		}
		return (m_Skeleton != NULL);
	}
	else
	{
		return m_Skeleton->AddSkeleton(skeletonFile, animsToMerge);
	}
}

//////////////////////////////////////////////////////////////////////////
void Mesh::RemoveSkeleton()
{
	if (m_Skeleton)
	{
		Game::GetInstance()->GetSkeletonMgr()->ReleaseSkeleton(m_Skeleton);

		foreach (MeshEntity* owner, m_Owners)
		{
			owner->OnSkeletonRemoved();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::DeleteAnimation(const WideString& animationName)
{
	if (m_Skeleton)
	{
		bool ret = m_Skeleton->DeleteAnimation(animationName);

		foreach (MeshEntity* owner, m_Owners)
		{
			Ogre::Entity* entity = owner->GetOgreEntity();
			if (entity)	entity->getSkeleton()->_refreshAnimationState(entity->getAllAnimationStates());
		}

		return ret;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
WideString Mesh::GetSkeletonFileName() const
{
	if (m_Skeleton) return m_Skeleton->GetFileName();
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
bool Mesh::HasAnimation(const WideString& animName) const
{
	try
	{
		if (m_Skeleton) return m_Skeleton->GetOgreSkeleton()->hasAnimation(StringUtil::WideToUtf8(animName));
		else return false;
	}
	catch (Ogre::Exception e)
	{
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////
// Ogre::MeshSerializerListener
//////////////////////////////////////////////////////////////////////////
void Mesh::processMaterialName(Ogre::Mesh* mesh, Ogre::String* name)
{
	// we are loading material elsewhere
	// just set the default material for now to prevent log errors
	*name = "BaseWhite";
}

//////////////////////////////////////////////////////////////////////////
void Mesh::processSkeletonName(Ogre::Mesh* mesh, Ogre::String* name)
{
	// we are loading the skeleton elsewhere
	*name = Ogre::StringUtil::BLANK;
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GizmoBase.h"
#include "StringUtil.h"
#include "Game.h"


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
GizmoBase::GizmoBase()
{
}

//////////////////////////////////////////////////////////////////////////
GizmoBase::~GizmoBase()
{
}

//////////////////////////////////////////////////////////////////////////
void GizmoBase::LoadMesh(const WideString& fileName)
{
	Ogre::MeshSerializerListener* oldListener = Ogre::MeshManager::getSingleton().getListener();
	try
	{
		Ogre::MeshManager::getSingleton().setListener(this);
		m_Mesh = Ogre::MeshManager::getSingleton().load(StringUtil::WideToUtf8(fileName), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error loading mesh '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());

		m_Mesh.setNull();
	}
	Ogre::MeshManager::getSingleton().setListener(oldListener);
}

//////////////////////////////////////////////////////////////////////////
void GizmoBase::LoadMaterial()
{
	WideString materialFile = L"__system/editor/gizmos/gizmo.material";

	// preload material script
	try
	{
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(StringUtil::WideToUtf8(materialFile));
		Ogre::MaterialManager::getSingleton().parseScript(stream, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error loading material script '" + materialFile + L"'.");
		Game::GetInstance()->Log(e.getDescription());
	}
}

//////////////////////////////////////////////////////////////////////////
// Ogre::MeshSerializerListener
//////////////////////////////////////////////////////////////////////////
void GizmoBase::processMaterialName(Ogre::Mesh* mesh, Ogre::String* name)
{
	// we will set proper material later
	*name = "BaseWhite";
}

//////////////////////////////////////////////////////////////////////////
void GizmoBase::processSkeletonName(Ogre::Mesh* mesh, Ogre::String* name)
{
	// do nothing
}


} // namespace Wme


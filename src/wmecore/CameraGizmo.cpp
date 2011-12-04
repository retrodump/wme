// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CameraGizmo.h"
#include "Viewport.h"
#include "Camera.h"
#include "CameraEntity.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "Game.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
CameraGizmo::CameraGizmo(CameraEntity* parentCamera)
{
	m_ParentCamera = parentCamera;

	m_SceneNode = NULL;
	m_Entity = NULL;
}

//////////////////////////////////////////////////////////////////////////
CameraGizmo::~CameraGizmo()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////
void CameraGizmo::Create()
{
	Destroy();

	LoadMaterial();

	Utf8String gizmoName = StringUtil::WideToUtf8(L"sys/gizmo/camera/" + m_ParentCamera->GetName());

	// load mesh
	LoadMesh(L"__system/editor/gizmos/camera.mesh");
	m_SceneNode = m_ParentCamera->GetStage()->GetSystemRootNode()->createChildSceneNode(gizmoName);

	m_Entity = m_ParentCamera->GetStage()->GetSceneMgr()->createEntity(gizmoName, m_Mesh->getName());
	m_SceneNode->attachObject(m_Entity);

	m_Entity->setCastShadows(false);
	m_Entity->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/camera"));

	m_Entity->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_ENTITY, m_ParentCamera)));
}

//////////////////////////////////////////////////////////////////////////
void CameraGizmo::Destroy()
{
	if (m_SceneNode) m_SceneNode->detachAllObjects();
	if (m_Entity) m_ParentCamera->GetStage()->GetSceneMgr()->destroyEntity(m_Entity);
	if (m_SceneNode) m_ParentCamera->GetStage()->GetSceneMgr()->destroySceneNode(m_SceneNode);

	m_SceneNode = NULL;
	m_Entity = NULL;
}

//////////////////////////////////////////////////////////////////////////
void CameraGizmo::PreRender(Viewport* viewport)
{
	Camera* cam = viewport->GetCamera();
	if (!cam) return;


	m_SceneNode->_setDerivedPosition(m_ParentCamera->GetSceneNode()->_getDerivedPosition());
	m_SceneNode->_setDerivedOrientation(m_ParentCamera->GetSceneNode()->_getDerivedOrientation());
	m_SceneNode->setScale(20, 20, 20);

	// hide the gizmo if our point of view is inside its bounding box
	m_Entity->setVisible(!m_Entity->getWorldBoundingBox(true).contains(cam->GetOgreCamera()->getPosition()));
}


} // namespace Wme

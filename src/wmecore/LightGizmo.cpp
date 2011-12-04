// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LightGizmo.h"
#include "LightEntity.h"
#include "Viewport.h"
#include "Camera.h"
#include "StringUtil.h"
#include "Scene3DBase.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LightGizmo::LightGizmo(LightEntity* parentLight)
{
	m_ParentLight = parentLight;

	m_SceneNode = NULL;
	m_Entity = NULL;
}

//////////////////////////////////////////////////////////////////////////
LightGizmo::~LightGizmo()
{
	Destroy();
}


//////////////////////////////////////////////////////////////////////////
void LightGizmo::Create()
{
	// preserve selection material
	AnsiString materialName = "sys/gizmo/light";
	if (m_Entity) materialName = m_Entity->getSubEntity(0)->getMaterialName();

	Destroy();

	LoadMaterial();

	Utf8String gizmoName = StringUtil::WideToUtf8(L"sys/gizmo/light/" + m_ParentLight->GetName());

	// load mesh
	switch (m_ParentLight->GetLightType())
	{
	case LightEntity::LIGHT_POINT:
		LoadMesh(L"__system/editor/gizmos/point.mesh");
		break;
	case LightEntity::LIGHT_DIRECTIONAL:
		LoadMesh(L"__system/editor/gizmos/directional.mesh");
		break;
	case LightEntity::LIGHT_SPOT:
		LoadMesh(L"__system/editor/gizmos/spot.mesh");
		break;
	}

	m_SceneNode = m_ParentLight->GetStage()->GetSystemRootNode()->createChildSceneNode(gizmoName);

	m_Entity = m_ParentLight->GetStage()->GetSceneMgr()->createEntity(gizmoName, m_Mesh->getName());
	m_SceneNode->attachObject(m_Entity);

	m_Entity->setCastShadows(false);
	m_Entity->setMaterial(Ogre::MaterialManager::getSingleton().getByName(materialName));

	m_Entity->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_ENTITY, m_ParentLight)));
}

//////////////////////////////////////////////////////////////////////////
void LightGizmo::Destroy()
{
	if (m_SceneNode) m_SceneNode->detachAllObjects();
	if (m_Entity) m_ParentLight->GetStage()->GetSceneMgr()->destroyEntity(m_Entity);
	if (m_SceneNode) m_ParentLight->GetStage()->GetSceneMgr()->destroySceneNode(m_SceneNode);

	m_SceneNode = NULL;
	m_Entity = NULL;
}

//////////////////////////////////////////////////////////////////////////
void LightGizmo::PreRender(Viewport* viewport)
{
	Camera* cam = viewport->GetCamera();
	if (!cam) return;


	m_SceneNode->_setDerivedPosition(m_ParentLight->GetSceneNode()->_getDerivedPosition());
	m_SceneNode->_setDerivedOrientation(m_ParentLight->GetSceneNode()->_getDerivedOrientation());

	switch (m_ParentLight->GetLightType())
	{
	case LightEntity::LIGHT_POINT:
		m_SceneNode->setScale(15, 15, 15);
		break;
	case LightEntity::LIGHT_DIRECTIONAL:
		m_SceneNode->setScale(15, 15, 15);
		break;
	case LightEntity::LIGHT_SPOT:
		m_SceneNode->setScale(20, 20, 20);
		break;
	}

	// hide the gizmo if our point of view is inside its bounding box
	m_Entity->setVisible(!m_Entity->getWorldBoundingBox(true).contains(cam->GetOgreCamera()->getPosition()));
}


} // namespace Wme

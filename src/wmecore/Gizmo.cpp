// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Gizmo.h"
#include "Game.h"
#include "NodeSelection.h"
#include "SceneNodeEditor.h"
#include "StringUtil.h"
#include "Viewport.h"
#include "Camera.h"
#include "Stage.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Gizmo::Gizmo(SceneNodeEditor* editor)
{
	m_Editor = editor;

	m_SceneNode = NULL;
	m_Entity = NULL;

	m_IsVisible = true;
}

//////////////////////////////////////////////////////////////////////////
Gizmo::~Gizmo()
{
	DestroyObject(m_SceneNode, m_Entity);
}

//////////////////////////////////////////////////////////////////////////
void Gizmo::ConformTo(NodeSelection* selection)
{
	m_SceneNode->_setDerivedPosition(selection->GetOrigin());

	if (m_Editor->GetGizmoLocalMode())
		m_SceneNode->_setDerivedOrientation(selection->GetOrientation());
	else
		m_SceneNode->_setDerivedOrientation(Ogre::Quaternion::IDENTITY);
}

//////////////////////////////////////////////////////////////////////////
void Gizmo::PreRender(Viewport* viewport, NodeSelection* selection)
{
	Camera* cam = viewport->GetCamera();
	if (!cam) return;

	//float viewportSize = std::min((float)viewport->GetOgreViewport()->getActualWidth(), (float)viewport->GetOgreViewport()->getActualHeight());
	float distance = (cam->GetOgreCamera()->getPosition() - selection->GetOrigin()).length();

	//distance /= (viewportSize / 8.0f);
	distance /= 80.0f;
	m_SceneNode->setScale(distance, distance, distance);
}

//////////////////////////////////////////////////////////////////////////
void Gizmo::DestroyObject(Ogre::SceneNode* node, Ogre::Entity* entity)
{
	if (node) node->detachAllObjects();
	if (entity) m_Editor->GetStage()->GetSceneMgr()->destroyEntity(entity);
	if (node) m_Editor->GetStage()->GetSceneMgr()->destroySceneNode(node);	
}


} // namespace Wme

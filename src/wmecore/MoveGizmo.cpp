// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MoveGizmo.h"
#include "Game.h"
#include "ContentManager.h"
#include "Stage.h"
#include "Camera.h"
#include "Viewport.h"
#include "MousePickParams.h"
#include "SceneNodeEditor.h"
#include "NodeSelection.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MoveGizmo::MoveGizmo(SceneNodeEditor* editor) : Gizmo(editor)
{
	m_NodeX = m_NodeY = m_NodeZ = NULL;
	m_EntityX = m_EntityY = m_EntityZ = NULL;

	m_DragViewport = NULL;
	m_DragPart = NULL;
	m_DragOrigin = Ogre::Vector3::ZERO;
	m_DragStoredPos = Ogre::Vector3::ZERO;
}

//////////////////////////////////////////////////////////////////////////
MoveGizmo::~MoveGizmo()
{
	DestroyObject(m_NodeX, m_EntityX);
	DestroyObject(m_NodeY, m_EntityY);
	DestroyObject(m_NodeZ, m_EntityZ);
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::Create()
{
	LoadMaterial();

	// load mesh
	LoadMesh(L"__system/editor/gizmos/arrow.mesh");
	m_SceneNode = m_Editor->GetStage()->GetSystemRootNode()->createChildSceneNode("sys/move_gizmo");

	Ogre::Quaternion rot;
	
	// x arrow
	CreateArrow(m_SceneNode, m_Mesh, L"x", m_NodeX, m_EntityX);
	
	// y arrow
	CreateArrow(m_SceneNode, m_Mesh, L"y", m_NodeY, m_EntityY);
	rot.FromAngleAxis(Ogre::Radian(Ogre::Degree(90)), Ogre::Vector3::UNIT_Z);
	m_NodeY->setOrientation(rot);
	
	// z arrow
	CreateArrow(m_SceneNode, m_Mesh, L"z", m_NodeZ, m_EntityZ);
	rot.FromAngleAxis(Ogre::Radian(Ogre::Degree(-90)), Ogre::Vector3::UNIT_Y);
	m_NodeZ->setOrientation(rot);

	// setup materials
	HighlightPart();
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::CreateArrow(Ogre::SceneNode* parentNode, const Ogre::MeshPtr mesh, const WideString& name, Ogre::SceneNode*& node, Ogre::Entity*& entity)
{
	node = parentNode->createChildSceneNode();
	entity = m_Editor->GetStage()->GetSceneMgr()->createEntity(StringUtil::WideToUtf8(L"sys/gizmo/" + name + L"_arrow"), mesh->getName());
	node->attachObject(entity);

	entity->setCastShadows(false);
	entity->setRenderQueueGroup(GIZMO_RENDER_QUEUE_ID);	
	entity->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_GIZMO, this)));
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::HighlightPart(Ogre::Entity* entity)
{
	if (entity == m_EntityX)
		m_EntityX->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/x_sel"));
	else
		m_EntityX->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/x"));

	if (entity == m_EntityY)
		m_EntityY->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/y_sel"));
	else
		m_EntityY->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/y"));

	if (entity == m_EntityZ)
		m_EntityZ->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/z_sel"));
	else
		m_EntityZ->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/z"));
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::HighlightDragPart()
{
	HighlightPart(m_DragPart);
}

//////////////////////////////////////////////////////////////////////////
bool MoveGizmo::GetAxis(Ogre::Entity* part, Ogre::Vector3& axis)
{
	if (part == m_EntityX)
		axis = m_SceneNode->_getDerivedOrientation().xAxis();
	else if (part == m_EntityY)
		axis = m_SceneNode->_getDerivedOrientation().yAxis();
	else if (part == m_EntityZ)
		axis = m_SceneNode->_getDerivedOrientation().zAxis();
	else return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MoveGizmo::GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal)
{
	Ogre::Vector3 axis;
	if (!GetAxis(part, axis)) return false;

	Ogre::Vector3 camDir = camera->GetOgreCamera()->getDerivedDirection();

	Ogre::Vector3 cross = axis.crossProduct(camDir);
	normal = cross.crossProduct(axis);
	normal.normalise();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool MoveGizmo::StartDrag(Viewport* viewport, Ogre::Entity* part)
{
	if (!part) return false;
	if (m_Editor->GetSelection()->IsEmpty()) return false;

	Ogre::Vector3 normal;
	if (!GetNormal(viewport->GetCamera(), part, normal)) return false;

	m_DragViewport = viewport;
	m_DragPart = part;
	m_DragPlane = Ogre::Plane(normal, m_Editor->GetSelection()->GetOrigin());
	m_DragStoredPos = m_Editor->GetSelection()->GetOrigin();

	// get drag starting point
	m_DragViewport->GetMousePointOnPlane(m_DragPlane, m_DragOrigin);

	Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);

	m_EntityX->setVisible(part == m_EntityX);
	m_EntityY->setVisible(part == m_EntityY);
	m_EntityZ->setVisible(part == m_EntityZ);
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::EndDrag()
{
	m_EntityX->setVisible(true);
	m_EntityY->setVisible(true);
	m_EntityZ->setVisible(true);

	Game::GetInstance()->GetContentMgr()->ReleaseMouse(m_DragViewport);
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::UpdateDrag()
{

	Ogre::Vector3 point;
	if (!m_DragViewport->GetMousePointOnPlane(m_DragPlane, point))
		return;

	Ogre::Vector3 moveVect = point - m_DragOrigin;

	Ogre::Vector3 axis;
	if (!GetAxis(m_DragPart, axis)) return;

	Ogre::Real mag = moveVect.dotProduct(axis);

	if (Game::GetInstance()->GetContentMgr()->IsControlDown())
		mag -= fmod(mag, m_Editor->GetMoveSnap());

	Ogre::Vector3 clampedMove = mag * axis;

	Ogre::Vector3 moveDiff = m_DragStoredPos - m_Editor->GetSelection()->GetOrigin() + clampedMove;
	m_Editor->GetSelection()->MoveBy(moveDiff, m_Editor->GetGizmoLocalMode());

	ConformTo(m_Editor->GetSelection());
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::SetVisible(bool visible, NodeSelection* selection)
{
	m_EntityX->setVisible(visible);
	m_EntityY->setVisible(visible);
	m_EntityZ->setVisible(visible);

	m_IsVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
void MoveGizmo::Reset()
{
	Ogre::Vector3 moveDiff = m_DragStoredPos - m_Editor->GetSelection()->GetOrigin();
	m_Editor->GetSelection()->MoveBy(moveDiff, m_Editor->GetGizmoLocalMode());
}


} // namespace Wme

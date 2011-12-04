// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "RotGizmo.h"
#include "SceneNodeEditor.h"
#include "Camera.h"
#include "Viewport.h"
#include "Stage.h"
#include "StringUtil.h"
#include "NodeSelection.h"
#include "ContentManager.h"
#include "MathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
RotGizmo::RotGizmo(SceneNodeEditor* editor) : Gizmo(editor)
{
	m_NodeX = m_NodeY = m_NodeZ = NULL;
	m_EntityX = m_EntityY = m_EntityZ = NULL;

	m_DragViewport = NULL;
	m_DragPart = NULL;
	m_DragOrigin = Ogre::Vector3::ZERO;
	m_DragCenter = Ogre::Vector3::ZERO;
	m_DragStoredRot = Ogre::Quaternion();

}

//////////////////////////////////////////////////////////////////////////
RotGizmo::~RotGizmo()
{
	DestroyObject(m_NodeX, m_EntityX);
	DestroyObject(m_NodeY, m_EntityY);
	DestroyObject(m_NodeZ, m_EntityZ);
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::Create()
{
	LoadMaterial();

	// load mesh
	LoadMesh(L"__system/editor/gizmos/ring.mesh");

	m_SceneNode = m_Editor->GetStage()->GetSystemRootNode()->createChildSceneNode("sys/rotation_gizmo");

	Ogre::Quaternion rot;

	float ringSize = 1.6f;
	float ringGrowth = 0.02f; // to prevent depth fighting on ring crossings

	// y ring
	CreateRing(m_SceneNode, m_Mesh, L"y", m_NodeY, m_EntityY);
	m_NodeY->setScale(ringSize + ringGrowth, 1.0f, ringSize + ringGrowth);

	// x ring
	CreateRing(m_SceneNode, m_Mesh, L"x", m_NodeX, m_EntityX);
	rot.FromAngleAxis(Ogre::Radian(Ogre::Degree(90)), Ogre::Vector3::UNIT_Z);
	m_NodeX->setOrientation(rot);
	m_NodeX->setScale(ringSize + ringGrowth, 1.0f, ringSize);

	// z ring
	CreateRing(m_SceneNode, m_Mesh, L"z", m_NodeZ, m_EntityZ);
	rot.FromAngleAxis(Ogre::Radian(Ogre::Degree(-90)), Ogre::Vector3::UNIT_X);
	m_NodeZ->setOrientation(rot);
	m_NodeZ->setScale(ringSize, 1.0f, ringSize);

	// setup materials
	HighlightPart();
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::CreateRing(Ogre::SceneNode* parentNode, const Ogre::MeshPtr mesh, const WideString& name, Ogre::SceneNode*& node, Ogre::Entity*& entity)
{
	node = parentNode->createChildSceneNode();
	entity = m_Editor->GetStage()->GetSceneMgr()->createEntity(StringUtil::WideToUtf8(L"sys/gizmo/" + name + L"_ring"), mesh->getName());
	node->attachObject(entity);

	entity->setCastShadows(false);
	entity->setRenderQueueGroup(GIZMO_RENDER_QUEUE_ID);	
	entity->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_GIZMO, this)));
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::HighlightPart(Ogre::Entity* entity)
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
void RotGizmo::HighlightDragPart()
{
	HighlightPart(m_DragPart);
}

//////////////////////////////////////////////////////////////////////////
bool RotGizmo::GetAxis(Ogre::Entity* part, Ogre::Vector3& axis)
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
bool RotGizmo::GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal)
{
	Ogre::Vector3 axis;
	if (!GetAxis(part, axis)) return false;

	normal = axis;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool RotGizmo::StartDrag(Viewport* viewport, Ogre::Entity* part)
{
	if (!part) return false;
	if (m_Editor->GetSelection()->IsEmpty()) return false;

	Ogre::Vector3 normal;
	if (!GetNormal(viewport->GetCamera(), part, normal)) return false;

	m_DragViewport = viewport;
	m_DragPart = part;
	m_DragPlane = Ogre::Plane(normal, m_Editor->GetSelection()->GetOrigin());
	m_DragStoredRot = m_Editor->GetSelection()->GetOrientation();
	m_DragCenter = m_Editor->GetSelection()->GetOrigin();

	// get drag starting point
	m_DragViewport->GetMousePointOnPlane(m_DragPlane, m_DragOrigin);

	Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);

	m_EntityX->setVisible(part == m_EntityX);
	m_EntityY->setVisible(part == m_EntityY);
	m_EntityZ->setVisible(part == m_EntityZ);

	return true;
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::EndDrag()
{
	m_EntityX->setVisible(true);
	m_EntityY->setVisible(true);
	m_EntityZ->setVisible(true);

	Game::GetInstance()->GetContentMgr()->ReleaseMouse(m_DragViewport);
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::UpdateDrag()
{
	Ogre::Vector3 point;
	if (!m_DragViewport->GetMousePointOnPlane(m_DragPlane, point))
	{
		return;
	}

	Ogre::Vector3 vectOrig = m_DragOrigin - m_DragCenter;
	Ogre::Vector3 vectCurr = point - m_DragCenter;

	Ogre::Real cosAngle = vectOrig.dotProduct(vectCurr) / (vectOrig.length() * vectCurr.length());	
	Ogre::Radian angle = Ogre::Math::ACos(cosAngle);

	// check rotation direction
	Ogre::Vector3 vectCurrRight = vectCurr.crossProduct(m_DragPlane.normal);

	Ogre::Real rightDot = vectOrig.dotProduct(vectCurrRight);
	if (rightDot < 0.0f) angle = -angle;

	

	//Game::GetInstance()->m_TestInfo = L"angle: " + StringUtil::ToString(angle.valueDegrees());

	// rotation axis
	Ogre::Vector3 rotAxis;
	Ogre::Radian origAngle;
	if (m_DragPart == m_EntityX)
	{
		rotAxis = Ogre::Vector3::UNIT_X;
	}
	else if (m_DragPart == m_EntityY)
	{
		rotAxis = Ogre::Vector3::UNIT_Y;
	}
	else if (m_DragPart == m_EntityZ)
	{
		rotAxis = Ogre::Vector3::UNIT_Z;
	}
	else return;


	if (Game::GetInstance()->GetContentMgr()->IsControlDown())
		SnapAngle(angle);


	// rotate selection
	Ogre::Quaternion q;
	q.FromAngleAxis(angle, rotAxis);

	if (m_Editor->GetGizmoLocalMode())
		q = m_DragStoredRot * q;
	else
		q = q * m_DragStoredRot;




	m_Editor->GetSelection()->SetOrientation(q, m_Editor->GetGizmoLocalMode());
		

	ConformTo(m_Editor->GetSelection());
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::SetVisible(bool visible, NodeSelection* selection)
{
	m_EntityX->setVisible(visible);
	m_EntityY->setVisible(visible);
	m_EntityZ->setVisible(visible);

	m_IsVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::Reset()
{
	m_Editor->GetSelection()->SetOrientation(m_DragStoredRot, m_Editor->GetGizmoLocalMode());
}

//////////////////////////////////////////////////////////////////////////
void RotGizmo::SnapAngle(Ogre::Radian& angle)
{
	Ogre::Real angleDeg = MathUtil::NormalizeAngle(Ogre::Degree(angle)).valueDegrees();
	angleDeg -= fmod(angleDeg, m_Editor->GetRotSnap());
	
	angle = Ogre::Radian(MathUtil::NormalizeAngle(Ogre::Degree(angleDeg)));
}


} // namespace Wme
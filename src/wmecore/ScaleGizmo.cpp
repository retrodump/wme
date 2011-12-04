// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScaleGizmo.h"
#include "LineElement3D.h"
#include "SceneNodeEditor.h"
#include "NodeSelection.h"
#include "Stage.h"
#include "CompositeBBox.h"
#include "StringUtil.h"
#include "Viewport.h"
#include "Camera.h"
#include "ContentManager.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ScaleGizmo::ScaleGizmo(SceneNodeEditor* editor) : Gizmo(editor)
{
	m_Box = NULL;
	m_SceneNode = NULL;

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		m_HandleEnt[i] = NULL;
		m_HandleNode[i] = NULL;
	}

	m_DragPart = NULL;
	m_DragViewport = NULL;
	m_DragOrigin = Ogre::Vector3::ZERO;
	m_DragCenter = Ogre::Vector3::ZERO;
	m_DragStoredScale = Ogre::Vector3::ZERO;
	m_LastScale = Ogre::Vector3::ZERO;
}

//////////////////////////////////////////////////////////////////////////
ScaleGizmo::~ScaleGizmo()
{
	SAFE_DELETE(m_Box);

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		DestroyObject(m_HandleNode[i], m_HandleEnt[i]);
	}	
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::Create()
{
	m_SceneNode = m_Editor->GetStage()->GetSystemRootNode()->createChildSceneNode("sys/scale_gizmo");

	// selection bounding box
	m_Box = new LineElement3D();
	m_SceneNode->attachObject(m_Box);
	m_Box->SetDepthCheck(false);

	LoadMaterial();

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		Utf8String handleName = StringUtil::WideToUtf8(L"sys/gizmo/scale" + StringUtil::ToString(i));

		m_HandleNode[i] = m_SceneNode->createChildSceneNode();
		m_HandleEnt[i] = m_Editor->GetStage()->GetSceneMgr()->createEntity(handleName, Ogre::SceneManager::PT_CUBE);

		m_HandleNode[i]->attachObject(m_HandleEnt[i]);

		m_HandleEnt[i]->setCastShadows(false);
		m_HandleEnt[i]->setRenderQueueGroup(GIZMO_RENDER_QUEUE_ID);	
		m_HandleEnt[i]->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_GIZMO, this)));
	}
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::ConformTo(NodeSelection* selection)
{
	Ogre::SceneNode* anchor = selection->GetAnchorNode();
	if (!anchor) return;

	m_Box->ClearLines();
	m_Box->AddBox(selection->GetBBox()->GetMinimum(), selection->GetBBox()->GetMaximum());
	m_Box->UpdateGeometry();

	m_SceneNode->_setDerivedPosition(anchor->_getDerivedPosition());
	m_SceneNode->_setDerivedOrientation(anchor->_getDerivedOrientation());
	m_SceneNode->setScale(anchor->_getDerivedScale());

	PositionHandles(selection->GetBBox()->GetMinimum(), selection->GetBBox()->GetMaximum());
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::HighlightPart(Ogre::Entity* entity)
{
	for (int i = 0; i < NUM_HANDLES; i++)
	{
		// corners
		if (i >= CORNER_FAR_LEFT_BOTTOM && i <= CORNER_NEAR_RIGHT_BOTTOM)
		{
			if (m_HandleEnt[i] == entity) m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/scale_sel"));
			else m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/scale"));
		}

		// x faces
		else if (i >= FACE_X_POS && i <= FACE_X_NEG)
		{
			if (m_HandleEnt[i] == entity) m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/x_sel"));
			else m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/x"));
		}

		// y faces
		else if (i >= FACE_Y_POS && i <= FACE_Y_NEG)
		{
			if (m_HandleEnt[i] == entity) m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/y_sel"));
			else m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/y"));
		}

		// z faces
		else if (i >= FACE_Z_POS && i <= FACE_Z_NEG)
		{
			if (m_HandleEnt[i] == entity) m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/z_sel"));
			else m_HandleEnt[i]->setMaterial(Ogre::MaterialManager::getSingleton().getByName("sys/gizmo/z"));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::HighlightDragPart()
{
	HighlightPart(m_DragPart);
}

//////////////////////////////////////////////////////////////////////////
bool ScaleGizmo::GetAxis(Ogre::Entity* part, Ogre::Vector3& axis)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScaleGizmo::GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScaleGizmo::StartDrag(Viewport* viewport, Ogre::Entity* part)
{
	if (!part) return false;
	if (m_Editor->GetSelection()->IsEmpty()) return false;

	if (!GetDragOrigin(part, viewport)) return false;

	Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		m_HandleEnt[i]->setVisible(m_HandleEnt[i] == part);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::EndDrag()
{
	SetVisible(true, m_Editor->GetSelection());
	Game::GetInstance()->GetContentMgr()->ReleaseMouse(m_DragViewport);
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::UpdateDrag()
{
	Ogre::Vector3 point;
	if (!m_DragViewport->GetMousePointOnRay(m_DragRay, point))
		return;

	Ogre::Real origLength = (m_DragOrigin - m_DragCenter).length();
	if (origLength == 0) return;

	Ogre::Real newLength = (point - m_DragCenter).length();

	if (Game::GetInstance()->GetContentMgr()->IsControlDown())
	{
		newLength -= fmod(newLength, m_Editor->GetScaleSnap());
		if (newLength <= 0.0f) return;
	}

	Ogre::Real scaleDiff = newLength / origLength;

	Ogre::Vector3 newScale = m_DragStoredScale;
	int dragType = GetPartType(m_DragPart);

	switch (dragType)
	{
	case FACE_X_POS:
	case FACE_X_NEG:
		newScale.x *= scaleDiff;
		break;

	case FACE_Y_POS:
	case FACE_Y_NEG:
		newScale.y *= scaleDiff;
		break;

	case FACE_Z_POS:
	case FACE_Z_NEG:
		newScale.z *= scaleDiff;
		break;

	default:
		newScale *= scaleDiff;
	}

	m_Editor->GetSelection()->ScaleBy(Ogre::Vector3::UNIT_SCALE * (newScale / m_LastScale));
	m_LastScale = newScale;


	ConformTo(m_Editor->GetSelection());
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::SetVisible(bool visible, NodeSelection* selection)
{
	m_Box->setVisible(visible);

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		if (i <= CORNER_NEAR_RIGHT_BOTTOM)
			m_HandleEnt[i]->setVisible(visible);
		else
			// hide face handles if multiple entities are selected
			m_HandleEnt[i]->setVisible(visible && !selection->IsMultiSelect());
	}

	m_IsVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::GetHandlePosition(int handleType, Ogre::Vector3& pos)
{
	Ogre::Vector3 min, max;
	min = m_Editor->GetSelection()->GetBBox()->GetMinimum();
	max = m_Editor->GetSelection()->GetBBox()->GetMaximum();

	float midX = min.x + (max.x - min.x) / 2;
	float midY = min.y + (max.y - min.y) / 2;
	float midZ = min.z + (max.z - min.z) / 2;

	switch (handleType)
	{
	case CORNER_FAR_LEFT_BOTTOM:
		pos = Ogre::Vector3(min.x, min.y, min.z);
		break;
	case CORNER_FAR_LEFT_TOP:
		pos = Ogre::Vector3(min.x, max.y, min.z);
		break;
	case CORNER_FAR_RIGHT_TOP:
		pos = Ogre::Vector3(max.x, max.y, min.z);
		break;
	case CORNER_FAR_RIGHT_BOTTOM:
		pos = Ogre::Vector3(max.x, min.y, min.z);
		break;
	case CORNER_NEAR_RIGHT_BOTTOM:
		pos = Ogre::Vector3(max.x, min.y, max.z);
		break;
	case CORNER_NEAR_LEFT_BOTTOM:
		pos = Ogre::Vector3(min.x, min.y, max.z);
		break;
	case CORNER_NEAR_LEFT_TOP:
		pos = Ogre::Vector3(min.x, max.y, max.z);
		break;
	case CORNER_NEAR_RIGHT_TOP:
		pos = Ogre::Vector3(max.x, max.y, max.z);
		break;
	case FACE_X_POS:
		pos = Ogre::Vector3(max.x, midY, midZ);
		break;
	case FACE_X_NEG:
		pos = Ogre::Vector3(min.x, midY, midZ);
		break;
	case FACE_Y_POS:
		pos = Ogre::Vector3(midX, max.y, midZ);
		break;
	case FACE_Y_NEG:
		pos = Ogre::Vector3(midX, min.y, midZ);
		break;
	case FACE_Z_POS:
		pos = Ogre::Vector3(midX, midY, max.z);
		break;
	case FACE_Z_NEG:
		pos = Ogre::Vector3(midX, midY, min.z);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::PositionHandles(const Ogre::Vector3& min, const Ogre::Vector3& max)
{
	for (int i = 0; i < NUM_HANDLES; i++)
	{
		Ogre::Vector3 pos;
		GetHandlePosition(i, pos);
		m_HandleNode[i]->setPosition(pos.x, pos.y, pos.z);
	}
}

//////////////////////////////////////////////////////////////////////////
int ScaleGizmo::GetPartType(Ogre::Entity* part)
{
	for (int i = 0; i < NUM_HANDLES; i++)
	{
		if (m_HandleEnt[i] == part) return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::PreRender(Viewport* viewport, NodeSelection* selection)
{
	Camera* cam = viewport->GetCamera();
	if (!cam) return;

	float viewportSize = std::min((float)viewport->GetOgreViewport()->getActualWidth(), (float)viewport->GetOgreViewport()->getActualHeight());

	Ogre::Vector3 nodeScale = m_SceneNode->_getDerivedScale();

	for (int i = 0; i < NUM_HANDLES; i++)
	{
		float distance = (cam->GetOgreCamera()->getPosition() - m_HandleNode[i]->_getDerivedPosition()).length();
		distance /= 6000.0f;

		m_HandleNode[i]->setScale(distance / nodeScale.x, distance / nodeScale.y, distance / nodeScale.z);
	}
}

//////////////////////////////////////////////////////////////////////////
bool ScaleGizmo::GetDragOrigin(Ogre::Entity* part, Viewport* viewport)
{
	// get a ray from center point to selected handle, in world coordinates
	
	int type = GetPartType(part);
	if (type < 0) return false;

	m_DragCenter = m_Editor->GetSelection()->GetScaleOrigin();

	GetHandlePosition(type, m_DragOrigin);
	m_Editor->GetSelection()->GetBBox()->LocalToWorld(m_DragOrigin);

	m_DragRay = Ogre::Ray(m_DragCenter, m_DragOrigin - m_DragCenter);

	if (!viewport->GetMousePointOnRay(m_DragRay, m_DragOrigin))
		return false;

	m_DragStoredScale = m_Editor->GetSelection()->GetScale();
	m_LastScale = m_DragStoredScale;

	m_DragViewport = viewport;
	m_DragPart = part;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void ScaleGizmo::Reset()
{
	m_Editor->GetSelection()->ScaleBy(Ogre::Vector3::UNIT_SCALE * (m_DragStoredScale / m_LastScale));
}


} // namespace Wme

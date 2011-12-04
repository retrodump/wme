// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SceneNodeEditor.h"
#include "Scene3DBase.h"
#include "NodeSelection.h"
#include "Gizmo.h"
#include "MoveGizmo.h"
#include "RotGizmo.h"
#include "ScaleGizmo.h"
#include "Viewport.h"
#include "Game.h"
#include "ContentManager.h"
#include "Camera.h"
#include "Stage.h"
#include "StringUtil.h"
#include "InputManager.h"
#include "ElementCollection.h"
#include "LineElement.h"
#include "MousePickParams.h"
#include "Entity3DBase.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SceneNodeEditor::SceneNodeEditor(Scene3DBase* stage) : Scene3DEditor(stage)
{
	m_Selection = new NodeSelection(this);
	m_Mode = MODE_MOVE;

	m_GizmoLocalMode = true;

	m_MoveGizmo = NULL;
	m_RotGizmo = NULL;
	m_ScaleGizmo = NULL;

	m_DragGizmo = NULL;
	m_IsDragging = false;
	m_DragCancelled = false;

	m_BoxSelElement = NULL;
	m_BoxSel = NULL;
	m_IsBoxSelecting = false;
	m_IsBoxSelectValid = false;
	m_BoxSelStartX = m_BoxSelStartY = 0;
	m_BoxSelPrev = Ogre::Vector4::ZERO;
	m_VolumeQuery = NULL;

	m_MoveSnap = 10.0f;
	m_RotSnap = 15.0f; // degrees
	m_ScaleSnap = 10.0f;
}

//////////////////////////////////////////////////////////////////////////
SceneNodeEditor::~SceneNodeEditor()
{
	SAFE_DELETE(m_BoxSel);
	SAFE_DELETE(m_BoxSelElement);
	SAFE_DELETE(m_Selection);
	SAFE_DELETE(m_MoveGizmo);
	SAFE_DELETE(m_RotGizmo);
	SAFE_DELETE(m_ScaleGizmo);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::SetEditorMode(EditorMode mode)
{
	m_Mode = mode;
	UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::SetGizmoLocalMode(bool localMode)
{
	m_GizmoLocalMode = localMode;
	UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::Create()
{
	CreateGizmos();

	m_BoxSelElement = new ElementCollection();
	m_Stage->GetSystemRootNode()->attachObject(m_BoxSelElement);
	m_BoxSelElement->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE + 1);
	
	m_BoxSel = new LineElement();		
	m_BoxSelElement->AddElement(m_BoxSel);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::CreateGizmos()
{
	m_MoveGizmo = new MoveGizmo(this);
	m_MoveGizmo->Create();
	m_MoveGizmo->SetVisible(false, m_Selection);

	m_RotGizmo = new RotGizmo(this);
	m_RotGizmo->Create();
	m_RotGizmo->SetVisible(false, m_Selection);

	m_ScaleGizmo = new ScaleGizmo(this);
	m_ScaleGizmo->Create();
	m_ScaleGizmo->SetVisible(false, m_Selection);
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_PRESS)
	{
		Ogre::Entity* gizmoEntity = PickGizmo(viewport);

		if (gizmoEntity)
		{
			StartDrag(viewport, gizmoEntity);
			return true;
		}
		else
		{
			m_IsBoxSelecting = true;
			m_IsBoxSelectValid = false;
			Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);
			m_BoxSelPrev = Ogre::Vector4::ZERO;
			viewport->GetMousePos(m_BoxSelStartX, m_BoxSelStartY);
			UpdateGizmos();
			return true;
		}
	}
	else if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_RELEASE)
	{
		if (m_DragCancelled)
		{
			m_DragCancelled = false;
			return true;
		}
		else if (m_IsDragging)
		{
			EndDrag();
			return true;
		}
		else
		{
			m_IsBoxSelecting = false;			
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(viewport);

			if (!m_IsBoxSelectValid)
			{
				Ogre::Entity* entity = PickObject(viewport);
				if (entity)
				{
					Ogre::SceneNode* selNode = entity->getParentSceneNode();

					Entity3DBase* entity3d = Entity3DBase::OgreEntityToEntity(entity);
					if (entity3d) selNode = entity3d->GetSceneNode();

					if (Game::GetInstance()->GetContentMgr()->IsShiftDown() || Game::GetInstance()->GetContentMgr()->IsControlDown())
					{
						if (m_Selection->IsSelected(selNode))
							m_Selection->RemoveNode(selNode);
						else
							m_Selection->AddNode(selNode);
					}
					else
					{
						//if (!m_Selection->IsSelected(selNode))
							m_Selection->SetNode(selNode);
					}
				}
				else m_Selection->Clear();
			}

			m_IsBoxSelectValid = false;
			UpdateGizmos();
			return true;
		}
	}
	else if (event.GetButton() == MouseEvent::BUTTON_RIGHT && event.GetType() == MouseEvent::EVENT_PRESS)
	{
		if (!m_IsDragging)
		{
			if (m_Selection->IsEmpty()) m_Mode = MODE_NONE;
			else
			{
				if (m_Mode == MODE_MOVE) m_Mode = MODE_ROTATE;
				else if (m_Mode == MODE_ROTATE) m_Mode = MODE_SCALE;
				else m_Mode = MODE_MOVE;
			}

			UpdateGizmos();
			return true;
		}
		else
		{
			m_DragGizmo->Reset();
			EndDrag();
			UpdateGizmos();
			m_DragCancelled = true;
			
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event)
{
	if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_ESCAPE)
	{
		if (m_IsDragging)
		{
			m_DragGizmo->Reset();
			EndDrag();
			UpdateGizmos();
			m_DragCancelled = true;

			return true;
		}
	}

	if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_C)
	{
		SetGizmoLocalMode(!GetGizmoLocalMode());
		return true;
	}

	if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_F)
	{
		FocusSelection(viewport);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::Update()
{
	if (m_IsDragging)
	{
		m_DragGizmo->UpdateDrag();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::PreRender(Viewport* viewport)
{
	Gizmo* gizmo = GetCurrentGizmo();
	if (gizmo)
	{
		gizmo->PreRender(viewport, m_Selection);


		if (m_IsDragging) gizmo->HighlightDragPart();
		else
		{
			Ogre::Entity* gizmoEntity = PickGizmo(viewport);
			gizmo->HighlightPart(gizmoEntity);
		}
	}


	if (m_IsBoxSelecting)
	{
		int MouseX, MouseY;
		viewport->GetMousePos(MouseX, MouseY);

		if (MouseX == m_BoxSelStartX && MouseY == m_BoxSelStartY)
		{
			m_BoxSelElement->setVisible(false);
		}
		else
		{
			m_BoxSel->ClearLines();
			m_BoxSel->AddRectangle(Ogre::Vector2((float)m_BoxSelStartX, (float)m_BoxSelStartY), Ogre::Vector2((float)MouseX, (float)MouseY));
			m_BoxSel->UpdateGeometry(viewport, true);
			m_BoxSelElement->setVisible(true);

			if (abs(MouseX - m_BoxSelStartX) > 2 && abs(MouseY - m_BoxSelStartY) > 2)
			{
				m_IsBoxSelectValid = true;
				UpdateGizmos();

				DoBoxSelect(viewport);
			}
		}
	}
	else m_BoxSelElement->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::PreparePick(Viewport* viewport, MousePickParams& pickParams, Stage*& stage)
{
	Camera* cam = viewport->GetCamera();
	if (!cam) return false;

	stage = cam->GetStage();
	if (!stage) return false;

	float x, y;
	viewport->GetRelativeMousePos(x, y);

	Ogre::Ray mouseRay = cam->GetOgreCamera()->getCameraToViewportRay(x, y);

	pickParams.Point = mouseRay.getPoint(0);
	pickParams.Normal = mouseRay.getDirection();
	pickParams.RenderQueueId = -1;
	pickParams.QueryUV = false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Entity* SceneNodeEditor::PickGizmo(Viewport* viewport)
{
	Stage* stage;
	MousePickParams pickParams;
	if (!PreparePick(viewport, pickParams, stage)) return NULL;

	pickParams.RenderQueueId = Gizmo::GIZMO_RENDER_QUEUE_ID;
	
	if (stage->RaycastFromPoint(pickParams)) return pickParams.ResultEntity;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Entity* SceneNodeEditor::PickObject(Viewport* viewport)
{
	Stage* stage;
	MousePickParams pickParams;
	if (!PreparePick(viewport, pickParams, stage)) return NULL;

	if (stage->RaycastFromPoint(pickParams)) return pickParams.ResultEntity;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::PlaceGizmo()
{
	Gizmo* gizmo = GetCurrentGizmo();
	if (!gizmo) return;

	gizmo->ConformTo(m_Selection);
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::StartDrag(Viewport* viewport, Ogre::Entity* gizmoEntity)
{
	Gizmo* gizmo = GetCurrentGizmo();
	if (!gizmo) return;

	if (gizmo->StartDrag(viewport, gizmoEntity))
	{
		m_DragGizmo = gizmo;
		m_IsDragging = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::EndDrag()
{
	if (m_DragGizmo) m_DragGizmo->EndDrag();
	
	m_IsDragging = false;
	m_DragGizmo = NULL;
}


//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::UpdateGizmos()
{
	bool isSelection = !m_Selection->IsEmpty() && !m_IsBoxSelectValid;
	Gizmo* gizmo = GetCurrentGizmo();

	m_MoveGizmo->SetVisible(isSelection && m_MoveGizmo == gizmo && m_Selection->IsMovable(), m_Selection);
	m_RotGizmo->SetVisible(isSelection && m_RotGizmo == gizmo && m_Selection->IsRotatable(), m_Selection);
	m_ScaleGizmo->SetVisible(isSelection && m_ScaleGizmo == gizmo && m_Selection->IsScalable(), m_Selection);

	PlaceGizmo();
}

//////////////////////////////////////////////////////////////////////////
Gizmo* SceneNodeEditor::GetCurrentGizmo()
{
	switch (m_Mode)
	{
	case MODE_MOVE:
		return m_MoveGizmo;

	case MODE_ROTATE:
		return m_RotGizmo;

	case MODE_SCALE:
		return m_ScaleGizmo;

	default:
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::DoBoxSelect(Viewport* viewport)
{
	Ogre::Vector2 corner1 = viewport->ConvertMouseToRelative(m_BoxSelStartX, m_BoxSelStartY);
	Ogre::Vector2 corner2;
	viewport->GetRelativeMousePos(corner2.x, corner2.y);

	float left = std::min(corner1.x, corner2.x);
	float right = std::max(corner1.x, corner2.x);
	float top = std::min(corner1.y, corner2.y);
	float bottom = std::max(corner1.y, corner2.y);

	// only do the selection shuffle if the selection rectangle changed (it's slooow)
	Ogre::Vector4 box(left, right, top, bottom);
	if (box == m_BoxSelPrev) return;
	m_BoxSelPrev = box;


	Camera* cam = viewport->GetCamera();
	if (!cam) return;

	Ogre::Ray topLeft = cam->GetOgreCamera()->getCameraToViewportRay(left, top);
	Ogre::Ray topRight = cam->GetOgreCamera()->getCameraToViewportRay(right, top);
	Ogre::Ray bottomLeft = cam->GetOgreCamera()->getCameraToViewportRay(left, bottom);
	Ogre::Ray bottomRight = cam->GetOgreCamera()->getCameraToViewportRay(right, bottom);


	Ogre::PlaneBoundedVolume vol;
	vol.planes.push_back(Ogre::Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));        // front plane
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), topLeft.getPoint(20), topRight.getPoint(20)));          // top plane
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), bottomLeft.getPoint(20), topLeft.getPoint(20)));        // left plane
	vol.planes.push_back(Ogre::Plane(bottomLeft.getOrigin(), bottomRight.getPoint(20), bottomLeft.getPoint(20))); // bottom plane
	vol.planes.push_back(Ogre::Plane(topRight.getOrigin(), topRight.getPoint(20), bottomRight.getPoint(20)));     // right plane   

	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);

	if (!m_VolumeQuery) m_VolumeQuery = m_Stage->GetSceneMgr()->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	
	m_VolumeQuery->setVolumes(volList);
	Ogre::SceneQueryResult queryResult = m_VolumeQuery->execute();

	m_Selection->StartBulkSelection();
	m_Selection->Clear(false);
	foreach (Ogre::MovableObject* obj, queryResult.movables)
	{
		if (obj->getMovableType() == "Entity" && obj->isVisible())
		{
			// skip editing gizmos
			if (!obj->getUserObjectBindings().getUserAny("pickable").isEmpty())
			{
				PickableObject pickable = Ogre::any_cast<PickableObject>(obj->getUserObjectBindings().getUserAny("pickable"));
				if (pickable.GetType() == PickableObject::PICKABLE_GIZMO) continue;
			}

			Entity3DBase* entity = Entity3DBase::OgreEntityToEntity(static_cast<Ogre::Entity*>(obj));
			if (entity) m_Selection->AddNode(entity->GetSceneNode());
			else m_Selection->AddNode(static_cast<Ogre::Entity*>(obj)->getParentSceneNode());
		}
	}
	m_Selection->EndBulkSelection();
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::OnSceneNodeDestroyed(Ogre::SceneNode* node)
{
	m_Selection->RemoveNode(node);
	UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
void SceneNodeEditor::FocusSelection(Viewport* viewport)
{
	if (m_Selection->IsEmpty()) return;

	Camera* cam = viewport->GetCamera();
	if (!cam) return;

	Ogre::Vector3 size = m_Selection->GetHalfSize() * 2.0f;
	Ogre::Vector3 center = m_Selection->GetCenter();

	float dist = std::max(std::max(size.x, size.y), size.z) * 2.0f;

	Ogre::Vector3 lookat = (cam->GetOgreCamera()->getDerivedDirection() * dist);
	cam->GetOgreCamera()->setPosition(center - lookat);
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::IsNodeMovable(Ogre::SceneNode* node) const
{
	if (node == m_Stage->GetRootNode()) return false;

	Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
	if (entity) return entity->IsEditorMovable();
	else return true;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::IsNodeRotatable(Ogre::SceneNode* node) const
{
	if (node == m_Stage->GetRootNode()) return false;

	Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
	if (entity) return entity->IsEditorRotatable();
	else return true;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::IsNodeScalable(Ogre::SceneNode* node) const
{
	if (node == m_Stage->GetRootNode()) return false;

	Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
	if (entity) return entity->IsEditorScalable();
	else return true;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNodeEditor::IsNodeMeshBased(Ogre::SceneNode* node) const
{
	if (!node) return false;

	Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
	if (entity) return entity->IsMeshBased();

	for (unsigned short i = 0; i < node->numAttachedObjects(); i++)
	{
		Ogre::MovableObject* obj = node->getAttachedObject(i);
		if (obj->getMovableType() == "Entity")
			return true;
	}
	return false;
}


} // namespace Wme

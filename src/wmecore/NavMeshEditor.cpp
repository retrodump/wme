// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Scene3DBase.h"
#include "NavMeshEditor.h"
#include "NavMesh.h"
#include "NavNode.h"
#include "NavEdge.h"
#include "NavMeshRenderer.h"
#include "Camera.h"
#include "Viewport.h"
#include "Game.h"
#include "ContentManager.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavMeshEditor::NavMeshEditor(Scene3DBase* stage) : Scene3DEditor(stage)
{
	m_Mode = MODE_NODES;

	m_NavMesh = NULL;
	m_NavMeshRenderer = NULL;
	m_HoverNode = NULL;
	m_HoverEdge = NULL;

	m_IsDragging = false;
	m_DragSelecting = false;
}

//////////////////////////////////////////////////////////////////////////
NavMeshEditor::~NavMeshEditor()
{
	if (m_NavMesh) m_NavMesh->RemoveListener(this);

	m_NavMesh = NULL;
	m_HoverNode = NULL;
	SAFE_DELETE(m_NavMeshRenderer);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::Create()
{
	m_NavMeshRenderer = new NavMeshRenderer(this);
	m_NavMeshRenderer->Create();
}

//////////////////////////////////////////////////////////////////////////
bool NavMeshEditor::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_PRESS)
	{
		SetHover(viewport);
		if (m_HoverNode && m_Mode == MODE_NODES)
		{
			m_DragSelecting = !IsNodeSelected(m_HoverNode);
			m_IsDragging = true;
			Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);
			return true;
		}
		else if (m_HoverEdge && m_Mode == MODE_EDGES)
		{
			m_DragSelecting = !IsEdgeSelected(m_HoverEdge);
			m_IsDragging = true;
			Game::GetInstance()->GetContentMgr()->CaptureMouse(viewport);
			return true;
		}
	}
	else if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_RELEASE)
	{
		if (m_IsDragging)
		{
			m_IsDragging = false;
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(viewport);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool NavMeshEditor::HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event)
{
	if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_SPACE)
	{
		ClearSelectedNodes();
		ClearSelectedEdges();
		m_NavMeshRenderer->UpdateGeometry();
		return true;
	}

	else if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_B)
	{
		switch (m_Mode)
		{
		case MODE_NODES:
			foreach (NavNode* node, m_SelectedNodes) node->SetActive(!node->IsActive());
			ClearSelectedNodes();
			break;
		case MODE_EDGES:
			foreach (NavEdge* edge, m_SelectedEdges) edge->SetActive(!edge->IsActive());
			ClearSelectedEdges();
			break;
		}
		m_NavMeshRenderer->UpdateGeometry();
	}

	else if (event.GetType() == KeyboardEvent::EVENT_PRESS && event.GetKeyCode() == OIS::KC_M)
	{
		switch (m_Mode)
		{
		case MODE_NODES:
			m_Mode = MODE_EDGES;
			ClearSelectedNodes();
			break;
		case MODE_EDGES:
			m_Mode = MODE_NODES;
			ClearSelectedEdges();
			break;
		}
		m_NavMeshRenderer->UpdateGeometry();
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::Update()
{

}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::PreRender(Viewport* viewport)
{
	SetHover(viewport);

	if (m_IsDragging)
	{
		// selecting nodes
		if (m_HoverNode && m_Mode == MODE_NODES)
		{
			if (m_DragSelecting)
			{
				if (!IsNodeSelected(m_HoverNode))
				{
					AddSelectedNode(m_HoverNode);
					m_NavMeshRenderer->UpdateGeometry();
				}
			}
			else
			{
				if (IsNodeSelected(m_HoverNode))
				{
					RemoveSelectedNode(m_HoverNode);
					m_NavMeshRenderer->UpdateGeometry();
				}
			}
		}

		// selecting edges
		if (m_HoverEdge && m_Mode == MODE_EDGES)
		{
			if (m_DragSelecting)
			{
				if (!IsEdgeSelected(m_HoverEdge))
				{
					AddSelectedEdge(m_HoverEdge);
					m_NavMeshRenderer->UpdateGeometry();
				}
			}
			else
			{
				if (IsEdgeSelected(m_HoverEdge))
				{
					RemoveSelectedEdge(m_HoverEdge);
					m_NavMeshRenderer->UpdateGeometry();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::SetHover(Viewport* viewport)
{
	if (!m_NavMesh || !m_NavMeshRenderer) return;

	Camera* cam = viewport->GetCamera();
	if (!cam) return;

	float x, y;
	viewport->GetRelativeMousePos(x, y);
	Ogre::Ray mouseRay = cam->GetOgreCamera()->getCameraToViewportRay(x, y);

	NavNode* hoverNode;
	NavEdge* hoverEdge;
	if (!m_NavMesh->GetIntersection(mouseRay, &hoverNode, &hoverEdge))
	{
		hoverNode = NULL;
		hoverEdge = NULL;
	}

	if (hoverNode != m_HoverNode || hoverEdge != m_HoverEdge)
	{
		m_HoverNode = hoverNode;
		m_HoverEdge = hoverEdge;
		m_NavMeshRenderer->UpdateGeometry();
	}
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::SetNavMesh(NavMesh* navMesh)
{
	if (m_NavMesh) m_NavMesh->RemoveListener(this);

	ClearSelectedNodes();
	ClearSelectedEdges();

	m_NavMesh = navMesh;
	m_NavMesh->AddListener(this);

	m_HoverNode = NULL;
	m_HoverEdge = NULL;
	m_NavMeshRenderer->UpdateGeometry();
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::OnPathFound(const NavMesh::PathPointList& path)
{
	m_NavMeshRenderer->SetPath(path);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::OnDestroy()
{
	ClearSelectedNodes();
	ClearSelectedEdges();

	m_HoverNode = NULL;
	m_HoverEdge = NULL;
	m_NavMesh = NULL;
}

//////////////////////////////////////////////////////////////////////////
NavNode* NavMeshEditor::GetHoverNode()
{
	if (m_Mode == MODE_NODES) return m_HoverNode;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
NavEdge* NavMeshEditor::GetHoverEdge()
{
	if (m_Mode == MODE_EDGES) return m_HoverEdge;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::SetSelectedNode(NavNode* node)
{
	ClearSelectedNodes();
	AddSelectedNode(node);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::AddSelectedNode(NavNode* node)
{
	if (!IsNodeSelected(node)) m_SelectedNodes.insert(node);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::RemoveSelectedNode(NavNode* node)
{
	if (IsNodeSelected(node)) m_SelectedNodes.erase(node);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::ClearSelectedNodes()
{
	m_SelectedNodes.clear();
}

//////////////////////////////////////////////////////////////////////////
bool NavMeshEditor::IsNodeSelected(NavNode* node) const
{
	return (m_SelectedNodes.find(node) != m_SelectedNodes.end());
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::SetSelectedEdge(NavEdge* edge)
{
	ClearSelectedEdges();
	AddSelectedEdge(edge);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::AddSelectedEdge(NavEdge* edge)
{
	if (!IsEdgeSelected(edge)) m_SelectedEdges.insert(edge);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::RemoveSelectedEdge(NavEdge* edge)
{
	if (IsEdgeSelected(edge)) m_SelectedEdges.erase(edge);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshEditor::ClearSelectedEdges()
{
	m_SelectedEdges.clear();
}

//////////////////////////////////////////////////////////////////////////
bool NavMeshEditor::IsEdgeSelected(NavEdge* edge) const
{
	return (m_SelectedEdges.find(edge) != m_SelectedEdges.end());
}


} // namespace Wme

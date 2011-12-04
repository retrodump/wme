// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NavMeshRenderer.h"
#include "NavMeshEditor.h"
#include "NavMesh.h"
#include "NavNode.h"
#include "Stage.h"
#include "Viewport.h"
#include "LineElement3D.h"
#include "TriangleElement3D.h"
#include "PathFinder.h"
#include "Game.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavMeshRenderer::NavMeshRenderer(NavMeshEditor* editor)
{
	m_Editor = editor;

	m_SceneNode = NULL;
	m_LineRenderer = NULL;
	m_TriRenderer = NULL;
	m_PathRenderer = NULL;


	// colors
	m_ColPath = Ogre::ColourValue(1.0f, 0.0f, 1.0f);

	m_ColNormalNode = Ogre::ColourValue(0.0f, 0.0f, 1.0f, 0.5f);
	m_ColBlockedNode = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.5f);
	m_ColSelectedNode = Ogre::ColourValue(1.0f, 0.0f, 0.0f, 0.5f);
	m_ColHoverNode = Ogre::ColourValue(0.0f, 1.0f, 0.0f, 0.5f);
	m_ColHoverSelectedNode = Ogre::ColourValue(1.0f, 1.0f, 0.0f, 0.5f);

	m_ColNormalEdge = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.3f);
	m_ColOuterEdge = Ogre::ColourValue::White;
	m_ColBlockedEdge = Ogre::ColourValue::Black;
	m_ColSelectedEdge = Ogre::ColourValue(1.0f, 0.0f, 0.0f);
	m_ColHoverEdge = Ogre::ColourValue(0.0f, 1.0f, 0.0f);;
	m_ColHoverSelectedEdge = Ogre::ColourValue(1.0f, 1.0f, 0.0f);;
}

//////////////////////////////////////////////////////////////////////////
NavMeshRenderer::~NavMeshRenderer()
{
	if (m_SceneNode)
	{
		m_SceneNode->detachAllObjects();
		m_SceneNode->getCreator()->destroySceneNode(m_SceneNode);
		m_SceneNode = NULL;
	}
	SAFE_DELETE(m_LineRenderer);
	SAFE_DELETE(m_TriRenderer);
	SAFE_DELETE(m_PathRenderer);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshRenderer::Create()
{
	m_LineRenderer = new LineElement3D;
	m_TriRenderer = new TriangleElement3D;
	m_PathRenderer = new LineElement3D;
	m_SceneNode = m_Editor->GetStage()->GetSystemRootNode()->createChildSceneNode();
	m_SceneNode->attachObject(m_LineRenderer);
	m_SceneNode->attachObject(m_TriRenderer);
	m_SceneNode->attachObject(m_PathRenderer);
	
	m_TriRenderer->setRenderQueueGroup(NAVMESH_RENDER_QUEUE_ID);
	m_LineRenderer->setRenderQueueGroup(NAVMESH_RENDER_QUEUE_ID + 1);
	m_PathRenderer->setRenderQueueGroup(NAVMESH_RENDER_QUEUE_ID + 2);

	m_PathRenderer->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////
void NavMeshRenderer::UpdateGeometry()
{
	if (!m_LineRenderer || !m_TriRenderer) return;

	m_LineRenderer->ClearLines();
	m_TriRenderer->ClearTriangles();

	if (!m_Editor->GetNavMesh())
	{
		m_LineRenderer->UpdateGeometry();
		m_TriRenderer->UpdateGeometry();
		return;
	}


	foreach (NavNode* node, m_Editor->GetNavMesh()->m_Nodes)
	{
		Ogre::ColourValue nodeColor;
		bool selectedNode = m_Editor->IsNodeSelected(node);

		if (selectedNode)
		{
			if (node == m_Editor->GetHoverNode()) nodeColor = m_ColHoverSelectedNode;
			else nodeColor = m_ColSelectedNode;
		}
		else
		{
			if (node == m_Editor->GetHoverNode()) nodeColor = m_ColHoverNode;
			else nodeColor = node->IsActive() ? m_ColNormalNode : m_ColBlockedNode;
		}	


		// nodes
		m_TriRenderer->AddTriangle(node->m_Points[0], node->m_Points[1], node->m_Points[2], nodeColor);


		// edges
		for (int i = 0; i < 3; i++)
		{
			NavEdge* edge = node->GetEdge(i);
			if (!edge) continue;

			Ogre::Vector3 point1, point2;
			node->GetEdgePoints(i, point1, point2);

			NavNode::EdgeType edgeType = node->GetEdgeType(i);
			if (edgeType == NavNode::EDGE_INNER_SECONDARY) continue;

			bool isActive = node->IsEdgeActive(i);
			bool isSelected = m_Editor->IsEdgeSelected(edge);

			Ogre::ColourValue edgeColor = m_ColOuterEdge;

			switch (edgeType)
			{
			case NavNode::EDGE_OUTER:
				edgeColor = m_ColOuterEdge;
				break;
			case NavNode::EDGE_INNER_PRIMARY:
				edgeColor = m_ColNormalEdge;
				break;
			}
			if (!isActive) edgeColor = m_ColBlockedEdge;

			

			if (m_Editor->GetHoverEdge() == edge)
			{
				edgeColor = isSelected ? m_ColHoverSelectedEdge : m_ColHoverEdge;
			}
			else if (isSelected) edgeColor = m_ColSelectedEdge;

			m_LineRenderer->AddLine(point1, point2, edgeColor);
		}		
	}

	m_LineRenderer->UpdateGeometry();
	m_LineRenderer->SetDepthCheck(false);

	m_TriRenderer->UpdateGeometry();
	m_TriRenderer->SetDepthCheck(false);

}

//////////////////////////////////////////////////////////////////////////
void NavMeshRenderer::SetPath(const NavMesh::PathPointList& path)
{
	m_PathRenderer->ClearLines();

	Ogre::Vector3 prevPoint = Ogre::Vector3::ZERO;

	foreach (NavMesh::PathPoint point, path)
	{
		if (prevPoint != Ogre::Vector3::ZERO)
		{
			m_PathRenderer->AddLine(prevPoint, point.Point, m_ColPath);
		}
		prevPoint = point.Point;
	}

	m_PathRenderer->SetDepthCheck(false);
	m_PathRenderer->UpdateGeometry();
	m_PathRenderer->setVisible(!path.empty());
}


//////////////////////////////////////////////////////////////////////////
void NavMeshRenderer::PathTest()
{
	/*
	NavMesh::PathPointList path;

	Ogre::Vector3 startPoint(-376.769f, 1.14929e-005f, 199.63f);
	Ogre::Vector3 endPoint(394.911f, 1.61963f, 93.2925f);


	//Ogre::Vector3 startPoint(49.4723f, 1.14929e-005f, 360.861f);
	//Ogre::Vector3 endPoint(484.768f, 1.61963f, 80.837f);


	//Ogre::Vector3 startPoint(86, 0, 309);
	//Ogre::Vector3 endPoint(600, 0, 65);

	//Ogre::Vector3 endPoint(131, 0, -244);
	//Ogre::Vector3 endPoint(297, 0, 464);
	//Ogre::Vector3 endPoint(424, 0, 152);

	
	//Ogre::Vector3 startPoint(223, 0, -266);
	//Ogre::Vector3 endPoint(565, 0, -227);
	


	
	Game::GetInstance()->Profile(L"start");
	m_NavMesh->FindPath(NULL, startPoint, NULL, endPoint, path);
	Game::GetInstance()->Profile(L"end");

	SetPath(path);


	NavNode* startNode, *endNode;
	m_NavMesh->MovePointToMesh(startPoint, &startNode);
	m_NavMesh->MovePointToMesh(endPoint, &endNode);

	m_SelectedNodes.insert(startNode);
	m_SelectedNodes.insert(endNode);
	*/
}


} // namespace Wme
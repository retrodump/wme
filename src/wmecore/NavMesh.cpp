// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NavMesh.h"
#include "NavNode.h"
#include "LineElement3D.h"
#include "Stage.h"
#include "Viewport.h"
#include "PathFinder.h"
#include "CardinalSpline.h"
#include "Game.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavMesh::NavMesh()
{
}

//////////////////////////////////////////////////////////////////////////
NavMesh::~NavMesh()
{
	foreach (Listener* listener, m_Listeners) listener->OnDestroy();

	Clear();
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::Clear()
{
	foreach (NavNode* node, m_Nodes)
	{
		SAFE_DELETE(node);
	}
	m_Nodes.clear();
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::AddNode(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC)
{
	NavNode* node = new NavNode();
	node->Initialize(pointA, pointB, pointC);

	m_Nodes.push_back(node);
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::AutoLinkNodes()
{
	foreach (NavNode* node1, m_Nodes)
	{
		foreach (NavNode* node2, m_Nodes)
		{
			if (node1 == node2) continue;

			node1->AttemptLink(node2);
		}
		node1->FinalizeLinks();
	}
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::LoadFromOgreMesh(Ogre::MeshPtr mesh)
{
	Clear();


	for (unsigned short subMeshIndex = 0; subMeshIndex < mesh->getNumSubMeshes(); subMeshIndex++)
	{
		Ogre::SubMesh* subMesh = mesh->getSubMesh(subMeshIndex);

		// get index buffer
		Ogre::IndexData* indexData = subMesh->indexData;

		size_t numTris = indexData->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr indexBuf = indexData->indexBuffer;

		bool use32bitIndexes = (indexBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
		unsigned long*  pLong = static_cast<unsigned long*>(indexBuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


		// get vertex buffer
		Ogre::VertexData* vertexData = subMesh->useSharedVertices ? mesh->sharedVertexData : subMesh->vertexData;
		const Ogre::VertexElement* posElem = vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

		Ogre::HardwareVertexBufferSharedPtr vertexBuf =	vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
		byte* vertex = static_cast<byte*>(vertexBuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));


		for (size_t i = 0; i < numTris; i++)
		{
			// tri vertices
			Ogre::Vector3 point[3];

			for (int pointIndex = 0; pointIndex < 3; pointIndex++)
			{
				unsigned long index;

				if (use32bitIndexes)
					index = pLong[i * 3 + pointIndex];
				else
					index = static_cast<unsigned long>(pShort[i * 3 + pointIndex]);

				byte* vertexPos = vertex + index * vertexBuf->getVertexSize();
				float* floatData;

				posElem->baseVertexPointerToElement(vertexPos, &floatData);
				point[pointIndex] = Ogre::Vector3(floatData[0], floatData[1], floatData[2]);
			}

			// we assume clocwise vertex order
			AddNode(point[0], point[1], point[2]);
		}

		vertexBuf->unlock();
		indexBuf->unlock();
	}

	AutoLinkNodes();
}

//////////////////////////////////////////////////////////////////////////
NavNode* NavMesh::GetNearestNode(const Ogre::Vector3& point) const
{
	bool foundExactMatch = false;
	bool foundApproxMatch = false;
	Ogre::Real closestHeight;
	Ogre::Real closestDistance;
	NavNode* nearestNode = NULL;

	foreach (NavNode* node, m_Nodes)
	{
		if (!node->IsActive()) continue;

		if (node->IsPointWithinNode(point))
		{
			Ogre::Vector3 pointOnNode = point;
			node->AdjustPointHeight(pointOnNode);

			Ogre::Real heightDiff = Ogre::Math::Abs(point.y - pointOnNode.y);

			if (foundExactMatch)
			{
				if (heightDiff < closestHeight)
				{
					closestHeight = heightDiff;
					nearestNode = node;
				}
			}
			else
			{
				closestHeight = heightDiff;
				nearestNode = node;
				foundExactMatch = true;			
			}
		}
		else
		{
			if (!foundExactMatch)
			{
				Ogre::Vector2 start(node->GetCenterPoint().x, node->GetCenterPoint().z);
				Ogre::Vector2 end(point.x, point.z);
				Line2D movementLine(start, end);
				
				NavNode* nextNode;
				NavNode::EdgeSide edgeHit;
				Ogre::Vector2 intersectionPoint;

				NavNode::LineClassification result = node->ClassifyLine(movementLine, &nextNode, edgeHit, &intersectionPoint);

				if (result == NavNode::EXITING_NODE)
				{
					Ogre::Vector3 closestPoint3D(intersectionPoint.x, 0.0f, intersectionPoint.y);
					node->AdjustPointHeight(closestPoint3D);

					Ogre::Real distance = (closestPoint3D - point).length();

					if (!foundApproxMatch || distance < closestDistance)
					{
						closestDistance = distance;
						nearestNode = node;
						foundApproxMatch = true;
					}
				}
			}
		}
	}

	return nearestNode;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 NavMesh::MovePointToMesh(const Ogre::Vector3& point, NavNode** node) const
{
	NavNode* nearestNode = GetNearestNode(point);
	Ogre::Vector3 newPoint = MovePointToNode(point, nearestNode);

	if (node != NULL) *node = nearestNode;
	return newPoint;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 NavMesh::MovePointToNode(const Ogre::Vector3& point, NavNode* node) const
{
	Ogre::Vector3 newPoint = point;

	if (!node->IsPointWithinNode(newPoint))
	{
		node->MovePointToNode(newPoint);
	}

	node->AdjustPointHeight(newPoint);
	return newPoint;
}

//////////////////////////////////////////////////////////////////////////
bool NavMesh::StraightLineExists(NavNode* startNode, const Ogre::Vector3& startPoint, NavNode* endNode, const Ogre::Vector3& endPoint) const
{
	if (startNode == endNode) return true;

	Line2D motionPath(Ogre::Vector2(startPoint.x, startPoint.z), Ogre::Vector2(endPoint.x, endPoint.z));
	NavNode* nextNode = startNode;
	NavNode::EdgeSide edge;
	NavNode::LineClassification result;

	while((result = nextNode->ClassifyLine(motionPath, &nextNode, edge, 0)) == NavNode::EXITING_NODE)
	{		
		if (!nextNode) return false;
		if (!nextNode->IsActive()) return false;
		if (nextNode == endNode) return true;
	}

	return (result == NavNode::ENDING_IN_NODE);
}

//////////////////////////////////////////////////////////////////////////
bool NavMesh::FindPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, PathPointList& path) const
{
	return FindPath(NULL, startPoint, NULL, endPoint, path);
}

//////////////////////////////////////////////////////////////////////////
bool NavMesh::FindPath(NavNode* startNode, const Ogre::Vector3& startPoint, NavNode* endNode, const Ogre::Vector3& endPoint, PathPointList& finalPath) const
{
	Ogre::Vector3 start, end;
	
	/*
	Game::GetInstance()->Log(L"");
	Game::GetInstance()->Log(L"from: " + StringUtil::ToString(startPoint));
	Game::GetInstance()->Log(L"to: " + StringUtil::ToString(endPoint));
	*/
	
	if (!startNode)
		start = MovePointToMesh(startPoint, &startNode);
	else
		start = MovePointToNode(startPoint, startNode);

	if (!endNode)
		end = MovePointToMesh(endPoint, &endNode);
	else
		end = MovePointToNode(endPoint, endNode);


	PathFinder* pf = new PathFinder;
	pf->Initialize(this);


	typedef std::list<void*> PathNodes;
	PathNodes pathNodes;
	bool exactPath = pf->FindPath(startNode, endNode, pathNodes);


	PathPointList path;
	
	size_t count = 0;
	NavNode* prevNode = NULL;
	foreach (void* node, pathNodes)
	{
		NavNode* navNode = static_cast<NavNode*>(node);
		PathPoint pathPoint;
		pathPoint.Node = navNode;

		// first node?
		if (count == 0)
		{
			pathPoint.Point = start;
			pathPoint.EdgeLine = -1;
		}
		// last node?
		else if (count == pathNodes.size() - 1)
		{
			if (navNode != endNode) end = MovePointToNode(endPoint, navNode);
			pathPoint.Point = end;
			pathPoint.EdgeLine = -1;
		}
		else
		{
			pathPoint.Point = prevNode->GetPortalPointTo(navNode);
			pathPoint.EdgeLine = prevNode->GetEdgeTo(navNode);
		}

		path.push_back(pathPoint);

		prevNode = navNode;
		count++;
	}

	delete pf;

	finalPath.clear();


	//foreach (PathPoint point, path) finalPath.push_back(point);
	//OptimizePathRubberBand(path, finalPath);


	PathPointList rubberBandPath;
	OptimizePathRubberBand(path, rubberBandPath);

	PathPointList straightPath;
	OptimizeStraightLines(rubberBandPath, straightPath);

	OptimizeSpline(straightPath, finalPath, 4);


	// let listeners know we have a new path
	foreach (Listener* listener, m_Listeners) listener->OnPathFound(finalPath);


	return exactPath && start == startPoint && end == endPoint;
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::OptimizePathRubberBand(const PathPointList& origPath, PathPointList& newPath) const
{
	PathPoint pointA, pointB, pointC;

	int count = 0;
	foreach (PathPoint point, origPath)
	{
		if (count == 0)
		{
			pointA = point;
			newPath.push_back(point);
			count++;
			continue;
		}
		else if (count == 1)
		{
			pointB = point;
			count++;
			continue;
		}
		else if (count == 2)
		{
			pointC = point;
		}
		else
		{
			pointA = pointB;
			pointB = pointC;
			pointC = point;
		}

		
		if (!RubberBandPoint(pointA, pointB, pointC)) newPath.push_back(pointB);
		else pointB = pointA;
		
		count++;
		if (count == origPath.size()) newPath.push_back(pointC);
	}

	if (count < 3)
	{
		newPath.clear();
		foreach (PathPoint point, origPath)	newPath.push_back(point);
	}
}

//////////////////////////////////////////////////////////////////////////
bool NavMesh::RubberBandPoint(const PathPoint& pointA, PathPoint& pointB, const PathPoint& pointC) const
{
	if (pointB.EdgeLine < 0) return false;

	if (!StraightLineExists(pointA.Node, pointA.Point, pointB.Node, pointB.Point)) return false;

	Line2D edgeLine = pointB.Node->GetEdgeLine(pointB.EdgeLine);
	Line2D pathLine(Ogre::Vector2(pointA.Point.x, pointA.Point.z), Ogre::Vector2(pointC.Point.x, pointC.Point.z));

	Ogre::Vector2 intersection;
	Line2D::LineClassification result = edgeLine.Intersects(pathLine, &intersection);

	switch (result)
	{
	case Line2D::SEGMENTS_INTERSECT:
		return true;
	
	case Line2D::PARALLEL:
	case Line2D::COLLINEAR:
		return false;
	
	default:
		{
			// move intersection to the nearest edge start/end point
			float distanceStart = (intersection - edgeLine.GetStartPoint()).length();
			float distanceEnd = (intersection - edgeLine.GetEndPoint()).length();

			Ogre::Vector2 newPoint, origin, direction;
			if (Ogre::Math::Abs(distanceStart) < Ogre::Math::Abs(distanceEnd))
			{
				newPoint = edgeLine.GetStartPoint();
				origin = edgeLine.GetEndPoint();
			}
			else
			{
				newPoint = edgeLine.GetEndPoint();
				origin = edgeLine.GetStartPoint();
			}

			direction = newPoint - origin;

			direction *= 0.999f; // almost to the edge point
			intersection = origin + direction;
		}
	}

	pointB.Point = Ogre::Vector3(intersection.x, 0.0f, intersection.y);
	pointB.Node->AdjustPointHeight(pointB.Point);

	return false;
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::OptimizeStraightLines(const PathPointList& origPath, PathPointList& newPath) const
{
	if (origPath.empty()) return;

	PathPointIt currentPoint = origPath.begin();
	newPath.push_back(*currentPoint);

	while (currentPoint != origPath.end())
	{
		PathPointIt farthestPoint = GetFarthestStraightPoint(origPath, currentPoint);
		if (farthestPoint != origPath.end()) newPath.push_back(*farthestPoint);

		currentPoint = farthestPoint;
	}
}

//////////////////////////////////////////////////////////////////////////
NavMesh::PathPointIt NavMesh::GetFarthestStraightPoint(const PathPointList& path, const NavMesh::PathPointIt& initPoint) const
{
	if (initPoint == path.end()) return initPoint;

	PathPointIt testPoint = initPoint;
	testPoint++;

	if (testPoint == path.end()) return testPoint;

	PathPointIt lastValid = testPoint;
	testPoint++;

	while (testPoint != path.end())
	{
		PathPoint startPoint = *initPoint;
		PathPoint endPoint = *testPoint;

		if (!StraightLineExists(startPoint.Node, startPoint.Point, endPoint.Node, endPoint.Point))
		{
			return lastValid;
		}

		lastValid = testPoint;
		testPoint++;
	}
	return lastValid;
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::OptimizeSpline(const PathPointList& origPath, PathPointList& newPath, int numSegments) const
{
	CardinalSpline* spline = new CardinalSpline;
	spline->SetAutoCalculate(false);
	spline->SetTension(0.5f);

	foreach (PathPoint point, origPath)
	{
		spline->AddPoint(point.Point);
	}
	spline->RecalcTangents();

	for (unsigned short i = 0; i < spline->GetNumPoints(); i++)
	{
		for (int segment = 0; segment <= numSegments; segment++)
		{
			if (segment == numSegments && i < spline->GetNumPoints() - 1) continue;

			NavNode* node;
			Ogre::Vector3 point = spline->Interpolate(i, (float)segment / (float)numSegments);
			point = MovePointToMesh(point, &node);

			PathPoint pathPoint;
			pathPoint.Node = node;
			pathPoint.Point = point;
			pathPoint.EdgeLine = -1;

			newPath.push_back(pathPoint);
		}
	}

	delete spline;
}

//////////////////////////////////////////////////////////////////////////
bool NavMesh::GetIntersection(const Ogre::Ray& ray, NavNode** node, NavEdge** edge, Ogre::Vector3* point)
{
	float closestDist = 0.0f;
	float closestEdgeDist = 0.0f;
	NavNode* closestNode = NULL;
	NavEdge* closestEdge = NULL;

	foreach (NavNode* node, m_Nodes)
	{
		std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, node->GetPoint(0), node->GetPoint(1), node->GetPoint(2));
		if (hit.first)
		{
			if (closestNode == NULL || hit.second < closestDist)
			{
				closestNode = node;
				closestDist = hit.second;
			}
		}
	}

	if (closestNode == NULL)
	{
		if (node) *node = NULL;
		if (edge) *edge = NULL;
		if (point) *point = Ogre::Vector3::ZERO;

		return false;
	}

	Ogre::Vector3 hitPoint = ray.getPoint(closestDist);
	closestEdge = closestNode->GetNearestEdge(hitPoint);

	if (node) *node = closestNode;
	if (edge) *edge = closestEdge;
	if (point) *point = hitPoint;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::AddListener(Listener* listener)
{
	if (std::find(m_Listeners.begin(), m_Listeners.end(), listener) == m_Listeners.end())
		m_Listeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::RemoveListener(Listener* listener)
{
	ListenerList::iterator it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if (it != m_Listeners.end()) m_Listeners.erase(it);
}


//////////////////////////////////////////////////////////////////////////
// IPathGraph
//////////////////////////////////////////////////////////////////////////
float NavMesh::GetHeuristic(void* start, void* end) const
{
	NavNode* startNode = static_cast<NavNode*>(start);
	NavNode* endNode = static_cast<NavNode*>(end);

	return startNode->GetHeuristic(endNode);
}

//////////////////////////////////////////////////////////////////////////
void NavMesh::GetSuccessors(void* node, std::vector<PathConnection>& successors) const
{
	successors.clear();

	NavNode* navNode = static_cast<NavNode*>(node);
	navNode->GetSuccessors(successors);
}


} // namespace Wme

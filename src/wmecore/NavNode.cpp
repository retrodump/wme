// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NavNode.h"
#include "NavEdge.h"
#include "MathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavNode::NavNode()
{
	for (int i = 0; i < 3; i++)
	{
		m_Points[i] = Ogre::Vector3::ZERO;
		m_Edges[i] = NULL;
		m_EdgeMidPoints[i] = Ogre::Vector3::ZERO;
		m_EdgeDistances[i] = 0.0f;
	}
	m_CenterPoint = Ogre::Vector3::ZERO;
	m_Active = true;
}

//////////////////////////////////////////////////////////////////////////
NavNode::~NavNode()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i]) m_Edges[i]->RemoveNode(this);
	}
}

//////////////////////////////////////////////////////////////////////////
void NavNode::Initialize(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC)
{
	m_Points[0] = pointA;
	m_Points[1] = pointB;
	m_Points[2] = pointC;


	Ogre::Vector2 point2DA(pointA.x, pointA.z);
	Ogre::Vector2 point2DB(pointB.x, pointB.z);
	Ogre::Vector2 point2DC(pointC.x, pointC.z);

	m_EdgeLines[EDGE_AB].SetPoints(point2DA, point2DB);
	m_EdgeLines[EDGE_BC].SetPoints(point2DB, point2DC);
	m_EdgeLines[EDGE_CA].SetPoints(point2DC, point2DA);

	m_EdgeMidPoints[EDGE_AB] = (pointA + pointB) / 2.0;
	m_EdgeMidPoints[EDGE_BC] = (pointB + pointC) / 2.0;
	m_EdgeMidPoints[EDGE_CA] = (pointC + pointA) / 2.0;

	m_Plane = Ogre::Plane(pointA, pointB, pointC);

	m_CenterPoint = (m_Points[0] + m_Points[1] + m_Points[2]) / 3.0f;


	for (int i = 0; i < 3; i++)
	{
		m_EdgeDistances[i] = Ogre::Math::Abs(m_CenterPoint.squaredDistance(m_EdgeMidPoints[i]));
	}

	// bounding rectangle
	float minX, minY, maxX, maxY;
	
	minX = std::min(std::min(point2DA.x, point2DB.x), point2DC.x);
	minY = std::min(std::min(point2DA.y, point2DB.y), point2DC.y);
	maxX = std::max(std::max(point2DA.x, point2DB.x), point2DC.x);
	maxY = std::max(std::max(point2DA.y, point2DB.y), point2DC.y);

	m_BoundingRect = Ogre::FloatRect(minX, minY, maxX, maxY);
}

//////////////////////////////////////////////////////////////////////////
void NavNode::AttemptLink(NavNode* node)
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i]) continue;

		for (int j = 0; j < 3; j++)
		{
			if (node->HasEdge(j)) continue;

			Ogre::Vector3 nodePoint1, nodePoint2;
			node->GetEdgePoints(j, nodePoint1, nodePoint2);


			Ogre::Vector3 point1, point2;
			GetEdgePoints(i, point1, point2);

			if ((point1 == nodePoint1 && point2 == nodePoint2) || (point1 == nodePoint2 && point2 == nodePoint1))
			{
				m_Edges[i] = new NavEdge(this, node);
				node->SetEdge(j, m_Edges[i]);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void NavNode::FinalizeLinks()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i]) continue;
		m_Edges[i] = new NavEdge(this);
	}
}

//////////////////////////////////////////////////////////////////////////
void NavNode::GetEdgePoints(int edgeSide, Ogre::Vector3& point1, Ogre::Vector3& point2) const
{
	switch (edgeSide)
	{
	case EDGE_AB:
		point1 = m_Points[0];
		point2 = m_Points[1];
		break;
	case EDGE_BC:
		point1 = m_Points[1];
		point2 = m_Points[2];
		break;
	case EDGE_CA:
		point1 = m_Points[2];
		point2 = m_Points[0];
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NavNode::GetPoint(unsigned short index) const
{
	if (index > 2) index = 2;
	return m_Points[index];
}

//////////////////////////////////////////////////////////////////////////
void NavNode::SetEdge(int edgeSide, NavEdge* edge)
{
	m_Edges[edgeSide] = edge;
}

//////////////////////////////////////////////////////////////////////////
bool NavNode::IsPointWithinNode(const Ogre::Vector2& point) const
{
	if (!MathUtil::PointInRect(point, m_BoundingRect)) return false;

	int insidePolygonCount = 0;

	for (int i=0; i < 3; i++)
	{
		Line2D::PointClassification edgeResult = m_EdgeLines[i].ClassifyPoint(point);

		if (edgeResult != Line2D::POINT_LEFT)
		{
			insidePolygonCount++;
		}
	}

	return (insidePolygonCount == 3);
}

//////////////////////////////////////////////////////////////////////////
bool NavNode::IsPointWithinNode(const Ogre::Vector3& point) const
{
	// don't care about height at this time
	Ogre::Vector2 point2D(point.x, point.z);

	return IsPointWithinNode(point2D);
}

//////////////////////////////////////////////////////////////////////////
void NavNode::MovePointToNode(Ogre::Vector2& point) const
{
	Line2D testPath(Ogre::Vector2(m_CenterPoint.x, m_CenterPoint.z), point);
	Ogre::Vector2 intersectionPoint;
	
	EdgeSide side;
	NavNode* nextNode;

	LineClassification result = ClassifyLine(testPath, &nextNode, side, &intersectionPoint);

	if (result == EXITING_NODE)
	{
		Ogre::Vector2 pathDirection(intersectionPoint.x - m_CenterPoint.x, intersectionPoint.y - m_CenterPoint.z);

		pathDirection *= 0.999f; // almost on edge

		point.x = m_CenterPoint.x + pathDirection.x;
		point.y = m_CenterPoint.z + pathDirection.y;
		
		return;
	}
	else if (result == NO_RELATIONSHIP)
	{
		// shouldn't happen
		point.x = m_CenterPoint.x;
		point.y = m_CenterPoint.z;
		
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
void NavNode::MovePointToNode(Ogre::Vector3& point) const
{
	Ogre::Vector2 point2D(point.x, point.z);
	MovePointToNode(point2D);

	point.x = point2D.x;
	point.z = point2D.y;
}

//////////////////////////////////////////////////////////////////////////
void NavNode::AdjustPointHeight(Ogre::Vector3& point) const
{
	point.y = MathUtil::GetPlaneY(m_Plane, point.x, point.z);
}

//////////////////////////////////////////////////////////////////////////
NavNode::LineClassification NavNode::ClassifyLine(const Line2D& line, NavNode** nextNode, EdgeSide& edgeSide, Ogre::Vector2* intersectionPoint) const
{
	int interiorCount = 0;

	for (int i = 0; i < 3; i++)
	{
		// end point outside
		if (m_EdgeLines[i].ClassifyPoint(line.GetEndPoint()) != Line2D::POINT_RIGHT)
		{
			// start point inside
			if (m_EdgeLines[i].ClassifyPoint(line.GetStartPoint()) != Line2D::POINT_LEFT)
			{
				// do we really intersect the edge?
				Line2D::LineClassification intersectResult = line.Intersects(m_EdgeLines[i], intersectionPoint);

				if (intersectResult == Line2D::SEGMENTS_INTERSECT || intersectResult == Line2D::A_BISECTS_B)
				{
					*nextNode = m_Edges[i]->GetLinkedNode(this);
					edgeSide = (EdgeSide)i;
					return EXITING_NODE;
				}
			}
		}
		else
		{
			// end point inside the node for this edge
			interiorCount++;
		}
	}

	// end point is inside the node for all edges
	if (interiorCount == 3)
	{
		return ENDING_IN_NODE;
	}


	return NO_RELATIONSHIP;
}

//////////////////////////////////////////////////////////////////////////
NavNode::EdgeType NavNode::GetEdgeType(int edgeSide) const
{
	if (m_Edges[edgeSide]->GetLink(1) == NULL) return EDGE_OUTER;
	if (m_Edges[edgeSide]->GetLink(0) == this) return EDGE_INNER_PRIMARY;
	else return EDGE_INNER_SECONDARY;
}

//////////////////////////////////////////////////////////////////////////
bool NavNode::IsEdgeActive(int edgeSide) const
{
	return HasEdge(edgeSide) && m_Edges[edgeSide]->IsActive();
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NavNode::GetPortalPointTo(NavNode* targetNode) const
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i]->GetLinkedNode(this) == targetNode) return m_EdgeMidPoints[i];
	}
	return m_CenterPoint;
}

//////////////////////////////////////////////////////////////////////////
int NavNode::GetEdgeTo(NavNode* targetNode) const
{
	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i]->GetLinkedNode(this) == targetNode) return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
NavEdge* NavNode::GetNearestEdge(const Ogre::Vector3& point) const
{
	NavEdge* nearestEdge = NULL;
	float nearestDist = 0.0f;

	for (int i = 0; i < 3; i++)
	{
		if (m_Edges[i] != NULL)
		{
			Ogre::Vector3 edgeStart, edgeEnd;
			GetEdgePoints(i, edgeStart, edgeEnd);

			float edgeDist = MathUtil::DistanceFromPointToLine(point, edgeStart, edgeEnd);
			if (nearestEdge == NULL || edgeDist < nearestDist)
			{
				nearestEdge = m_Edges[i];
				nearestDist = edgeDist;
			}
		}
	}
	return nearestEdge;
}

//////////////////////////////////////////////////////////////////////////
float NavNode::GetHeuristic(NavNode* goalNode) const
{
	if (goalNode == this) return 0.0f;
	else return Ogre::Math::Abs(m_CenterPoint.squaredDistance(goalNode->GetCenterPoint()));
}

//////////////////////////////////////////////////////////////////////////
void NavNode::GetSuccessors(std::vector<PathConnection>& successors) const
{
	for (int i = 0; i < 3; i++)
	{
		NavNode* linkedNode = m_Edges[i]->GetLinkedNode(this);
		if (!linkedNode || !linkedNode->IsActive()) continue;

		successors.push_back(PathConnection((void*)linkedNode, m_EdgeDistances[i]));
	}
}


} // namespace Wme

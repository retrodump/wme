// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavNode_H__
#define __WmeNavNode_H__


#include "Object.h"
#include "Line2D.h"
#include "IPathGraph.h"


namespace Wme
{
	class NavEdge;

	class WmeDllExport NavNode : public Object
	{
	public:
		friend class NavMeshRenderer;

		NavNode();
		~NavNode();

		void Initialize(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC);

		void AttemptLink(NavNode* node);
		void FinalizeLinks();

		enum EdgeSide
		{
			EDGE_AB = 0,
			EDGE_BC = 1,
			EDGE_CA = 2
		};

		enum EdgeType
		{
			EDGE_OUTER,
			EDGE_INNER_PRIMARY,
			EDGE_INNER_SECONDARY
		};


		bool HasEdge(int edgeSide) const { return m_Edges[edgeSide] != NULL; }
		NavEdge* GetEdge(int edgeSide) const { return m_Edges[edgeSide]; }
		void GetEdgePoints(int edgeSide, Ogre::Vector3& point1, Ogre::Vector3& point2) const;
		Line2D GetEdgeLine(int edgeSide) const { return m_EdgeLines[edgeSide]; }
		void SetEdge(int edgeSide, NavEdge* edge);

		EdgeType GetEdgeType(int edgeSide) const;
		bool IsEdgeActive(int edgeSide) const;

		const Ogre::Vector3& GetCenterPoint() const { return m_CenterPoint; }
		const Ogre::Vector3& GetPoint(unsigned short index) const;
		const Ogre::Plane& GetPlane() const { return m_Plane; }

		bool IsActive() const { return m_Active; }
		void SetActive(bool active) { m_Active = active; }

		bool IsPointWithinNode(const Ogre::Vector2& point) const;
		bool IsPointWithinNode(const Ogre::Vector3& point) const;
		
		void MovePointToNode(Ogre::Vector2& point) const;
		void MovePointToNode(Ogre::Vector3& point) const;

		NavEdge* GetNearestEdge(const Ogre::Vector3& point) const;

		void AdjustPointHeight(Ogre::Vector3& point) const;

		enum LineClassification
		{
			NO_RELATIONSHIP,
			ENDING_IN_NODE,
			EXITING_NODE,
		};
		LineClassification ClassifyLine(const Line2D& line, NavNode** nextNode, EdgeSide& edgeSide, Ogre::Vector2* intersectionPoint) const;

		const Ogre::Vector3& GetEdgeMidPoint(int edge) const { return m_EdgeMidPoints[edge]; }
		const Ogre::Vector3& GetPortalPointTo(NavNode* targetNode) const;
		int GetEdgeTo(NavNode* targetNode) const;

		float GetHeuristic(NavNode* goalNode) const;
		void GetSuccessors(std::vector<PathConnection>& successors) const;

	private:
		bool m_Active;
		Ogre::Vector3 m_Points[3];
		Ogre::Vector3 m_CenterPoint;
		Ogre::Plane m_Plane;
		NavEdge* m_Edges[3];
		Line2D m_EdgeLines[3];
		Ogre::Vector3 m_EdgeMidPoints[3];
		float m_EdgeDistances[3];
		Ogre::FloatRect m_BoundingRect;
	};
}

#endif // __WmeNavNode_H__

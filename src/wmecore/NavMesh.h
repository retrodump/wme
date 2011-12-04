// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavMesh_H__
#define __WmeNavMesh_H__


#include "Object.h"
#include "IPathGraph.h"


namespace Wme
{
	class NavNode;
	class NavEdge;

	class WmeDllExport NavMesh : public Object, public IPathGraph
	{
	public:
		friend class NavMeshRenderer;

		struct PathPoint
		{
			Ogre::Vector3 Point;
			NavNode* Node;
			int EdgeLine;
		};
		typedef std::list<PathPoint> PathPointList;


		class WmeDllExport Listener 
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void OnPathFound(const PathPointList& path) {}
			virtual void OnDestroy() {}
		};


		NavMesh();
		~NavMesh();

		void Clear();
		void AddNode(const Ogre::Vector3& pointA, const Ogre::Vector3& pointB, const Ogre::Vector3& pointC);
		void AutoLinkNodes();

		void LoadFromOgreMesh(Ogre::MeshPtr mesh);

		NavNode* GetNearestNode(const Ogre::Vector3& point) const;

		Ogre::Vector3 MovePointToMesh(const Ogre::Vector3& point, NavNode** node = NULL) const;
		Ogre::Vector3 MovePointToNode(const Ogre::Vector3& point, NavNode* node) const;

		bool StraightLineExists(NavNode* startNode, const Ogre::Vector3& startPoint, NavNode* endNode, const Ogre::Vector3& endPoint) const;

		bool GetIntersection(const Ogre::Ray& ray, NavNode** node = NULL, NavEdge** edge = NULL, Ogre::Vector3* point = NULL);
		
		bool FindPath(const Ogre::Vector3& startPoint, const Ogre::Vector3& endPoint, PathPointList& path) const;
		bool FindPath(NavNode* startNode, const Ogre::Vector3& startPoint, NavNode* endNode, const Ogre::Vector3& endPoint, PathPointList& path) const;

		void AddListener(Listener* listener);
		void RemoveListener(Listener* listener);
		
		// IPathGraph
		float GetHeuristic(void* start, void* end) const;
		void GetSuccessors(void* node, std::vector<PathConnection>& successors) const;

	private:
		typedef std::vector<NavNode*> NodeVector;
		NodeVector m_Nodes;

		void OptimizePathRubberBand(const PathPointList& origPath, PathPointList& newPath) const;
		bool RubberBandPoint(const PathPoint& pointA, PathPoint& pointB, const PathPoint& pointC) const;
		
		typedef PathPointList::const_iterator PathPointIt;
		void OptimizeStraightLines(const PathPointList& origPath, PathPointList& newPath) const;
		PathPointIt GetFarthestStraightPoint(const PathPointList& path, const PathPointIt& initPoint) const;

		void OptimizeSpline(const PathPointList& origPath, PathPointList& newPath, int numSegments) const;

		typedef std::vector<Listener*> ListenerList;
		ListenerList m_Listeners;
	};
}

#endif // __WmeNavMesh_H__

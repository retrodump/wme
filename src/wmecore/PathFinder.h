// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePathFinder_H__
#define __WmePathFinder_H__


#include "Object.h"
#include "IPathGraph.h"


namespace Wme
{
	class PathNode;

	/// generic A* path finder
	class WmeDllExport PathFinder : public Object
	{
	public:
		PathFinder();
		~PathFinder();
		
		void Initialize(const IPathGraph* graph);
		void ClearLists();

		bool FindPath(void* start, void* end, std::list<void*>& pathNodes);

	private:
		const IPathGraph* m_Graph;

		void AddClosed(PathNode* node);
		void RemoveClosed(PathNode* node);

		void AddOpen(PathNode* node);
		void UpdateOpen(PathNode* node);
		void RemoveOpen(PathNode* node);
		PathNode* GetBestOpen();


		typedef std::vector<PathNode*> NodeVector;
		NodeVector m_OpenNodes;

		typedef std::map<void*, PathNode*> NodeMap;
		NodeMap m_NodeMap;

		PathNode* MapNode(void* node);
	};
}

#endif // __WmePathFinder_H__

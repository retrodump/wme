// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeIPathGraph_H__
#define __WmeIPathGraph_H__


namespace Wme
{
	struct PathConnection
	{
		PathConnection()
		{
			ToNode = NULL;
			Cost = 0.0f;
		}

		PathConnection(void* toNode, float cost)
		{
			ToNode = toNode;
			Cost = cost;
		}

		void* ToNode;
		float Cost;
	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport IPathGraph
	{
	public:
		virtual ~IPathGraph() {}

		virtual float GetHeuristic(void* startNode, void* endNode) const = 0;
		virtual void GetSuccessors(void* node, std::vector<PathConnection>& successors) const = 0;
	};
}

#endif // __WmeIPathGraph_H__
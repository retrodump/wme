// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PathFinder.h"
#include "PathNode.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
PathFinder::PathFinder()
{
	m_Graph = NULL;
}

//////////////////////////////////////////////////////////////////////////
PathFinder::~PathFinder()
{
	ClearLists();
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::ClearLists()
{
	m_OpenNodes.clear();

	NodeMap::iterator it;
	for (it = m_NodeMap.begin(); it != m_NodeMap.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_NodeMap.clear();
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::Initialize(const IPathGraph* graph)
{
	ClearLists();
	m_Graph = graph;
}

//////////////////////////////////////////////////////////////////////////
bool PathFinder::FindPath(void* start, void* end, std::list<void*>& pathNodes)
{
	if (start == end)
	{
		pathNodes.clear();
		pathNodes.push_back(start);
		pathNodes.push_back(end);
		return true;
	}

	PathNode* startNode = MapNode(start);
	startNode->SetArrivalCost(0);
	startNode->SetGoalEstimate(startNode->CalculateGoalEstimate(m_Graph, end));
	AddOpen(startNode);

	PathNode* bestAccessible = NULL;
	float bestCost = -1.0;

	PathNode* current;

	while (!m_OpenNodes.empty())
	{
		current = GetBestOpen();
		if (current->GetNode() == end) break;


		std::vector<PathConnection> connections;
		m_Graph->GetSuccessors(current->GetNode(), connections);

		foreach (PathConnection connection, connections)
		{
			float endNodeEstimate;

			PathNode* endNode = MapNode(connection.ToNode);
			float endNodeCost = current->GetArrivalCost() + connection.Cost;


			if (endNode->IsClosed())
			{
				if (endNode->GetArrivalCost() < endNodeCost) continue;
				if (endNodeCost - endNode->GetArrivalCost() <= 0.00001f) continue;

				RemoveClosed(endNode);
				endNodeEstimate = endNode->GetTotalCost() - endNode->GetArrivalCost();
			}
			else if (endNode->IsOpen())
			{
				endNodeEstimate = endNode->GetTotalCost() - endNode->GetArrivalCost();
			}
			else
			{
				endNodeEstimate = endNode->CalculateGoalEstimate(m_Graph, end);
			}

			endNode->SetArrivalCost(endNodeCost);
			endNode->SetFromNode(current);
			endNode->SetGoalEstimate(endNodeCost + endNodeEstimate);


			if (!endNode->IsOpen()) AddOpen(endNode);
			else UpdateOpen(endNode);

			if (bestCost < 0.0f || endNode->GetGoalEstimate() < bestCost)
			{
				bestAccessible = endNode;
				bestCost = endNode->GetGoalEstimate();
			}

		}
		
		AddClosed(current);
	}

	bool exactPath = (current->GetNode() == end);


	// path doesn't exist - restart with nearest node as target
	if (!exactPath && bestAccessible)
	{
		void* newEnd = bestAccessible->GetNode();
		ClearLists();
		FindPath(start, newEnd, pathNodes);
		return false;
	}

	// build path
	pathNodes.clear();
	while (current->GetNode() != start)
	{
		pathNodes.push_front(current->GetNode());

		current = current->GetFromNode();

		// break cycled nodes (how can that happen???)
		//if (current->GetFromNode() && current == current->GetFromNode()->GetFromNode()) break;
	}
	pathNodes.push_front(start);

	ClearLists();

	return exactPath;
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::AddClosed(PathNode* node)
{
	node->SetClosed(true);
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::RemoveClosed(PathNode* node)
{
	node->SetClosed(false);
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::AddOpen(PathNode* node)
{
	node->SetOpen(true);

	m_OpenNodes.push_back(node);
	std::push_heap(m_OpenNodes.begin(), m_OpenNodes.end(), NodeTotalCostGreater());
}

//////////////////////////////////////////////////////////////////////////
void PathFinder::UpdateOpen(PathNode* node)
{
	// changing node's total cost while it's still in the heap causes 'strict weak ordering' assert to kick in
	// just rebuild the heap for now
	std::make_heap(m_OpenNodes.begin(), m_OpenNodes.end(), NodeTotalCostGreater());


	/*
	for (NodeVector::iterator it = m_OpenNodes.begin(); it != m_OpenNodes.end(); it++)
	{
		if ((*it)->GetNode() == node->GetNode())
		{
			std::make_heap(m_OpenNodes.begin(), it + 1, NodeTotalCostGreater());
			return;
		}
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
PathNode* PathFinder::GetBestOpen()
{
	PathNode* node = m_OpenNodes.front();
	std::pop_heap(m_OpenNodes.begin(), m_OpenNodes.end(), NodeTotalCostGreater());
	m_OpenNodes.pop_back();

	node->SetOpen(false);

	return node;
}

//////////////////////////////////////////////////////////////////////////
PathNode* PathFinder::MapNode(void* node)
{
	NodeMap::iterator it = m_NodeMap.find(node);
	if (it != m_NodeMap.end()) return it->second;
	else
	{
		PathNode* newNode = new PathNode;
		newNode->SetNode(node);

		m_NodeMap[node] = newNode;
		return newNode;
	}
}


} // namespace Wme

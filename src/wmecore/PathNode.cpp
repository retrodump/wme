// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "PathNode.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
PathNode::PathNode()
{
	m_GraphNode = NULL;
	m_FromNode = NULL;
	m_ArrivalCost = m_GoalEstimate = 0.0f;

	m_IsClosed = m_IsOpen = false;
}

//////////////////////////////////////////////////////////////////////////
PathNode::~PathNode()
{
}

//////////////////////////////////////////////////////////////////////////
bool PathNode::operator <(const PathNode& right) const
{
	return GetTotalCost() > right.GetTotalCost();
}

//////////////////////////////////////////////////////////////////////////
float PathNode::CalculateGoalEstimate(const IPathGraph* graph, void* goal)
{
	return graph->GetHeuristic(m_GraphNode, goal);
}


} // namespace Wme

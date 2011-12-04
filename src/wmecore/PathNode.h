// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePathNode_H__
#define __WmePathNode_H__


#include "Object.h"
#include "IPathGraph.h"


namespace Wme
{
	class WmeDllExport PathNode : public Object
	{
	public:
		PathNode();
		~PathNode();

		bool operator <(const PathNode& right) const;

		float GetArrivalCost() const { return m_ArrivalCost; }
		void SetArrivalCost(float cost) { m_ArrivalCost = cost; }

		float GetGoalEstimate() const { return m_GoalEstimate; }
		void SetGoalEstimate(float cost) { m_GoalEstimate = cost; }

		float GetTotalCost() const { return m_ArrivalCost + m_GoalEstimate; }

		void* GetNode() const { return m_GraphNode; }
		void SetNode(void* node) { m_GraphNode = node; }

		PathNode* GetFromNode() const { return m_FromNode; }
		void SetFromNode(PathNode* node) { m_FromNode = node; }

		bool IsOpen() const { return m_IsOpen; }
		void SetOpen(bool open) { m_IsOpen = open; }

		bool IsClosed() const { return m_IsClosed; }
		void SetClosed(bool closed) { m_IsClosed = closed; }

		float CalculateGoalEstimate(const IPathGraph* graph, void* goal);

	private:
		void* m_GraphNode;
		float m_ArrivalCost;
		float m_GoalEstimate;
		PathNode* m_FromNode;

		bool m_IsClosed;
		bool m_IsOpen;
	};

	//////////////////////////////////////////////////////////////////////////
	class NodeTotalCostGreater
	{
	public:
		bool operator ()(PathNode* first, PathNode* second) const
		{
			return first->GetTotalCost() > second->GetTotalCost();
		}
	};
}

#endif // __WmePathNode_H__

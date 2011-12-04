// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavEdge_H__
#define __WmeNavEdge_H__


#include "Object.h"
#include "Line2D.h"


namespace Wme
{
	class NavNode;

	class WmeDllExport NavEdge : public Object
	{
	public:
		NavEdge(NavNode* node1, NavNode* node2 = NULL);
		~NavEdge();

		void RemoveNode(const NavNode* node);

		bool IsActive() { return m_Active; }
		void SetActive(bool active) { m_Active = active; }

		NavNode* GetLinkedNode(const NavNode* node);
		NavNode* GetLink(int index) { return m_Nodes[index]; }

	private:
		NavNode* m_Nodes[2];
		bool m_Active;
	};
}

#endif // __WmeNavEdge_H__
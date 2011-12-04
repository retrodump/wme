// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NavEdge.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavEdge::NavEdge(NavNode* node1, NavNode* node2)
{
	m_Active = true;

	m_Nodes[0] = node1;
	m_Nodes[1] = node2;
}

//////////////////////////////////////////////////////////////////////////
NavEdge::~NavEdge()
{
}

//////////////////////////////////////////////////////////////////////////
void NavEdge::RemoveNode(const NavNode* node)
{
	bool nodesExist = false;

	for (int i = 0; i < 2; i++)
	{
		if (m_Nodes[i] == node) m_Nodes[i] = NULL;
	}

	if (m_Nodes[0] == NULL && m_Nodes[1] == NULL) delete this;
}

//////////////////////////////////////////////////////////////////////////
NavNode* NavEdge::GetLinkedNode(const NavNode* node)
{
	if (!m_Active) return NULL;

	if (node == m_Nodes[0]) return m_Nodes[1];
	else if (node == m_Nodes[1]) return m_Nodes[0];
	else return NULL;
}


} // namespace Wme

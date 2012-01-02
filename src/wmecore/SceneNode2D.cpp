// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SceneNode2D::SceneNode2D()
{
	m_SortNeeded = false;

	m_ParentNode = NULL;
	m_ZOrder = 0;
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D::~SceneNode2D()
{
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AddChild(SceneNode2D* child, int zOrder)
{
	if (!child || child == this) return;
	child->SetParentNode(this);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::RemoveChild(SceneNode2D* child)
{
	if (!child || child->GetParentNode() != this) return;
	child->SetParentNode(NULL);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetParentNode(SceneNode2D* newParent)
{
	if (m_ParentNode == newParent) return;

	if (m_ParentNode)
	{
		m_ParentNode->m_Children.remove(this);
	}

	if (newParent)
	{
		newParent->m_Children.push_back(this);
		newParent->m_SortNeeded = true;
	}
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D* SceneNode2D::GetChild(size_t index) const
{
	SortChildren();

	if (index >= m_Children.size()) return NULL;

	size_t i = 0;
	foreach (SceneNode2D* child, m_Children)
	{
		if (i == index) return child;
		i++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetZOrder(int zOrder)
{
	if (zOrder != m_ZOrder)
	{
		m_ZOrder = zOrder;
		m_SortNeeded = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SortChildren() const
{
	if (!m_SortNeeded) return;

	m_Children.sort(SceneNode2D::ZOrderComparer);
	
	m_SortNeeded = false;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNode2D::ZOrderComparer(SceneNode2D* node1, SceneNode2D* node2)
{
	return node1->m_ZOrder < node2->m_ZOrder;
}


} // namespace Wme

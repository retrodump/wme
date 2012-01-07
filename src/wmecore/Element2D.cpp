// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Element2D.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Element2D::Element2D()
{
	m_ParentNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
Element2D::~Element2D()
{
	DetachFromParent();
}

//////////////////////////////////////////////////////////////////////////
void Element2D::OnAttached(SceneNode2D* sceneNode)
{
	m_ParentNode = sceneNode;
}

//////////////////////////////////////////////////////////////////////////
void Element2D::OnDetached(SceneNode2D* sceneNode)
{
	m_ParentNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
bool Element2D::IsAttached() const
{
	return m_ParentNode != NULL;
}

//////////////////////////////////////////////////////////////////////////
void Element2D::DetachFromParent()
{
	if (m_ParentNode)
	{
		m_ParentNode->DetachElement();
	}
}


} // namespace Wme

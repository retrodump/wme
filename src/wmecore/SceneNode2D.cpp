// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SceneNode2D.h"
#include "Element2D.h"
#include "MathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SceneNode2D::SceneNode2D()
{
	m_SortNeeded = false;

	m_ParentNode = NULL;
	
	m_ZOrder = 0;
	m_Position = m_DerivedPosition = Ogre::Vector2::ZERO;
	m_Rotation = m_DerivedRotation = 0;
	m_Scale = m_DerivedScale = Ogre::Vector2::UNIT_SCALE;

	m_TransformDirty = true;
	m_GeometryDirty = true;

	m_AttachedElement = NULL;
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D::~SceneNode2D()
{
	DetachElement();
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
		m_ParentNode = newParent;
		SetTransformDirty();
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
void SceneNode2D::SetPosition(const Ogre::Vector2& pos)
{
	if (m_Position != pos)
	{
		m_Position = pos;
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetPosition(float x, float y)
{
	SetPosition(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetRotation(const Ogre::Degree& angle)
{
	if (m_Rotation != angle)
	{
		m_Rotation = MathUtil::NormalizeAngle(angle);
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetRotation(float degrees)
{
	SetRotation(Ogre::Degree(degrees));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetScale(const Ogre::Vector2& scale)
{
	if (m_Scale != scale)
	{
		m_Scale = scale;
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetScale(float x, float y)
{
	SetScale(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedPosition(const Ogre::Vector2& pos)
{
	if (!m_ParentNode) SetPosition(pos);
	else SetPosition(m_ParentNode->PositionSceneToLocal(pos));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedPosition(float x, float y)
{
	SetDerivedPosition(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedRotation(const Ogre::Degree& angle)
{
	if (!m_ParentNode) SetRotation(angle);
	else SetRotation(m_ParentNode->RotationSceneToLocal(angle));	
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedRotation(float degrees)
{
	SetDerivedRotation(Ogre::Degree(degrees));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedScale(const Ogre::Vector2& scale)
{
	if (!m_ParentNode) SetScale(scale);
	else SetScale(m_ParentNode->ScaleSceneToLocal(scale));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedScale(float x, float y)
{
	SetDerivedScale(Ogre::Vector2(x, y));
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

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetTransformDirty()
{
	m_TransformDirty = true;
	m_GeometryDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetGeometryDirty()
{
	m_GeometryDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AttachElement(Element2D* element)
{
	if (element == m_AttachedElement) return;

	if (m_AttachedElement) DetachElement();
	m_AttachedElement = element;
	if (m_AttachedElement) m_AttachedElement->OnAttached(this);

	SetGeometryDirty();
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::DetachElement()
{
	if (m_AttachedElement)
	{
		m_AttachedElement->OnDetached(this);
		m_AttachedElement = NULL;

		SetGeometryDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::UpdateTransform(bool forceUpdate, bool updateChildren)
{
	if (!forceUpdate && !m_TransformDirty) return;

	UpdateTransfromInternal();

	m_TransformDirty = false;

	if (updateChildren)
	{
		foreach (SceneNode2D* child, m_Children)
		{
			child->UpdateTransform(true, true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::UpdateTransfromInternal()
{
	if (m_ParentNode)
	{
		m_DerivedPosition = m_ParentNode->GetSceneTransform() * m_Position;
		m_DerivedRotation = MathUtil::NormalizeAngle(m_ParentNode->GetRotation() + m_Rotation);
		m_DerivedScale = m_ParentNode->GetScale() * m_Scale;
	}
	else
	{
		// root node (no parent)
		m_DerivedPosition = m_Position;
		m_DerivedRotation = m_Rotation;
		m_DerivedScale = m_Scale;
	}

	m_SceneTransform.Reset();
	
	m_SceneTransform.Scale(m_DerivedScale.x, m_DerivedScale.y);	
	m_SceneTransform.Rotate(m_DerivedRotation);
	m_SceneTransform.Translate(m_Position.x, m_Position.y);	
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 SceneNode2D::PositionSceneToLocal(const Ogre::Vector2& pos) const
{
	return m_SceneTransform.Inverted() * pos;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Degree SceneNode2D::RotationSceneToLocal(const Ogre::Degree& angle) const
{
	return MathUtil::NormalizeAngle(angle - m_Rotation);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 SceneNode2D::ScaleSceneToLocal(const Ogre::Vector2& scale) const
{
	return scale / m_Scale;
}


} // namespace Wme

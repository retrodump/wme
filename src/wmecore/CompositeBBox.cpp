// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CompositeBBox.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
CompositeBBox::CompositeBBox()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
CompositeBBox::~CompositeBBox()
{
}

//////////////////////////////////////////////////////////////////////////
void CompositeBBox::Clear()
{
	m_Minimum = m_Maximum = Ogre::Vector3::ZERO;
	m_WorldTransform = Ogre::Matrix4::IDENTITY;
	m_InvWorldTransform = Ogre::Matrix4::IDENTITY;
	m_Empty = true;
}

//////////////////////////////////////////////////////////////////////////
void CompositeBBox::AddEntity(const Ogre::Entity* entity)
{
	Ogre::SceneNode* node = entity->getParentSceneNode();
	if (!node) return;

	if (m_Empty)
	{
		m_WorldTransform = node->_getFullTransform();
		m_InvWorldTransform = m_WorldTransform.inverse();
		m_Minimum = m_Maximum = Ogre::Vector3::ZERO;
	}

	Ogre::AxisAlignedBox bbox = entity->getBoundingBox();
	const Ogre::Vector3* corners = bbox.getAllCorners();

	for (int i = 0; i < 8; i++)
	{
		Ogre::Vector3 corner = corners[i];

		// from local to world position
		corner = node->_getFullTransform() * corner;

		// from world to main node's local space
		corner = m_InvWorldTransform * corner;

		AddPoint(corner);
	}
}

//////////////////////////////////////////////////////////////////////////
void CompositeBBox::AddNode(const Ogre::SceneNode* node)
{
	if (m_Empty)
	{
		m_WorldTransform = node->_getFullTransform();
		m_InvWorldTransform = m_WorldTransform.inverse();
		m_Minimum = m_Maximum = Ogre::Vector3::ZERO;
	}
	AddPoint(m_InvWorldTransform * node->_getDerivedPosition());

}

//////////////////////////////////////////////////////////////////////////
void CompositeBBox::AddPoint(const Ogre::Vector3& point)
{
	if (m_Empty)
	{
		m_Minimum = point;
		m_Maximum = point;		
	}
	else
	{
		m_Minimum.x = std::min(m_Minimum.x, point.x);
		m_Minimum.y = std::min(m_Minimum.y, point.y);
		m_Minimum.z = std::min(m_Minimum.z, point.z);

		m_Maximum.x = std::max(m_Maximum.x, point.x);
		m_Maximum.y = std::max(m_Maximum.y, point.y);
		m_Maximum.z = std::max(m_Maximum.z, point.z);
	}
	m_Empty = false;
}

//////////////////////////////////////////////////////////////////////////
void CompositeBBox::LocalToWorld(Ogre::Vector3& point) const
{
	point = m_WorldTransform * point;
}


} // namespace Wme
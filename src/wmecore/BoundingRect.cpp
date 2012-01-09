// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "BoundingRect.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
BoundingRect::BoundingRect()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
BoundingRect::~BoundingRect()
{
}

//////////////////////////////////////////////////////////////////////////
void BoundingRect::Clear()
{
	m_Empty = true;
	m_Minimum = m_Maximum = Ogre::Vector2::ZERO;
}

//////////////////////////////////////////////////////////////////////////
void BoundingRect::AddPoint(float x, float y)
{
	AddPoint(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void BoundingRect::AddPoint(const Ogre::Vector2& point)
{
	if (m_Empty)
	{
		m_Minimum = m_Maximum = point;		
	}
	else
	{
		m_Minimum.x = std::min(point.x, m_Minimum.x);
		m_Minimum.y = std::min(point.y, m_Minimum.y);

		m_Maximum.x = std::max(point.x, m_Maximum.x);
		m_Maximum.y = std::max(point.y, m_Maximum.y);
	}
	m_Empty = false;
}

//////////////////////////////////////////////////////////////////////////
bool BoundingRect::ContainsPoint(float x, float y) const
{
	return ContainsPoint(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
bool BoundingRect::ContainsPoint(const Ogre::Vector2& point) const
{
	if (m_Empty) return false;
	else return point.x >= m_Minimum.x && point.x <= m_Maximum.x && point.y >= m_Minimum.y && point.y <= m_Maximum.y;
}


} // namespace Wme

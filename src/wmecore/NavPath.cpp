// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NavPath.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NavPath::NavPath()
{
	m_Ready = false;
}

//////////////////////////////////////////////////////////////////////////
NavPath::~NavPath()
{
}

//////////////////////////////////////////////////////////////////////////
void NavPath::Reset()
{
	m_Ready = false;
	m_Points.clear();
}

//////////////////////////////////////////////////////////////////////////
int NavPath::GetNumPoints() const
{
	return m_Points.size();
}

//////////////////////////////////////////////////////////////////////////
void NavPath::AddPoint(const Ogre::Vector3& point)
{
	m_Points.push_back(point);
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NavPath::GetPoint(int index) const
{
	return m_Points[index];
}

//////////////////////////////////////////////////////////////////////////
void NavPath::Reserve(size_t count)
{
	m_Points.reserve(count);
}


} // namespace Wme

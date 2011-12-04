// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Timer.h"

namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
Timer::Timer()
{
	m_OgreTimer = new Ogre::Timer();	
	Reset();
}

//////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{
	SAFE_DELETE(m_OgreTimer);
}

//////////////////////////////////////////////////////////////////////////
void Timer::Reset()
{
	m_CurrentTime = m_PrevTime = m_ElapsedTime = 0;
	m_IsFrozen = false;
	m_Multiplier = 1.0;
}

//////////////////////////////////////////////////////////////////////////
unsigned long Timer::Update()
{
	if (m_IsFrozen)
	{
		m_ElapsedTime = 0;
		return m_CurrentTime;
	}

	unsigned long currentTime = m_OgreTimer->getMilliseconds();

	unsigned long delta = currentTime - m_PrevTime;
	if (delta > 1000) delta = 1000;
	delta = (unsigned long)((float)delta * m_Multiplier);

	m_CurrentTime += delta;
	m_PrevTime = currentTime;

	m_ElapsedTime = delta;

	return m_CurrentTime;
}

//////////////////////////////////////////////////////////////////////////
void Timer::Freeze()
{
	m_IsFrozen = true;
}

//////////////////////////////////////////////////////////////////////////
void Timer::Unfreeze()
{
	m_IsFrozen = false;
}


} // namespace Wme

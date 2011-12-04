// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MicroTask.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MicroTask::MicroTask()
{
	m_IsActive = false;
	m_IsFinished = false;
}

//////////////////////////////////////////////////////////////////////////
MicroTask::~MicroTask()
{
}

//////////////////////////////////////////////////////////////////////////
void MicroTask::SetActive()
{
	if (!m_IsActive)
	{
		m_IsActive = true;
		OnActivate();
	}
}


//////////////////////////////////////////////////////////////////////////
void MicroTask::SetFinished()
{
	if (m_IsActive)
	{
		m_IsActive = false;
		OnDeactivate();
	}
	m_IsFinished = true;
}


} // namespace Wme

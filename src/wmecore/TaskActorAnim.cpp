// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TaskActorAnim.h"
#include "Actor3D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TaskActorAnim::TaskActorAnim(Actor3D* actor)
{
	m_Actor = actor;
}

//////////////////////////////////////////////////////////////////////////
TaskActorAnim::~TaskActorAnim()
{
}


//////////////////////////////////////////////////////////////////////////
void TaskActorAnim::OnActivate()
{
	if (m_AnimProp.Name.empty())
	{
		SetFinished();
		return;
	}

	m_Actor->StartAnimation(m_AnimProp);
}

//////////////////////////////////////////////////////////////////////////
void TaskActorAnim::OnDeactivate()
{
	if (m_Actor->IsAnimationPlaying(m_AnimProp))
		m_Actor->StopAnimation(m_AnimProp);
}

//////////////////////////////////////////////////////////////////////////
void TaskActorAnim::OnUpdate()
{
	if (!m_Actor->IsAnimationPlaying(m_AnimProp)) SetFinished();
}


} // namespace Wme

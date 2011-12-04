// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TaskActorTurn.h"
#include "Actor3D.h"
#include "Timer.h"
#include "StringUtil.h"
#include "MathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TaskActorTurn::TaskActorTurn(Actor3D* actor)
{
	m_Actor = actor;
	m_TargetPoint = Ogre::Vector3::ZERO;

	m_TargetAngle = m_CurrentAngle = Ogre::Degree(0);
	m_TurnClockwise = false;
}

//////////////////////////////////////////////////////////////////////////
TaskActorTurn::~TaskActorTurn()
{
}

//////////////////////////////////////////////////////////////////////////
WideString TaskActorTurn::GetAnimName() const
{
	WideString animName;
	if (m_TurnClockwise) animName = m_Actor->m_TurnCWAnimName;
	else animName = m_Actor->m_TurnCCWAnimName;

	if (animName.empty()) animName = m_Actor->m_IdleAnimName;

	return animName;
}

//////////////////////////////////////////////////////////////////////////
void TaskActorTurn::OnActivate()
{
	Ogre::Degree targetAngle = m_Actor->GetRotationTo(m_TargetPoint).getYaw();
	m_Actor->PrepareTurn(targetAngle, m_TargetAngle, m_TurnClockwise);

	m_CurrentAngle = MathUtil::NormalizeAngle(m_Actor->GetOrientation().getYaw());

	if (m_CurrentAngle == m_TargetAngle) return;


	AnimProp turnAnim;
	turnAnim.Layer = m_Actor->m_MovementAnimChannel;
	turnAnim.Name = GetAnimName();
	
	if (!turnAnim.Name.empty())
		m_Actor->StartAnimation(turnAnim);
}

//////////////////////////////////////////////////////////////////////////
void TaskActorTurn::OnDeactivate()
{
}

//////////////////////////////////////////////////////////////////////////
void TaskActorTurn::OnUpdate()
{
	unsigned long elapsedTime = m_Actor->GetTier()->GetTimer()->GetElapsedTime();

	if (m_TurnClockwise)
	{
		m_CurrentAngle += Ogre::Degree(m_Actor->m_TurnSpeedStand * elapsedTime);
		if (m_CurrentAngle > m_TargetAngle) m_CurrentAngle = m_TargetAngle;
	}
	else
	{
		m_CurrentAngle -= Ogre::Degree(m_Actor->m_TurnSpeedStand * elapsedTime);
		if (m_CurrentAngle < m_TargetAngle) m_CurrentAngle = m_TargetAngle;
	}

	Ogre::Quaternion q;
	q.FromAngleAxis(Ogre::Radian(m_CurrentAngle), Ogre::Vector3::UNIT_Y);

	m_Actor->SetOrientation(q);

	if (m_CurrentAngle == m_TargetAngle)
	{
		SetFinished();
	}
}


} // namespace Wme

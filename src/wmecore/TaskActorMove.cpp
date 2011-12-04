// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "TaskActorMove.h"
#include "Actor3D.h"
#include "Scene3DBase.h"
#include "NavPath.h"
#include "Timer.h"
#include "MathUtil.h"
#include "TaskActorTurn.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
TaskActorMove::TaskActorMove(Actor3D* actor)
{
	m_Actor = actor;
	m_TargetPoint = Ogre::Vector3::ZERO;

	m_IsFollowingPath = false;
	m_CurrentSubGoal = 0;
	
	m_TurnSubTask = NULL;
}

//////////////////////////////////////////////////////////////////////////
TaskActorMove::~TaskActorMove()
{
	SAFE_DELETE(m_TurnSubTask);
}

//////////////////////////////////////////////////////////////////////////
void TaskActorMove::OnActivate()
{
	m_Actor->m_Stage->GetPath(m_Actor->GetPosition(), m_TargetPoint, m_Actor->m_Path);
}

//////////////////////////////////////////////////////////////////////////
void TaskActorMove::OnDeactivate()
{
	if (m_TurnSubTask)
	{
		m_TurnSubTask->SetFinished();
		SAFE_DELETE(m_TurnSubTask);
	}
}

//////////////////////////////////////////////////////////////////////////
void TaskActorMove::OnUpdate()
{
	if (!m_Actor->m_Path->IsReady())
	{
		AnimProp idleAnim;
		idleAnim.Layer = m_Actor->m_MovementAnimChannel;
		idleAnim.Name = m_Actor->m_IdleAnimName;
		idleAnim.AnimPrevWhileFadingIn = false;
		m_Actor->StartAnimation(idleAnim);
		return;
	}

	if (m_CurrentSubGoal >= m_Actor->m_Path->GetNumPoints())
	{
		SetFinished();
		return;
	}


	// we're starting; check initial turning angle
	if (!m_IsFollowingPath && !m_TurnSubTask && m_Actor->m_Path->GetNumPoints() > 1)
	{
		Ogre::Vector3 secondPoint = m_Actor->m_Path->GetPoint(1);

		bool clockwise;
		Ogre::Degree targetAngle = m_Actor->GetRotationTo(secondPoint).getYaw();
		m_Actor->PrepareTurn(targetAngle, targetAngle, clockwise);

		Ogre::Degree currentAngle = MathUtil::NormalizeAngle(m_Actor->GetOrientation().getYaw());

		// add a turning subtask is needed
		if (Ogre::Math::Abs(currentAngle.valueDegrees() - targetAngle.valueDegrees()) >= m_Actor->m_MinTurningAngle)
		{
			m_TurnSubTask = new TaskActorTurn(m_Actor);
			m_TurnSubTask->SetTargetPoint(secondPoint);
			m_TurnSubTask->OnActivate();
		}
	}

	// process turning first
	if (m_TurnSubTask && !m_TurnSubTask->IsFinished())
	{
		m_TurnSubTask->OnUpdate();
		if (!m_TurnSubTask->IsFinished()) return;
	}



	unsigned long elapsedTime = m_Actor->GetTier()->GetTimer()->GetElapsedTime();
	float distance = elapsedTime * m_Actor->m_MoveSpeed;


	Ogre::Vector3 origPos = m_Actor->GetPosition();
	Ogre::Vector3 pos = origPos;

	// process path fragments until the required distance is reached
	do
	{
		Ogre::Vector3 subGoal = m_Actor->m_Path->GetPoint(m_CurrentSubGoal);

		float subGoalDist = Ogre::Math::Abs(pos.distance(subGoal));

		if (subGoalDist < distance)
		{
			pos = subGoal;
			distance -= subGoalDist;

			m_CurrentSubGoal++;
			if (m_CurrentSubGoal >= m_Actor->m_Path->GetNumPoints()) break;
		}
		else
		{
			Ogre::Vector3 dir = subGoal - pos;
			pos += dir * (distance / subGoalDist);
			distance = 0;
		}

	} while (distance > 0);

	AnimProp walkAnim;
	walkAnim.Layer = m_Actor->m_MovementAnimChannel;
	walkAnim.Name = m_Actor->m_WalkAnimName;
	walkAnim.AnimWhileFadingIn = false;

	// only start moving once the animation is faded in
	if (!m_Actor->HasAnimation(m_Actor->m_WalkAnimName) || m_Actor->IsAnimationPlaying(walkAnim, false))
		m_Actor->SetPosition(pos);


	if (m_CurrentSubGoal < m_Actor->m_Path->GetNumPoints())
	{
		// start walking animation
		if (!m_IsFollowingPath)
		{
			m_IsFollowingPath = true;
			m_Actor->StartAnimation(walkAnim);
		}

		// adjust orientation
		Ogre::Vector3 subGoal = m_Actor->m_Path->GetPoint(m_CurrentSubGoal);
		Ogre::Quaternion dir = m_Actor->GetOrientation();


		Ogre::Quaternion destDir = m_Actor->GetRotationTo(subGoal);
		Ogre::Quaternion q = Ogre::Quaternion::Slerp(std::min(1.0f, (m_Actor->m_TurnSpeedMove * elapsedTime / 100.0f)), dir, destDir, true);
		m_Actor->SetOrientation(q);
	}
	else
	{
		SetFinished();
	}
}


} // namespace Wme

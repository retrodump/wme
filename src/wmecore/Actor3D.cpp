// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Actor3D.h"
#include "Stage.h"
#include "NavPath.h"
#include "SimpleAnimTree.h"

#include "TaskActorTurn.h"
#include "TaskActorMove.h"
#include "TaskActorAnim.h"
#include "MathUtil.h"

// temp
#include "Scene3DBase.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Actor3D::Actor3D()
{
	m_Path = new NavPath;

	m_MoveSpeed = 0.2f;
	m_TurnSpeedMove = 0.8f;
	m_TurnSpeedStand = 0.3f;
	
	m_MinTurningAngle = 90.0f;

	m_IdleAnimName = L"idle";
	m_WalkAnimName = L"walk";
	m_TurnCWAnimName = L"";
	m_TurnCCWAnimName = L"";
	m_MovementAnimChannel = 0;
}

//////////////////////////////////////////////////////////////////////////
Actor3D::~Actor3D()
{
	SAFE_DELETE(m_Path);
}

//////////////////////////////////////////////////////////////////////////
void Actor3D::Update()
{
	if (!IsInStage()) return;

	UpdateTasks();

	// no current task, go idle
	if (!GetCurrentTask())
	{
		AnimProp idleAnim;
		idleAnim.Layer = m_MovementAnimChannel;
		idleAnim.Name = m_IdleAnimName;
		idleAnim.AnimPrevWhileFadingIn = false;

		if (!IsAnyAnimationPlaying()) idleAnim.FadeInTime = 0;

		StartAnimation(idleAnim);
	}

	if (m_AnimTree) m_AnimTree->Update();
}

//////////////////////////////////////////////////////////////////////////
void Actor3D::MoveTo(const Ogre::Vector3& targetPoint, bool enqueue)
{
	TaskActorMove* task = new TaskActorMove(this);
	task->SetTargetPoint(targetPoint);
	
	if (enqueue) EnqueueTask(task);
	else SetTask(task);
}

//////////////////////////////////////////////////////////////////////////
void Actor3D::TurnTo(const Ogre::Vector3& targetPoint, bool enqueue)
{
	TaskActorTurn* task = new TaskActorTurn(this);
	task->SetTargetPoint(targetPoint);
	
	if (enqueue) EnqueueTask(task);
	else SetTask(task);
}

//////////////////////////////////////////////////////////////////////////
void Actor3D::PlayFullBodyAnim(AnimProp& animProp, bool enqueue)
{
	TaskActorAnim* task = new TaskActorAnim(this);
	task->SetAnimProp(animProp);

	if (enqueue) EnqueueTask(task);
	else SetTask(task);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Quaternion Actor3D::GetRotationTo(const Ogre::Vector3& point)
{
	Ogre::Vector3 desiredDir = point - GetPosition();
	desiredDir.y = 0;
	desiredDir.normalise();
	return Ogre::Vector3::UNIT_Z.getRotationTo(desiredDir);
}

//////////////////////////////////////////////////////////////////////////
void Actor3D::PrepareTurn(Ogre::Degree targetAngle, Ogre::Degree& newTargetAngle, bool& clockwise)
{
	Ogre::Degree currentAngle = MathUtil::NormalizeAngle(GetOrientation().getYaw());
	newTargetAngle = MathUtil::NormalizeAngle(targetAngle);
	
	// already there
	if (currentAngle == newTargetAngle)
	{
		clockwise = true;
		return;
	}


	float delta1, delta2, delta3, delta;

	delta1 = newTargetAngle.valueDegrees() - currentAngle.valueDegrees();
	delta2 = newTargetAngle.valueDegrees() + 360.0f - currentAngle.valueDegrees();
	delta3 = newTargetAngle.valueDegrees() - 360.0f - currentAngle.valueDegrees();

	delta1 = (fabs(delta1) <= fabs(delta2)) ? delta1 : delta2;
	delta = (fabs(delta1) <= fabs(delta3)) ? delta1 : delta3;

	newTargetAngle = currentAngle + Ogre::Degree(delta);
	clockwise = (delta > 0);
}


//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool Actor3D::LoadFromXml(TiXmlElement* rootNode)
{
	return MeshEntity::LoadFromXml(rootNode);
}

//////////////////////////////////////////////////////////////////////////
bool Actor3D::SaveToXml(TiXmlElement* rootNode)
{
	return MeshEntity::SaveToXml(rootNode);
}


//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void Actor3D::RegisterScriptInterface()
{
	MeshEntity::RegisterScriptInterface();

	RegisterScriptMethod(L"Test",	(MethodHandler)&Actor3D::ScTest);
}

//////////////////////////////////////////////////////////////////////////
bool Actor3D::ScTest(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);

	AnimProp animProp;
	animProp.Name = L"wave";
	PlayFullBodyAnim(animProp);

	SaveToFile(L"c:/test.actor");
	GetStage()->SaveToFile(L"c:/test.scene");

	script->GetStack()->PushNull();
	return true;
}


} // namespace Wme

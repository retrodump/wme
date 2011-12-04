// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTaskActorMove_H__
#define __WmeTaskActorMove_H__


#include "MicroTask.h"


namespace Wme
{
	class Actor3D;
	class TaskActorTurn;

	class WmeDllExport TaskActorMove : public MicroTask
	{
	public:
		TaskActorMove(Actor3D* actor);
		~TaskActorMove();

		void OnActivate();
		void OnDeactivate();
		void OnUpdate();

		void SetTargetPoint(const Ogre::Vector3& val) { m_TargetPoint = val; }


	private:
		Actor3D* m_Actor;
		Ogre::Vector3 m_TargetPoint;
		int m_CurrentSubGoal;
		bool m_IsFollowingPath;

		TaskActorTurn* m_TurnSubTask;
	};
}

#endif // __WmeTaskActorMove_H__


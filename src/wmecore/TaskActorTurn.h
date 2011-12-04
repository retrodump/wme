// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTaskActorTurn_H__
#define __WmeTaskActorTurn_H__


#include "MicroTask.h"


namespace Wme
{
	class Actor3D;

	class WmeDllExport TaskActorTurn : public MicroTask
	{
	public:
		TaskActorTurn(Actor3D* actor);
		~TaskActorTurn();

		void OnActivate();
		void OnDeactivate();
		void OnUpdate();

		void SetTargetPoint(const Ogre::Vector3& val) { m_TargetPoint = val; }


	private:
		WideString GetAnimName() const;

		Actor3D* m_Actor;
		Ogre::Vector3 m_TargetPoint;
		
		Ogre::Degree m_CurrentAngle;
		Ogre::Degree m_TargetAngle;
		bool m_TurnClockwise;
	};
}

#endif // __WmeTaskActorTurn_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTaskActorAnim_H__
#define __WmeTaskActorAnim_H__


#include "MicroTask.h"
#include "AnimProp.h"


namespace Wme
{
	class Actor3D;

	class WmeDllExport TaskActorAnim : public MicroTask
	{
	public:
		TaskActorAnim(Actor3D* actor);
		~TaskActorAnim();

		void OnActivate();
		void OnDeactivate();
		void OnUpdate();

		void SetAnimProp(AnimProp& animProp) { m_AnimProp = animProp; }

	private:
		Actor3D* m_Actor;
		AnimProp m_AnimProp;
	};
}

#endif // __WmeTaskActorAnim_H__


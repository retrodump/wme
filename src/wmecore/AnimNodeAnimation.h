// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimNodeAnimation_H__
#define __WmeAnimNodeAnimation_H__


#include "AnimTreeNode.h"


namespace Wme
{
	class Animation;

	class AnimNodeAnimation : public AnimTreeNode
	{
	public:
		AnimNodeAnimation(AnimTree* parentTree);
		~AnimNodeAnimation();

		void Create();
		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

		void SetAnimation(Animation* anim);
		Animation* GetAnimation() const { return m_Animation; }

	private:
		Animation* m_Animation;
	};
}

#endif // __WmeAnimNodeAnimation_H__

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimNodeAnimation.h"
#include "Animation.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimNodeAnimation::AnimNodeAnimation(AnimTree* parentTree) : AnimTreeNode(parentTree)
{
	m_Animation = NULL;
}

//////////////////////////////////////////////////////////////////////////
AnimNodeAnimation::~AnimNodeAnimation()
{
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeAnimation::Create()
{

}

//////////////////////////////////////////////////////////////////////////
void AnimNodeAnimation::SetAnimation(Animation* anim)
{
	m_Animation = anim;
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeAnimation::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	if (m_Animation && !m_Animation->IsFinished())
	{
		m_Animation->IncreaseTotalWeight(weight);
		animNodes.push_back(this);
	}
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeAnimation::GetActiveAnimations(AnimNodeList& animNodes)
{
	if (m_Animation && !m_Animation->IsFinished())
	{
		animNodes.push_back(this);
	}
}


} // namespace Wme

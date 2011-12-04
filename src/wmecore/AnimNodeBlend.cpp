// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimNodeBlend.h"
#include "MathUtil.h"
#include "AnimTreePin.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimNodeBlend::AnimNodeBlend(AnimTree* parentTree) : AnimTreeNode(parentTree)
{
	m_PinA = m_PinB = NULL;
	m_BlendRatio = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
AnimNodeBlend::~AnimNodeBlend()
{
}


//////////////////////////////////////////////////////////////////////////
void AnimNodeBlend::Create()
{
	m_PinA = AddInputPin(L"animA");
	m_PinB = AddInputPin(L"animB");
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeBlend::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	m_BlendRatio = Ogre::Math::Clamp(m_BlendRatio, 0.0f, 1.0f);	
	float weightB = m_BlendRatio;
	float weightA = 1.0f - weightB;

	m_PinA->ProcessBlending(weightA, animNodes);
	m_PinB->ProcessBlending(weightB, animNodes);
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeBlend::GetActiveAnimations(AnimNodeList& animNodes)
{
	m_PinA->GetActiveAnimations(animNodes);
	m_PinB->GetActiveAnimations(animNodes);	
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimNodeInput.h"
#include "AnimTree.h"
#include "AnimTreePin.h"
#include "Animation.h"
#include "AnimNodeAnimation.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimNodeInput::AnimNodeInput(AnimTree* parentTree) : AnimTreeNode(parentTree)
{
}

//////////////////////////////////////////////////////////////////////////
AnimNodeInput::~AnimNodeInput()
{
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::Create()
{

}

//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	// remove finished animations
	PinVector finishedPins;

	foreach (AnimTreePin* pin, m_ActiveAnims)
	{
		AnimNodeList animNodes;
		pin->GetActiveAnimations(animNodes);

		bool allFinished = true;
		foreach (AnimNodeAnimation* animNode, animNodes)
		{
			if (!animNode->GetAnimation()->IsFinished()) allFinished = false;
		}

		if (allFinished)
		{
			m_ParentTree->DeleteNode(pin->GetConnection());
			finishedPins.push_back(pin);
		}
	}

	foreach (AnimTreePin* pin, finishedPins)
	{
		RemoveActivePin(pin);
		RemoveInputPin(pin);		
	}


	// process blending
	foreach (AnimTreePin* pin, m_ActiveAnims)
	{
		pin->ProcessBlending(weight, animNodes);
	}
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::GetActiveAnimations(AnimNodeList& animNodes)
{
	foreach (AnimTreePin* pin, m_ActiveAnims)
	{
		pin->GetActiveAnimations(animNodes);
	}
}


//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::Play(const WideString& animName, unsigned long fadeInTime, bool animateWhileFading)
{
	// prepare animation
	Animation* anim = m_ParentTree->GetAnimation(animName);
	if (!anim) return;

	anim->Reset();
	anim->Play(fadeInTime, animateWhileFading);

	// attach animation to input pin
	AnimNodeAnimation* animNode = m_ParentTree->AddAnimationNode(anim);
	AnimTreePin* pin = ClaimPin(animName);

	pin->SetConnection(animNode);
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::CrossFade(const WideString& animName, unsigned long fadeInTime, bool animateWhileFadingIn, bool animateWhileFadingOut)
{
	// finish all current animations
	foreach (AnimTreePin* pin, m_ActiveAnims)
	{
		AnimNodeList animNodes;
		pin->GetActiveAnimations(animNodes);

		foreach (AnimNodeAnimation* animNode, animNodes)
		{
			animNode->GetAnimation()->Stop(fadeInTime, animateWhileFadingOut);
		}
	}

	// and start the new animation
	Play(animName, fadeInTime, animateWhileFadingIn);
}

//////////////////////////////////////////////////////////////////////////
AnimTreePin* AnimNodeInput::ClaimPin(const WideString& name)
{
	// recycle existing pin with this name or create a new one
	AnimTreePin* pin = GetInputPin(name);
	if (pin)
	{
		if (pin->HasConnection()) m_ParentTree->DeleteNode(pin->GetConnection());
		pin->SetConnection(NULL);
		return pin;
	}
	else
	{
		pin = AddInputPin(name);
		m_ActiveAnims.push_back(pin);
		return pin;
	}
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeInput::RemoveActivePin(AnimTreePin* activePin)
{
	for (PinVector::iterator it = m_ActiveAnims.begin(); it != m_ActiveAnims.end(); it++)
	{
		if ((*it) == activePin)
		{
			m_ActiveAnims.erase(it);
			return;
		}
	}
}


} // namespace Wme
// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimNodeOutput.h"
#include "AnimTreePin.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimNodeOutput::AnimNodeOutput(AnimTree* parentTree) : AnimTreeNode(parentTree)
{
	m_InputPin = NULL;
}

//////////////////////////////////////////////////////////////////////////
AnimNodeOutput::~AnimNodeOutput()
{
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeOutput::Create()
{
	m_InputPin = AddInputPin(L"input");
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeOutput::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	m_InputPin->ProcessBlending(weight, animNodes);
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeOutput::GetActiveAnimations(AnimNodeList& animNodes)
{
	m_InputPin->GetActiveAnimations(animNodes);
}


} // namespace Wme

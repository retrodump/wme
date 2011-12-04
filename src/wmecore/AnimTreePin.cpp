// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimTreePin.h"
#include "AnimTreeNode.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimTreePin::AnimTreePin(const WideString& name, AnimTreeNode* ownerNode)
{
	m_Name = name;
	m_OwnerNode = ownerNode;
	m_ConnectedNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
AnimTreePin::~AnimTreePin()
{
}

//////////////////////////////////////////////////////////////////////////
void AnimTreePin::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	if (m_ConnectedNode) m_ConnectedNode->ProcessBlending(weight, animNodes);
}

//////////////////////////////////////////////////////////////////////////
void AnimTreePin::GetActiveAnimations(AnimNodeList& animNodes)
{
	if (m_ConnectedNode) m_ConnectedNode->GetActiveAnimations(animNodes);
}


} // namespace Wme

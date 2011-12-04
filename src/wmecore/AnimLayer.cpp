// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimLayer.h"
#include "AnimTreeNode.h"
#include "AnimTreePin.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimLayer::AnimLayer(AnimTreeNode* ownerNode, LayerType type)
{
	m_Type = type;
	m_InputPin = new AnimTreePin(L"", ownerNode);
	m_Weight = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
AnimLayer::~AnimLayer()
{
	SAFE_DELETE(m_InputPin);
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimTreeNode.h"
#include "AnimTreePin.h"
#include "AnimTree.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimTreeNode::AnimTreeNode(AnimTree* parentTree)
{
	m_ParentTree = parentTree;
}

//////////////////////////////////////////////////////////////////////////
AnimTreeNode::~AnimTreeNode()
{
	foreach (AnimTreePin* pin, m_InputPins)
	{
		SAFE_DELETE(pin);
	}
	m_InputPins.clear();

	m_PinNameMap.clear();
}

//////////////////////////////////////////////////////////////////////////
AnimTreePin* AnimTreeNode::AddInputPin(const WideString& name)
{
	AnimTreePin* pin = GetInputPin(name);
	if (pin) return pin;

	pin = new AnimTreePin(name, this);
	m_InputPins.push_back(pin);

	m_PinNameMap[name] = m_InputPins.size() - 1;

	return pin;
}

//////////////////////////////////////////////////////////////////////////
AnimTreePin* AnimTreeNode::GetInputPin(const WideString& name)
{
	PinNameMap::iterator it = m_PinNameMap.find(name);
	if (it == m_PinNameMap.end()) return NULL;

	return GetInputPin(it->second);
}

//////////////////////////////////////////////////////////////////////////
AnimTreePin* AnimTreeNode::GetInputPin(int id)
{
	if (id < 0 || id >= (int)m_InputPins.size()) return NULL;

	return m_InputPins[id];
}

//////////////////////////////////////////////////////////////////////////
void AnimTreeNode::RemoveInputPin(AnimTreePin* pin)
{
	if (!pin) return;

	PinNameMap::iterator it = m_PinNameMap.find(pin->GetName());
	if (it != m_PinNameMap.end()) m_PinNameMap.erase(it);

	for (PinVector::iterator it = m_InputPins.begin(); it != m_InputPins.end(); it++)
	{
		if ((*it) == pin)
		{
			m_InputPins.erase(it);
			delete pin;
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void AnimTreeNode::DeleteChildren()
{
	foreach (AnimTreePin* pin, m_InputPins)
	{
		if (pin->HasConnection()) m_ParentTree->DeleteNode(pin->GetConnection());
		pin->SetConnection(NULL);
	}
}


} // namespace Wme

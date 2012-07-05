// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiControl.h"
#include "UiObjectState.h"
#include "SceneNode2D.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiControl::UiControl(Canvas2D* canvas) : UiElement(canvas)
{
}

//////////////////////////////////////////////////////////////////////////
UiControl::~UiControl()
{
	foreach (StateMap::value_type state, m_States)
	{
		SAFE_DELETE(state.second);
	}
}

//////////////////////////////////////////////////////////////////////////
void UiControl::AddState(const WideString& stateName, UiObjectState* state)
{
	if (m_States.find(stateName) != m_States.end())
	{
		delete m_States[stateName];
		m_States.erase(stateName);
	}

	m_States[stateName] = state;
	state->GetSceneNode()->SetParentNode(NULL);
}

//////////////////////////////////////////////////////////////////////////
void UiControl::SetCurrentState(const WideString& stateName)
{
	if (stateName == m_CurrentState) return;

	if (m_States.find(stateName) == m_States.end())
	{
		Game::GetInstance()->Log(L"Invalid control state: " + stateName);
		return;
	}

	UiObjectState* newState = m_States[stateName];
	m_ElementsNode->RemoveAllChildren();
	m_ElementsNode->AddChild(newState->GetSceneNode());
}

//////////////////////////////////////////////////////////////////////////
void UiControl::OnSizeChanged()
{
	foreach (StateMap::value_type state, m_States)
	{
		state.second->SetSize(GetWidth(), GetHeight());
	}
}


} // namespace Wme

// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiButton.h"
#include "SceneNode2D.h"
#include "ContentManager.h"

// temp
#include "UiBorder.h"
#include "UiInteractiveArea.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiButton::UiButton(Canvas2D* canvas) : UiControl(canvas)
{
	m_IsBeingPressed = false;

	UiBorder* border = new UiBorder(canvas);
	border->SetImage(L"ui/button_normal.image");
	border->SetImage(L"ui/button_hover.image", L"hover");
	border->SetImage(L"ui/button_pressed.image", L"pressed");
	border->SetImage(L"ui/button_disabled.image", L"disabled");
	AddChild(border);
	border->FillParent();

	UiInteractiveArea* area = new UiInteractiveArea(canvas);
	AddChild(area);
	area->FillParent();
}

//////////////////////////////////////////////////////////////////////////
UiButton::~UiButton()
{
}

//////////////////////////////////////////////////////////////////////////
void UiButton::UpdateState()
{
	if (!IsEnabled()) SetCurrentState(L"disabled");
	else
	{
		if (IsMouseOver())
		{
			if (m_IsBeingPressed) SetCurrentState(L"pressed");
			else SetCurrentState(L"hover");
		}
		else SetCurrentState(L"normal");
	}
}

//////////////////////////////////////////////////////////////////////////
void UiButton::OnStateChanged()
{
	foreach (UiObjectBase* child, m_Children)
	{
		child->SetCurrentState(GetCurrentState());
	}
}

//////////////////////////////////////////////////////////////////////////
bool UiButton::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	if (!IsEnabled()) return false;

	if (event.GetButton() == MouseEvent::BUTTON_LEFT)
	{
		if (event.GetType() == MouseEvent::EVENT_PRESS)
		{
			Game::GetInstance()->GetContentMgr()->CaptureMouse(this, viewport);
			m_IsBeingPressed = true;
		}
		else if (event.GetType() == MouseEvent::EVENT_RELEASE)
		{
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(this);
			if (m_IsBeingPressed)
			{
				if (IsMouseOver())
				{
					//if (m_TwoStateButton) m_IsPressed = !m_IsPressed;
					//InvokeEvent(L"press");
				}
			}
			m_IsBeingPressed = false;
		}
		UpdateState();
	}

	return true;
}



} // namespace Wme

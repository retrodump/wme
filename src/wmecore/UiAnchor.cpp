// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiAnchor.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiAnchor::UiAnchor(UiObjectBase* owner, UiObjectBase* target, UiObjectBase::AnchorType type, float margin)
{
	m_Owner = owner;
	m_Target = target;
	m_Type = type;
	m_Margin = margin;

	if (m_Target) m_Target->AddListener(this);

	m_UpdateInProgress = false;

	OnGeometryChanged();
}

//////////////////////////////////////////////////////////////////////////
UiAnchor::~UiAnchor()
{
	if (m_Target) m_Target->RemoveListener(this);
}

//////////////////////////////////////////////////////////////////////////
// UiObjectBase::Listener
//////////////////////////////////////////////////////////////////////////
void UiAnchor::OnGeometryChanged()
{
	if (!m_Target || m_Type == UiObjectBase::ANCHOR_NONE) return;
	if (m_UpdateInProgress) return;

	m_UpdateInProgress = true;

	if (m_Owner->IsSibling(m_Target)) UpdateFromSibling();
	else if (m_Owner->IsParent(m_Target)) UpdateFromParent();

	m_UpdateInProgress = false;
}

//////////////////////////////////////////////////////////////////////////
void UiAnchor::OnDestroy()
{
	m_Target = NULL;
	m_Type = UiObjectBase::ANCHOR_NONE;
}

//////////////////////////////////////////////////////////////////////////
void UiAnchor::UpdateFromSibling()
{
	switch (m_Type)
	{
	case UiObjectBase::ANCHOR_LEFT:
		m_Owner->SetLeft(m_Target->GetRight() + m_Margin);
		break;
	case UiObjectBase::ANCHOR_RIGHT:
		m_Owner->SetRight(m_Target->GetLeft() - m_Margin);
		break;
	case UiObjectBase::ANCHOR_TOP:
		m_Owner->SetTop(m_Target->GetBottom() + m_Margin);
		break;
	case UiObjectBase::ANCHOR_BOTTOM:
		m_Owner->SetBottom(m_Target->GetTop() - m_Margin);
		break;
	case UiObjectBase::ANCHOR_VERTICAL_CENTER:
		m_Owner->SetVerticalCenter(m_Target->GetVerticalCenter() + m_Margin);
		break;
	case UiObjectBase::ANCHOR_HORIZONTAL_CENTER:
		m_Owner->SetHorizontalCenter(m_Target->GetHorizontalCenter() + m_Margin);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void UiAnchor::UpdateFromParent()
{
	switch (m_Type)
	{
	case UiObjectBase::ANCHOR_LEFT:
		m_Owner->SetLeft(m_Margin);
		break;
	case UiObjectBase::ANCHOR_RIGHT:
		m_Owner->SetRight(m_Target->GetWidth() - m_Margin);
		break;
	case UiObjectBase::ANCHOR_TOP:
		m_Owner->SetTop(m_Margin);
		break;
	case UiObjectBase::ANCHOR_BOTTOM:
		m_Owner->SetBottom(m_Target->GetHeight() - m_Margin);
		break;
	case UiObjectBase::ANCHOR_VERTICAL_CENTER:
		m_Owner->SetVerticalCenter(m_Target->GetHeight() / 2.0f + m_Margin);
		break;
	case UiObjectBase::ANCHOR_HORIZONTAL_CENTER:
		m_Owner->SetHorizontalCenter(m_Target->GetWidth() / 2.0f + m_Margin);
		break;
	}
}


} // namespace Wme

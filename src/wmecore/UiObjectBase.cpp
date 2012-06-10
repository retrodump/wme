// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObjectBase.h"
#include "Canvas2D.h"
#include "SceneNode2D.h"
#include "UiAnchor.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiObjectBase::UiObjectBase(Canvas2D* canvas)
{
	m_Canvas = canvas;
	m_SceneNode = m_Canvas->GetRootNode()->CreateChildNode();
	m_ElementsNode = m_SceneNode->CreateChildNode();
	m_ChildrenNode = m_SceneNode->CreateChildNode();
	m_ClipChildren = false;
	m_Width = m_Height = 0.0f;

	m_LeftAnchor = m_RightAnchor = m_TopAnchor = m_BottomAnchor = m_VerticalCenterAnchor = m_HorizontalCenterAnchor = NULL;

	m_Parent = NULL;
}

//////////////////////////////////////////////////////////////////////////
UiObjectBase::~UiObjectBase()
{
	foreach (Listener* listener, m_Listeners) listener->OnDestroy();

	SAFE_DELETE(m_LeftAnchor);
	SAFE_DELETE(m_RightAnchor);
	SAFE_DELETE(m_TopAnchor);
	SAFE_DELETE(m_BottomAnchor);
	SAFE_DELETE(m_VerticalCenterAnchor);
	SAFE_DELETE(m_HorizontalCenterAnchor);

	foreach (UiObjectBase* child, m_Children)
	{
		SAFE_DELETE(child);
	}
	m_Children.clear();

	m_ElementsNode->RemoveAndDestroyAllChildren(true);
	SAFE_DELETE(m_ElementsNode);

	m_ChildrenNode->RemoveAndDestroyAllChildren(true);
	SAFE_DELETE(m_ChildrenNode);

	SAFE_DELETE(m_SceneNode);
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetParent(UiObjectBase* parent)
{
	if (parent == m_Parent) return;

	if (m_Parent)
	{
		m_Parent->m_Children.remove(this);
	}

	m_Parent = parent;
	if (m_Parent)
	{
		m_SceneNode->SetParentNode(m_Parent->GetChildrenNode());
		m_Parent->m_Children.push_back(this);
		m_Parent->UpdateChildrenZOrder();
	}
	else
	{
		m_SceneNode->SetParentNode(m_Canvas->GetRootNode());
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetClipChildren(bool clipChildren)
{
	m_ClipChildren = clipChildren;
	UpdateClippingRect();
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetPosX() const
{
	return m_SceneNode->GetPosition().x;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetPosX(float posX)
{
	m_SceneNode->SetPosition(posX, GetPosY());
	InvokeGeometryChanged();
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetPosY() const
{
	return m_SceneNode->GetPosition().y;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetPosY(float posY)
{
	m_SceneNode->SetPosition(GetPosX(), posY);
	InvokeGeometryChanged();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetWidth(float width)
{
	if (width != m_Width)
	{
		m_Width = width;
		OnSizeChanged();
		UpdateClippingRect();
		InvokeGeometryChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetHeight(float height)
{
	if (height != m_Height)
	{
		m_Height = height;
		OnSizeChanged();
		UpdateClippingRect();
		InvokeGeometryChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetLeft() const
{
	return GetPosX();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetLeft(float left)
{
	if (HasAnchor(ANCHOR_RIGHT))
		SetWidth(GetWidth() + GetPosX() - left);

	SetPosX(left);
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetRight() const
{
	return GetPosX() + GetWidth();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetRight(float right)
{
	if (HasAnchor(ANCHOR_LEFT))
	{
		SetWidth(GetWidth() + right - GetRight());
	}
	else
	{
		SetPosX(right - GetWidth());
	}
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetTop() const
{
	return GetPosY();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetTop(float top)
{
	if (HasAnchor(ANCHOR_BOTTOM))
		SetHeight(GetHeight() + GetPosY() - top);

	SetPosY(top);
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetBottom() const
{
	return GetPosY() + GetHeight();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetBottom(float bottom)
{
	if (HasAnchor(ANCHOR_TOP))
	{
		SetHeight(GetHeight() + bottom - GetBottom());
	}
	else
	{
		SetPosY(bottom - GetHeight());
	}
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetVerticalCenter() const
{
	return GetPosY() + GetHeight() / 2.0f;	
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetVerticalCenter(float verticalCenter)
{
	SetPosY(verticalCenter - GetHeight() / 2.0f);
}

//////////////////////////////////////////////////////////////////////////
float UiObjectBase::GetHorizontalCenter() const
{
	return GetPosX() + GetWidth() / 2.0f;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetHorizontalCenter(float horizontalCenter)
{
	SetPosX(horizontalCenter - GetWidth() / 2.0f);
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::IsVisible() const
{
	return m_SceneNode->IsVisible();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetVisible(bool visible)
{
	m_SceneNode->SetVisible(visible);
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::IsEnabled() const
{
	if (!m_Enabled) return false;
	else
	{
		if (m_Parent) return m_Parent->IsEnabled();
		else return true;
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetEnabled(bool enabled)
{
	if (enabled != m_Enabled)
	{
		m_Enabled = enabled;
		OnEnabledChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::AddChild(UiObjectBase* child)
{
	if (!child || child == this) return;
	child->SetParent(this);
	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::RemoveChild(UiObjectBase* child)
{
	if (!child || child->GetParent() != this) return;
	child->SetParent(NULL);
	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::MoveChildAfter(UiObjectBase* child, UiObjectBase* pos)
{
	UiObjectList::iterator it;

	it = std::find(m_Children.begin(), m_Children.end(), child);
	if (it != m_Children.end()) m_Children.erase(it);

	it = std::find(m_Children.begin(), m_Children.end(), pos);
	if (it != m_Children.end())
	{
		it++;
		m_Children.insert(it, child);
	}
	else m_Children.push_back(child);

	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::MoveChildBefore(UiObjectBase* child, UiObjectBase* pos)
{
	UiObjectList::iterator it;

	it = std::find(m_Children.begin(), m_Children.end(), child);
	if (it != m_Children.end()) m_Children.erase(it);

	it = std::find(m_Children.begin(), m_Children.end(), pos);
	if (it != m_Children.end()) m_Children.insert(it, child);
	else m_Children.push_front(child);

	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
UiObjectBase* UiObjectBase::GetChild(int index)
{
	foreach (UiObjectBase* child, m_Children)
	{
		if (child->GetSceneNode()->GetZOrder() == index) return child;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
UiObjectBase* UiObjectBase::GetChild(const WideString& name)
{
	foreach (UiObjectBase* child, m_Children)
	{
		if (child->GetName() == name) return child;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::UpdateChildrenZOrder()
{
	size_t order = 0;
	foreach (UiObjectBase* child, m_Children)
	{
		child->GetSceneNode()->SetZOrder(order);
		order++;
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::UpdateClippingRect()
{
	if (m_ClipChildren)
	{
		m_ChildrenNode->SetClippingRect(Rect(0, 0, GetWidth(), GetHeight()));
	}
	else
	{
		m_ChildrenNode->SetClippingRect(Rect::EMPTY_RECT);
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::InvokeGeometryChanged() const
{
	foreach (Listener* listener, m_Listeners) listener->OnGeometryChanged();
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::AddListener(Listener* listener)
{
	if (std::find(m_Listeners.begin(), m_Listeners.end(), listener) == m_Listeners.end())
		m_Listeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::RemoveListener(Listener* listener)
{
	ListenerList::iterator it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if (it != m_Listeners.end()) m_Listeners.erase(it);
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::HasChild(UiObjectBase* object) const
{
	if (!object) return false;
	return std::find(m_Children.begin(), m_Children.end(), object) != m_Children.end();
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::IsSibling(UiObjectBase* object) const
{
	if (!object) return false;
	return object->GetParent() == m_Parent;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::IsParent(UiObjectBase* object) const
{
	return object != NULL && object == m_Parent;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::SetAnchor(AnchorType type, UiObjectBase* target, float margin)
{
	// TODO checks
	switch (type)
	{
	case ANCHOR_LEFT:
		SAFE_DELETE(m_LeftAnchor);
		SAFE_DELETE(m_HorizontalCenterAnchor);
		if (target) m_LeftAnchor = new UiAnchor(this, target, type, margin);
		return true;
	
	case ANCHOR_RIGHT:
		SAFE_DELETE(m_RightAnchor);
		SAFE_DELETE(m_HorizontalCenterAnchor);
		if (target) m_RightAnchor = new UiAnchor(this, target, type, margin);
		return true;
	
	case ANCHOR_TOP:
		SAFE_DELETE(m_TopAnchor);
		SAFE_DELETE(m_VerticalCenterAnchor);
		if (target) m_TopAnchor = new UiAnchor(this, target, type, margin);
		return true;
	
	case ANCHOR_BOTTOM:
		SAFE_DELETE(m_BottomAnchor);
		SAFE_DELETE(m_VerticalCenterAnchor);
		if (target) m_BottomAnchor = new UiAnchor(this, target, type, margin);
		return true;

	case ANCHOR_VERTICAL_CENTER:
		SAFE_DELETE(m_TopAnchor);
		SAFE_DELETE(m_BottomAnchor);
		SAFE_DELETE(m_VerticalCenterAnchor);
		if (target) m_VerticalCenterAnchor = new UiAnchor(this, target, type, margin);
		return true;

	case ANCHOR_HORIZONTAL_CENTER:
		SAFE_DELETE(m_LeftAnchor);
		SAFE_DELETE(m_RightAnchor);
		SAFE_DELETE(m_HorizontalCenterAnchor);
		if (target) m_HorizontalCenterAnchor = new UiAnchor(this, target, type, margin);
		return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::GetAnchor(AnchorType type, UiObjectBase*& target, float& margin) const
{
	UiAnchor* anchor = NULL;

	switch (type)
	{
	case ANCHOR_LEFT:
		if (m_LeftAnchor) anchor = m_LeftAnchor;
		break;

	case ANCHOR_RIGHT:
		if (m_RightAnchor) anchor = m_RightAnchor;
		break;

	case ANCHOR_TOP:
		if (m_TopAnchor) anchor = m_TopAnchor;
		break;

	case ANCHOR_BOTTOM:
		if (m_BottomAnchor) anchor = m_BottomAnchor;
		break;

	case ANCHOR_VERTICAL_CENTER:
		if (m_VerticalCenterAnchor) anchor = m_VerticalCenterAnchor;
		break;

	case ANCHOR_HORIZONTAL_CENTER:
		if (m_HorizontalCenterAnchor) anchor = m_HorizontalCenterAnchor;
		break;
	}

	if (anchor && anchor->GetType() != ANCHOR_NONE)
	{
		target = anchor->GetTarget();
		margin = anchor->GetMargin();
		return true;
	}
	else
	{
		target = NULL;
		margin = 0.0f;
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool UiObjectBase::HasAnchor(AnchorType type) const
{
	UiAnchor* anchor = NULL;

	switch (type)
	{
	case ANCHOR_LEFT:
		if (m_LeftAnchor) anchor = m_LeftAnchor;
		break;

	case ANCHOR_RIGHT:
		if (m_RightAnchor) anchor = m_RightAnchor;
		break;

	case ANCHOR_TOP:
		if (m_TopAnchor) anchor = m_TopAnchor;
		break;

	case ANCHOR_BOTTOM:
		if (m_BottomAnchor) anchor = m_BottomAnchor;
		break;

	case ANCHOR_VERTICAL_CENTER:
		if (m_VerticalCenterAnchor) anchor = m_VerticalCenterAnchor;
		break;

	case ANCHOR_HORIZONTAL_CENTER:
		if (m_HorizontalCenterAnchor) anchor = m_HorizontalCenterAnchor;
		break;
	}

	return (anchor && anchor->GetType() != ANCHOR_NONE);
}


} // namespace Wme

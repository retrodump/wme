// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObjectBase.h"
#include "Canvas2D.h"
#include "SceneNode2D.h"


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

	m_Parent = NULL;
}

//////////////////////////////////////////////////////////////////////////
UiObjectBase::~UiObjectBase()
{
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
}

//////////////////////////////////////////////////////////////////////////
void UiObjectBase::SetWidth(float width)
{
	if (width != m_Width)
	{
		m_Width = width;
		OnSizeChanged();
		UpdateClippingRect();
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
	}
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


} // namespace Wme

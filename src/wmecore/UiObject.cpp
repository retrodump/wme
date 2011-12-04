// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObject.h"
#include "UiObjectFactory.h"
#include "ContentManager.h"
#include "ElementCollection.h"
#include "Sprite.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiObject::UiObject(GuiStage* parentStage)
{
	m_ParentStage = parentStage;
	m_Parent = NULL;
	m_ZOrder = 0;

	m_PixelPerfect = false;
	m_Disabled = false;
	m_Visible = true;

	m_PosX = m_PosY = 0;
	m_Width = m_Height = 0;
}

//////////////////////////////////////////////////////////////////////////
UiObject::~UiObject()
{
	foreach (UiObject* child, m_Children)
	{
		SAFE_DELETE(child);
	}
	m_Children.clear();
}

//////////////////////////////////////////////////////////////////////////
void UiObject::Create()
{
}

//////////////////////////////////////////////////////////////////////////
void UiObject::Display(ElementCollection* elementCol, const SpriteDrawingParams& params)
{
	foreach (UiObject* child, m_Children)
	{
		child->Display(elementCol, params);
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObject::Update()
{
	foreach (UiObject* child, m_Children)
	{
		child->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObject::AddChild(UiObject* child)
{
	UiObjectList::iterator it = std::find(m_Children.begin(), m_Children.end(), child);
	if (it != m_Children.end()) return;

	m_Children.push_back(child);
	child->SetParent(this);
	
	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
void UiObject::RemoveChild(UiObject* child)
{
	UiObjectList::iterator it = std::find(m_Children.begin(), m_Children.end(), child);
	if (it != m_Children.end())
	{
		(*it)->SetParent(NULL);
		m_Children.erase(it);
	}

	UpdateChildrenZOrder();
}

//////////////////////////////////////////////////////////////////////////
void UiObject::MoveChildAfter(UiObject* child, UiObject* pos)
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
void UiObject::MoveChildBefore(UiObject* child, UiObject* pos)
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
UiObject* UiObject::GetChild(size_t index)
{
	foreach (UiObject* child, m_Children)
	{
		if (child->GetZOrder() == index) return child;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
UiObject* UiObject::GetChild(const WideString& name)
{
	foreach (UiObject* child, m_Children)
	{
		if (child->GetName() == name) return child;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void UiObject::UpdateChildrenZOrder()
{
	size_t order = 0;
	foreach (UiObject* child, m_Children)
	{
		child->SetZOrder(order);
		order++;
	}
}

//////////////////////////////////////////////////////////////////////////
void UiObject::GetOffset(int& offsetX, int& offsetY) const
{
	if (m_Parent) m_Parent->GetOffset(offsetX, offsetY);
	offsetX += m_PosX;
	offsetY += m_PosY;
}

//////////////////////////////////////////////////////////////////////////
int UiObject::GetAbsoluteX() const
{
	int posX = 0;
	int posY = 0;

	GetOffset(posX, posY);
	return posX;
}

//////////////////////////////////////////////////////////////////////////
int UiObject::GetAbsoluteY() const
{
	int posX = 0;
	int posY = 0;

	GetOffset(posX, posY);
	return posY;
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool UiObject::LoadFromXml(TiXmlElement* rootNode)
{
	ScriptableObject::LoadFromXml(rootNode);

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Position")
		{
			m_PosX = XmlUtil::AttrToInt(elem, "X", 0);
			m_PosY = XmlUtil::AttrToInt(elem, "Y", 0);
		}
		else if (elem->ValueStr() == "Size")
		{
			m_Width = XmlUtil::AttrToInt(elem, "Width", 0);
			m_Height = XmlUtil::AttrToInt(elem, "Height", 0);
		}
		else if (elem->ValueStr() == "PixelPerfect")
		{
			m_PixelPerfect = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "Disabled")
		{
			m_Disabled = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "Visible")
		{
			m_Visible = XmlUtil::TextToBool(elem, false);
		}
		else if (elem->ValueStr() == "Text")
		{
			m_Text = XmlUtil::TextToString(elem);
		}
		// load children
		else if (elem->ValueStr() == "Children")
		{
			for (TiXmlElement* childNode = elem->FirstChildElement(); childNode != NULL; childNode = childNode->NextSiblingElement())
			{
				UiObject* child = UiObjectFactory::GetInstance()->CreateInstance(m_ParentStage, childNode->ValueStr());
				if (!child) continue;

				child->Create();
				child->LoadFromXml(childNode);

				AddChild(child);
			}
		}
	}

	UpdateChildrenZOrder();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool UiObject::SaveToXml(TiXmlElement* rootNode)
{
	ScriptableObject::SaveToXml(rootNode);

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Position", rootNode);
	XmlUtil::SetAttr(elem, "X", m_PosX);
	XmlUtil::SetAttr(elem, "Y", m_PosY);

	elem = XmlUtil::AddElem("Size", rootNode);
	XmlUtil::SetAttr(elem, "Width", m_Width);
	XmlUtil::SetAttr(elem, "Height", m_Height);

	elem = XmlUtil::AddElem("PixelPerfect", rootNode);
	XmlUtil::SetText(elem, m_PixelPerfect);

	elem = XmlUtil::AddElem("Disabled", rootNode);
	XmlUtil::SetText(elem, m_Disabled);

	elem = XmlUtil::AddElem("Visible", rootNode);
	XmlUtil::SetText(elem, m_Visible);

	elem = XmlUtil::AddElem("Text", rootNode);
	XmlUtil::SetText(elem, m_Text, true);


	// save children
	elem = XmlUtil::AddElem("Children", rootNode);
	foreach (UiObject* child, m_Children)
	{
		TiXmlElement* childNode = XmlUtil::AddElem(child->GetDocRootName(), elem);
		child->SaveToXml(childNode);
	}


	return true;
}



} // namespace Wme

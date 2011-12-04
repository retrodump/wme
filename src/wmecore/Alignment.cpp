// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Alignment.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Alignment::Alignment()
{
	m_Horizontal = HA_LEFT;
	m_Vertical = VA_TOP;
}

//////////////////////////////////////////////////////////////////////////
Alignment::Alignment(Horizontal horizontal, Vertical vertical)
{
	m_Horizontal = horizontal;
	m_Vertical = vertical;
}

//////////////////////////////////////////////////////////////////////////
Alignment::~Alignment()
{

}

//////////////////////////////////////////////////////////////////////////
Alignment::Alignment(const Alignment& a)
{
	Clone(a);
}

//////////////////////////////////////////////////////////////////////////
Alignment& Alignment::operator= (const Alignment& a)
{
	if (this == &a) return *this;
	Clone(a);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
bool Alignment::operator!= (const Alignment& a) const
{
	return m_Horizontal != a.GetHorizontal() || m_Vertical != a.GetVertical();
}

//////////////////////////////////////////////////////////////////////////
bool Alignment::operator== (const Alignment& a) const
{
	return m_Horizontal == a.GetHorizontal() && m_Vertical == a.GetVertical();
}

//////////////////////////////////////////////////////////////////////////
void Alignment::Clone(const Alignment& a)
{
	m_Vertical = a.GetVertical();
	m_Horizontal = a.GetHorizontal();
}

//////////////////////////////////////////////////////////////////////////
void Alignment::SetAlignment(Horizontal horizontal, Vertical vertical)
{
	m_Horizontal = horizontal;
	m_Vertical = vertical;
}

//////////////////////////////////////////////////////////////////////////
bool Alignment::LoadFromXml(TiXmlElement* node)
{
	WideString str;

	str = XmlUtil::AttrToString(node, "Horizontal");
	if (str == L"Left") m_Horizontal = HA_LEFT;
	else if (str == L"Right") m_Horizontal = HA_RIGHT;
	else m_Horizontal = HA_CENTER;

	str = XmlUtil::AttrToString(node, "Vertical");
	if (str == L"Top") m_Vertical = VA_TOP;
	else if (str == L"Bottom") m_Vertical = VA_BOTTOM;
	else m_Vertical = VA_CENTER;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Alignment::SaveToXml(TiXmlElement* node)
{
	WideString str;

	switch (m_Horizontal)
	{
	case HA_LEFT:
		str = L"Left";
		break;
	case HA_RIGHT:
		str = L"Right";
		break;
	case HA_CENTER:
		str = L"Center";
		break;
	default:
		str.clear();
	}
	XmlUtil::SetAttr(node, "Horizontal", str);

	switch (m_Vertical)
	{
	case VA_TOP:
		str = L"Left";
		break;
	case VA_BOTTOM:
		str = L"Bottom";
		break;
	case VA_CENTER:
		str = L"Center";
		break;
	default:
		str.clear();
	}
	XmlUtil::SetAttr(node, "Vertical", str);

	return true;
}

} // namespace Wme
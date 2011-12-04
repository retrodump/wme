// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "Navigator.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
Navigator::Navigator(QWidget* parent) : QWidget(parent)
{
	m_CurrentNav = m_CurrentNavParent = NULL;

	m_Layout = new QVBoxLayout(this);
	m_Layout->setMargin(0);
}

//////////////////////////////////////////////////////////////////////////
Navigator::~Navigator()
{
	SetNavigator(NULL);
}

//////////////////////////////////////////////////////////////////////////
void Navigator::SetNavigator(QWidget* nav)
{
	if (m_CurrentNav)
	{
		m_Layout->removeWidget(m_CurrentNav);
		if (m_CurrentNavParent) m_CurrentNav->setParent(m_CurrentNavParent);
	}
	if (nav)
	{
		m_CurrentNavParent = nav->parentWidget();
		m_CurrentNav = nav;
		m_Layout->addWidget(nav);
	}
	else
	{
		m_CurrentNav = m_CurrentNavParent = NULL;
	}
	
}


} // namespace Armed

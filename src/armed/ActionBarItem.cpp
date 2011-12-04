// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ActionBarItem.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ActionBarItem::ActionBarItem(QObject* parent, const QString& actionName, const QString& caption) : QObject(parent)
{
	m_Caption = caption;
	m_ActionName = actionName;
}

//////////////////////////////////////////////////////////////////////////
ActionBarItem::~ActionBarItem()
{
	qDeleteAll(m_Children);
}

//////////////////////////////////////////////////////////////////////////
ActionBarItem* ActionBarItem::AddItem(const QString& actionName)
{
	ActionBarItem* item = new ActionBarItem(this, actionName, "");
	m_Children.append(item);
	return item;
}

//////////////////////////////////////////////////////////////////////////
ActionBarItem* ActionBarItem::AddSub(const QString& name, const QString& caption)
{
	ActionBarItem* item = new ActionBarItem(this, name, caption);
	m_Children.append(item);
	return item;
}


} // namespace Armed

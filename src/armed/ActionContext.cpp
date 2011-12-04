// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ActionContext.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ActionContext::ActionContext(QObject* parent) : QObject(parent)
{

}

//////////////////////////////////////////////////////////////////////////
ActionContext::~ActionContext()
{

}

//////////////////////////////////////////////////////////////////////////
void ActionContext::SetAction(QAction* action)
{
	if (action) m_ActionMap[action->parent()] = action;
}

//////////////////////////////////////////////////////////////////////////
void ActionContext::RemoveActions(QObject* object)
{
	m_ActionMap.remove(object);
}

//////////////////////////////////////////////////////////////////////////
QAction* ActionContext::GetAction(QObject* object)
{
	if (m_ActionMap.contains(object)) return m_ActionMap[object];
	else return NULL;
}


} // namespace Armed

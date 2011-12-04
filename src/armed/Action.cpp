// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "Action.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
Action::Action(QObject* parent) : QObject(parent)
{

}

//////////////////////////////////////////////////////////////////////////
Action::~Action()
{

}

//////////////////////////////////////////////////////////////////////////
void Action::SetActionForContext(QAction* action, ActionContext::Type contextType)
{
	ActionContext* context;

	if (!m_Contexts.contains(contextType))
	{
		if (action == NULL) return;
		context = new ActionContext(this);
		m_Contexts[contextType] = context;
	}
	else
	{
		context = m_Contexts[contextType];
	}
	context->SetAction(action);
}

//////////////////////////////////////////////////////////////////////////
void Action::RemoveActions(QObject* object)
{
	qforeach (ActionContext* context, m_Contexts)
	{
		context->RemoveActions(object);
	}
}

//////////////////////////////////////////////////////////////////////////
QAction* Action::GetAction(ActionContext::Type contextType, QObject* object)
{
	if (!m_Contexts.contains(contextType)) return NULL;
	return m_Contexts[contextType]->GetAction(object);
}


} // namespace Armed

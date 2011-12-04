// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ActionManager.h"
#include "Action.h"
#include "ActionBar.h"


namespace Armed
{


ActionManager* ActionManager::s_Instance = NULL;

//////////////////////////////////////////////////////////////////////////
ActionManager::ActionManager(QObject* parent) : QObject(parent)
{
	s_Instance = this;
	m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
ActionManager::~ActionManager()
{

}

//////////////////////////////////////////////////////////////////////////
ActionManager* ActionManager::GetInstance()
{
	return s_Instance;
}

//////////////////////////////////////////////////////////////////////////
Action* ActionManager::AddAction(const QString& actionName, QAction* action, ActionContext::Type context)
{
	Action* newAction;

	if (m_NamedActions.contains(actionName))
	{
		newAction = m_NamedActions[actionName];
	}
	else
	{
		newAction = new Action(this);
		m_NamedActions[actionName] = newAction;
	}

	newAction->SetActionForContext(action, context);
	SetDirty(true);
	
	return newAction;
}

//////////////////////////////////////////////////////////////////////////
void ActionManager::RemoveActions(QObject* object)
{
	qforeach (Action* action, m_NamedActions)
	{
		action->RemoveActions(object);
	}
	SetDirty(true);
}

//////////////////////////////////////////////////////////////////////////
void ActionManager::SetContextObject(ActionContext::Type contextType, QObject* object)
{
	if (m_ContextObjects[contextType] != object)
	{
		SetDirty(true);
		m_ContextObjects[contextType] = object;
	}
}

//////////////////////////////////////////////////////////////////////////
QAction* ActionManager::GetAction(const QString& actionName) const
{
	if (!m_NamedActions.contains(actionName)) return NULL;

	Action* action = m_NamedActions[actionName];
	
	for (int i = ActionContext::NUM_CONTEXTS; i >= 0; i--)
	{
		if (!m_ContextObjects.contains((ActionContext::Type)i)) continue;
		
		QObject* object = m_ContextObjects[(ActionContext::Type)i];
		if (!object) continue;

		QAction* nativeAction = action->GetAction((ActionContext::Type)i, object);
		if (nativeAction) return nativeAction;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void ActionManager::AddActionBar(ActionBar* actionBar)
{
	if (!m_ActionBars.contains(actionBar))
	{
		m_ActionBars.append(actionBar);
		SetDirty(true);
	}
}

//////////////////////////////////////////////////////////////////////////
void ActionManager::SetDirty(bool dirty)
{
	if (dirty)
	{
		if (!m_IsDirty)
		{
			m_IsDirty = true;
			QTimer::singleShot(0, this, SLOT(RefreshActionBars()));			
		}
	}
	else m_IsDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void ActionManager::RefreshActionBars()
{
	qforeach (ActionBar* actionBar, m_ActionBars)
	{
		actionBar->Rebuild();
	}
	SetDirty(false);
}


//////////////////////////////////////////////////////////////////////////
void ActionManager::ClearActionBars()
{
	qforeach (ActionBar* actionBar, m_ActionBars)
	{
		actionBar->Clear();
	}
	SetDirty(false);
}


} // namespace Armed
